/*
 *************************************************************
 *                      NoNameRc                           *
 *                                                           *
 *             -> Build your DIY MEGA 2560 TX                *
 *                                                           *
 *      Based on code named                                  *
 *      OpenTx - https://github.com/opentx/opentx            *
 *                                                           *
 *         Only avr code here for lisibility ;-)             *
 *                                                           *
 *  License GPLv2: http://www.gnu.org/licenses/gpl-2.0.html  *
 *                                                           *
 *************************************************************
 */

#include "../NoNameRc.h"
//#include "telemetry.h"

uint8_t frskyStreaming = 0;

#if defined(WS_HOW_HIGH)
uint8_t frskyUsrStreaming = 0;
#endif

uint8_t link_counter = 0;

#define FRSKY_RX_PACKET_SIZE   19
uint8_t frskyRxBuffer[FRSKY_RX_PACKET_SIZE];   // Receive buffer. 9 bytes (full packet), worst case 18 bytes with byte-stuffing (+1)

uint8_t frskyTxBuffer[FRSKY_TX_PACKET_SIZE];

uint8_t frskyTxBufferCount = 0;



uint8_t frskyRxBufferCount = 0;

FrskyData frskyData;

#define IS_FRSKY_D_PROTOCOL()     (true)
#define IS_FRSKY_SPORT_PROTOCOL() (false)


void FrskyValueWithMin::set(uint8_t value)
{
  if (this->value == 0) {
    this->value = value;
  }
  else {
    sum += value;
    if (link_counter == 0) {
      this->value = sum / (IS_FRSKY_D_PROTOCOL() ? FRSKY_D_AVERAGING : FRSKY_SPORT_AVERAGING);
      sum = 0;
    }
  }

  if (!min || value < min) {
    min = value;
  }
}

void FrskyValueWithMinMax::set(uint8_t value, uint8_t unit)
{
  FrskyValueWithMin::set(value);
  if (unit != UNIT_VOLTS) {
    this->value = value;
  }
  if (!max || value > max) {
    max = value;
  }
}

uint16_t getChannelRatio(source_t channel)
{
  return (uint16_t)g_model.frsky.channels[channel].ratio << g_model.frsky.channels[channel].multiplier;
}

lcdint_t applyChannelRatio(source_t channel, lcdint_t val)
{
  return ((int32_t)val+g_model.frsky.channels[channel].offset) * getChannelRatio(channel) * 2 / 51;
}

#if defined(TELEMETREZ)
#define PRIVATE         0x1B
uint8_t privateDataLen;
uint8_t privateDataPos;
#endif

#if defined(ROTARY_ENCODER_NAVIGATION) && defined(TELEMETREZ)
extern uint8_t TrotCount;
extern uint8_t TezRotary;
#endif

NOINLINE void processSerialData(uint8_t data)
{
  static uint8_t dataState = STATE_DATA_IDLE;

#if defined(BLUETOOTH)
  // TODO if (g_model.bt_telemetry)
  btPushByte(data);
#endif



  switch (dataState)
  {
    case STATE_DATA_START:
      if (data == START_STOP) {
        if (IS_FRSKY_SPORT_PROTOCOL()) {
          dataState = STATE_DATA_IN_FRAME ;
          frskyRxBufferCount = 0;
        }
      }
      else {
        if (frskyRxBufferCount < FRSKY_RX_PACKET_SIZE) {
          frskyRxBuffer[frskyRxBufferCount++] = data;
        }
        dataState = STATE_DATA_IN_FRAME;
      }
      break;

    case STATE_DATA_IN_FRAME:
      if (data == BYTESTUFF) {
        dataState = STATE_DATA_XOR; // XOR next byte
      }
      else if (data == START_STOP) {
        if (IS_FRSKY_SPORT_PROTOCOL()) {
          dataState = STATE_DATA_IN_FRAME ;
          frskyRxBufferCount = 0;
        }
        else {
          // end of frame detected
          frskyDProcessPacket(frskyRxBuffer);
          dataState = STATE_DATA_IDLE;
        }
        break;
      }
      else if (frskyRxBufferCount < FRSKY_RX_PACKET_SIZE) {
        frskyRxBuffer[frskyRxBufferCount++] = data;
      }
      break;

    case STATE_DATA_XOR:
      if (frskyRxBufferCount < FRSKY_RX_PACKET_SIZE) {
        frskyRxBuffer[frskyRxBufferCount++] = data ^ STUFF_MASK;
      }
      dataState = STATE_DATA_IN_FRAME;
      break;

    case STATE_DATA_IDLE:
      if (data == START_STOP) {
        frskyRxBufferCount = 0;
        dataState = STATE_DATA_START;
      }
#if defined(TELEMETREZ)
      if (data == PRIVATE) {
        dataState = STATE_DATA_PRIVATE_LEN;
      }
#endif
      break;

#if defined(TELEMETREZ)
    case STATE_DATA_PRIVATE_LEN:
      dataState = STATE_DATA_PRIVATE_VALUE;
      privateDataLen = data; // Count of bytes to receive
      privateDataPos = 0;
      break;

    case STATE_DATA_PRIVATE_VALUE :
      if (privateDataPos == 0) {
        // Process first private data byte
        // PC6, PC7
        if ((data & 0x3F) == 0) {// Check byte is valid
          DDRC |= 0xC0;          // Set as outputs
          PORTC = ( PORTC & 0x3F ) | ( data & 0xC0 ); // update outputs
        }
      }
#if defined(ROTARY_ENCODER_NAVIGATION)
      if (privateDataPos == 1) {
        TrotCount = data;
      }
      if (privateDataPos == 2) { // rotary encoder switch
        RotEncoder = data;
      }
#endif
      if (++privateDataPos == privateDataLen) {
        dataState = STATE_DATA_IDLE;
      }
      break;
#endif
  } // switch

#if defined(FRSKY_SPORT)
  if (IS_FRSKY_SPORT_PROTOCOL() && frskyRxBufferCount >= FRSKY_SPORT_PACKET_SIZE) {
    processSportPacket(frskyRxBuffer);
    dataState = STATE_DATA_IDLE;
  }
#endif
}

void telemetryWakeup()
{


  if (IS_FRSKY_D_PROTOCOL()) {
    // Attempt to transmit any waiting Fr-Sky alarm set packets every 50ms (subject to packet buffer availability)
    static uint8_t frskyTxDelay = 5;
    if (frskyAlarmsSendState && (--frskyTxDelay == 0)) {
      frskyTxDelay = 5; // 50ms
#if !defined(SIMU)
      frskyDSendNextAlarm();
#endif
    }
  }


#if defined(VARIO)
  if (TELEMETRY_STREAMING() && !IS_FAI_ENABLED()) {
    varioWakeup();
  }
#endif

  #define FRSKY_BAD_ANTENNA() (frskyData.swr.value > 0x33)

}

void telemetryInterrupt10ms()
{
#if   defined(FRSKY_HUB)
  uint16_t voltage = 0; /* unit: 1/10 volts */
  for (uint8_t i=0; i<frskyData.hub.cellsCount; i++)
    voltage += frskyData.hub.cellVolts[i];
  voltage /= (10 / TELEMETRY_CELL_VOLTAGE_MUTLIPLIER);
  frskyData.hub.cellsSum = voltage;
  if (frskyData.hub.cellsSum < frskyData.hub.minCells) {
    frskyData.hub.minCells = frskyData.hub.cellsSum;
  }
#endif

  if (TELEMETRY_STREAMING()) {
    if (!TELEMETRY_OPENXSENSOR()) {
      // power calculation
      uint8_t channel = g_model.frsky.voltsSource;
      if (channel <= FRSKY_VOLTS_SOURCE_A2) {
        voltage = applyChannelRatio(channel, frskyData.analog[channel].value) / 10;
      }

#if defined(FRSKY_HUB)
      else if (channel == FRSKY_VOLTS_SOURCE_FAS) {
        voltage = frskyData.hub.vfas;
      }
#endif

#if defined(FRSKY_HUB)
      uint16_t current = frskyData.hub.current; /* unit: 1/10 amps */
#else
      uint16_t current = 0;
#endif

      channel = g_model.frsky.currentSource - FRSKY_CURRENT_SOURCE_A1;
      if (channel < MAX_FRSKY_A_CHANNELS) {
        current = applyChannelRatio(channel, frskyData.analog[channel].value) / 10;
      }

      frskyData.hub.power = ((current>>1) * (voltage>>1)) / 25;

      frskyData.hub.currentPrescale += current;
      if (frskyData.hub.currentPrescale >= 3600) {
        frskyData.hub.currentConsumption += 1;
        frskyData.hub.currentPrescale -= 3600;
      }
    }

    if (frskyData.hub.power > frskyData.hub.maxPower) {
      frskyData.hub.maxPower = frskyData.hub.power;
    }
  }

#if defined(WS_HOW_HIGH)
  if (frskyUsrStreaming > 0) {
    frskyUsrStreaming--;
  }
#endif

  if (frskyStreaming > 0) {
    frskyStreaming--;
  }
  else {
#if !defined(SIMU)
    frskyData.rssi[0].set(0);
    frskyData.rssi[1].set(0);
#endif
  }
}

void telemetryReset()
{
  memclear(&frskyData, sizeof(frskyData));


  frskyStreaming = 0; // reset counter only if valid frsky packets are being detected
  link_counter = 0;


#if defined(FRSKY_HUB)
  frskyData.hub.gpsLatitude_bp = 2;
  frskyData.hub.gpsLongitude_bp = 2;
  frskyData.hub.gpsFix = -1;
#endif

#if defined(SIMU)

  frskyData.rssi[0].value = 75;
  frskyData.rssi[1].value = 75;
  frskyData.analog[TELEM_ANA_A1].set(120, UNIT_VOLTS);
  frskyData.analog[TELEM_ANA_A2].set(240, UNIT_VOLTS);

  frskyData.hub.fuelLevel = 75;
  frskyData.hub.rpm = 12000;
  frskyData.hub.vfas = 100;
  frskyData.hub.minVfas = 90;

#if defined(GPS)
  frskyData.hub.gpsFix = 1;
  frskyData.hub.gpsLatitude_bp = 4401;
  frskyData.hub.gpsLatitude_ap = 7710;
  frskyData.hub.gpsLongitude_bp = 1006;
  frskyData.hub.gpsLongitude_ap = 8872;
  frskyData.hub.gpsSpeed_bp = 200;  //in knots
  frskyData.hub.gpsSpeed_ap = 0;
  getGpsPilotPosition();

  frskyData.hub.gpsLatitude_bp = 4401;
  frskyData.hub.gpsLatitude_ap = 7455;
  frskyData.hub.gpsLongitude_bp = 1006;
  frskyData.hub.gpsLongitude_ap = 9533;
  getGpsDistance();
#endif

  frskyData.hub.airSpeed = 1000; // 185.1 km/h

  frskyData.hub.cellsCount = 6;
  frskyData.hub.cellVolts[0] = 410/TELEMETRY_CELL_VOLTAGE_MUTLIPLIER;
  frskyData.hub.cellVolts[1] = 420/TELEMETRY_CELL_VOLTAGE_MUTLIPLIER;
  frskyData.hub.cellVolts[2] = 430/TELEMETRY_CELL_VOLTAGE_MUTLIPLIER;
  frskyData.hub.cellVolts[3] = 440/TELEMETRY_CELL_VOLTAGE_MUTLIPLIER;
  frskyData.hub.cellVolts[4] = 450/TELEMETRY_CELL_VOLTAGE_MUTLIPLIER;
  frskyData.hub.cellVolts[5] = 460/TELEMETRY_CELL_VOLTAGE_MUTLIPLIER;
  frskyData.hub.minCellVolts = 250/TELEMETRY_CELL_VOLTAGE_MUTLIPLIER;
  frskyData.hub.minCell = 300;    //unit 10mV
  frskyData.hub.minCells = 220;  //unit 100mV
  //frskyData.hub.cellsSum = 261;    //calculated from cellVolts[]

  frskyData.hub.gpsAltitude_bp = 50;
  frskyData.hub.baroAltitude_bp = 50;
  frskyData.hub.minAltitude = 10;
  frskyData.hub.maxAltitude = 500;

  frskyData.hub.accelY = 100;
  frskyData.hub.temperature1 = -30;
  frskyData.hub.maxTemperature1 = 100;

  frskyData.hub.current = 55;
  frskyData.hub.maxCurrent = 65;
#endif

/*Add some default sensor values to the simulator*/
}

void telemetryInit()
{
  telemetryPortInit();
}

#if   defined(FRSKY_HUB)
void frskyUpdateCells(void)
{
  // Voltage => Cell number + Cell voltage
  uint8_t battnumber = ((frskyData.hub.volts & 0x00F0) >> 4);
  if (battnumber < 12) {
    if (frskyData.hub.cellsCount < battnumber+1) {
      frskyData.hub.cellsCount = battnumber+1;
    }
    uint8_t cellVolts = (uint8_t)(((((frskyData.hub.volts & 0xFF00) >> 8) + ((frskyData.hub.volts & 0x000F) << 8))) / 10);
    frskyData.hub.cellVolts[battnumber] = cellVolts;
    if (!frskyData.hub.minCellVolts || cellVolts<frskyData.hub.minCellVolts || battnumber==frskyData.hub.minCellIdx) {
      frskyData.hub.minCellIdx = battnumber;
      frskyData.hub.minCellVolts = cellVolts;
      if (!frskyData.hub.minCell || frskyData.hub.minCellVolts<frskyData.hub.minCell)
        frskyData.hub.minCell = frskyData.hub.minCellVolts;
    }
  }
}
#endif
