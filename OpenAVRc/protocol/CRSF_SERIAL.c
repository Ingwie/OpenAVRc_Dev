 /*
 **************************************************************************
 *                                                                        *
 *                 ____                ___ _   _____                      *
 *                / __ \___  ___ ___  / _ | | / / _ \____                 *
 *               / /_/ / _ \/ -_) _ \/ __ | |/ / , _/ __/                 *
 *               \____/ .__/\__/_//_/_/ |_|___/_/|_|\__/                  *
 *                   /_/                                                  *
 *                                                                        *
 *              This file is part of the OpenAVRc project.                *
 *                                                                        *
 *                         Based on code(s) named :                       *
 *             OpenTx - https://github.com/opentx/opentx                  *
 *             Deviation - https://www.deviationtx.com/                   *
 *                                                                        *
 *                Only AVR code here for visibility ;-)                   *
 *                                                                        *
 *   OpenAVRc is free software: you can redistribute it and/or modify     *
 *   it under the terms of the GNU General Public License as published by *
 *   the Free Software Foundation, either version 2 of the License, or    *
 *   (at your option) any later version.                                  *
 *                                                                        *
 *   OpenAVRc is distributed in the hope that it will be useful,          *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of       *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        *
 *   GNU General Public License for more details.                         *
 *                                                                        *
 *       License GPLv2: http://www.gnu.org/licenses/gpl-2.0.html          *
 *                                                                        *
 **************************************************************************
*/


#include "../OpenAVRc.h"

#include "crsf.h"
#define XTELEMETRY (g_model.rfOptionBool1)

const pm_char STR_CRSF_SPEEDS[] PROGMEM = "400000""115200";

const static RfOptionSettingsvar_t RfOpt_CRSF_Ser[] PROGMEM = {
/*rfProtoNeed*/BOOL1USED, //can be PROTO_NEED_SPI | BOOL1USED | BOOL2USED | BOOL3USED
/*rfSubTypeMax*/1,//2 seul subtype
/*rfOptionValue1Min*/0,
/*rfOptionValue1Max*/0,
/*rfOptionValue2Min*/0,
/*rfOptionValue2Max*/0,
/*rfOptionValue3Max*/5// RF POWER (10mW (10dBm), 25mW (14dBm), 100mW (20dBm),250mW 500mW (27dBm) 1W (30dBm)*, 2W (33dBm)
};

static void CRSF_Reset()
{
  USART_DISABLE_TX(CRSF_USART);
  USART_DISABLE_RX(CRSF_USART);
}

#include "crsf.h"

#define CHAN_MULTIPLIER 100
#define CHAN_MAX_VALUE (100 * CHAN_MULTIPLIER)


//#if HAS_EXTENDED_TELEMETRY
//#include "telemetry.h"
//#endif


//#define CRSF_DATARATE             400000
#define CRSF_FRAME_PERIOD         4000   // 4ms
#define CRSF_CHANNELS             16
#define CRSF_PACKET_SIZE          26


// crc implementation from CRSF protocol document rev7
static uint8_t crsf_crc8tab[256] = {
    0x00, 0xD5, 0x7F, 0xAA, 0xFE, 0x2B, 0x81, 0x54, 0x29, 0xFC, 0x56, 0x83, 0xD7, 0x02, 0xA8, 0x7D,
    0x52, 0x87, 0x2D, 0xF8, 0xAC, 0x79, 0xD3, 0x06, 0x7B, 0xAE, 0x04, 0xD1, 0x85, 0x50, 0xFA, 0x2F,
    0xA4, 0x71, 0xDB, 0x0E, 0x5A, 0x8F, 0x25, 0xF0, 0x8D, 0x58, 0xF2, 0x27, 0x73, 0xA6, 0x0C, 0xD9,
    0xF6, 0x23, 0x89, 0x5C, 0x08, 0xDD, 0x77, 0xA2, 0xDF, 0x0A, 0xA0, 0x75, 0x21, 0xF4, 0x5E, 0x8B,
    0x9D, 0x48, 0xE2, 0x37, 0x63, 0xB6, 0x1C, 0xC9, 0xB4, 0x61, 0xCB, 0x1E, 0x4A, 0x9F, 0x35, 0xE0,
    0xCF, 0x1A, 0xB0, 0x65, 0x31, 0xE4, 0x4E, 0x9B, 0xE6, 0x33, 0x99, 0x4C, 0x18, 0xCD, 0x67, 0xB2,
    0x39, 0xEC, 0x46, 0x93, 0xC7, 0x12, 0xB8, 0x6D, 0x10, 0xC5, 0x6F, 0xBA, 0xEE, 0x3B, 0x91, 0x44,
    0x6B, 0xBE, 0x14, 0xC1, 0x95, 0x40, 0xEA, 0x3F, 0x42, 0x97, 0x3D, 0xE8, 0xBC, 0x69, 0xC3, 0x16,
    0xEF, 0x3A, 0x90, 0x45, 0x11, 0xC4, 0x6E, 0xBB, 0xC6, 0x13, 0xB9, 0x6C, 0x38, 0xED, 0x47, 0x92,
    0xBD, 0x68, 0xC2, 0x17, 0x43, 0x96, 0x3C, 0xE9, 0x94, 0x41, 0xEB, 0x3E, 0x6A, 0xBF, 0x15, 0xC0,
    0x4B, 0x9E, 0x34, 0xE1, 0xB5, 0x60, 0xCA, 0x1F, 0x62, 0xB7, 0x1D, 0xC8, 0x9C, 0x49, 0xE3, 0x36,
    0x19, 0xCC, 0x66, 0xB3, 0xE7, 0x32, 0x98, 0x4D, 0x30, 0xE5, 0x4F, 0x9A, 0xCE, 0x1B, 0xB1, 0x64,
    0x72, 0xA7, 0x0D, 0xD8, 0x8C, 0x59, 0xF3, 0x26, 0x5B, 0x8E, 0x24, 0xF1, 0xA5, 0x70, 0xDA, 0x0F,
    0x20, 0xF5, 0x5F, 0x8A, 0xDE, 0x0B, 0xA1, 0x74, 0x09, 0xDC, 0x76, 0xA3, 0xF7, 0x22, 0x88, 0x5D,
    0xD6, 0x03, 0xA9, 0x7C, 0x28, 0xFD, 0x57, 0x82, 0xFF, 0x2A, 0x80, 0x55, 0x01, 0xD4, 0x7E, 0xAB,
    0x84, 0x51, 0xFB, 0x2E, 0x7A, 0xAF, 0x05, 0xD0, 0xAD, 0x78, 0xD2, 0x07, 0x53, 0x86, 0x2C, 0xF9};

uint8_t crsf_crc8(const uint8_t *ptr, uint8_t len) {
    uint8_t crc = 0;
    for (uint8_t i=0; i < len; i++) {
        crc = crsf_crc8tab[crc ^ *ptr++];
    }
    return crc;
}

#if defined(FRSKY)//HAS_EXTENDED_TELEMETRY
/*
static uint8_t telemetryRxBuffer[TELEMETRY_RX_PACKET_SIZE];
static uint8_t telemetryRxBufferCount;

static void set_telemetry(crossfire_telem_t offset, int32_t value) {
    Telemetry.value[offset] = value;
    TELEMETRY_SetUpdated(offset);
}

static uint8_t checkCrossfireTelemetryFrameCRC() {
  uint8_t len = telemetryRxBuffer[1];
  uint8_t crc = crsf_crc8(&telemetryRxBuffer[2], len-1);
  return (crc == telemetryRxBuffer[len+1]);
}

static uint8_t getCrossfireTelemetryValue(uint8_t index, int32_t *value, uint8_t len) {
  uint8_t result = 0;
  uint8_t *byte = &telemetryRxBuffer[index];
  *value = (*byte & 0x80) ? -1 : 0;
  for (uint8_t i=0; i < len; i++) {
    *value <<= 8;
    if (*byte != 0xff) result = 1;
    *value += *byte++;
  }
  return result;
}

static void processCrossfireTelemetryFrame()
{

  int32_t value;
  uint8_t i;
  uint8_t id = telemetryRxBuffer[2];

  switch(id) {
    case TYPE_GPS:
      if (getCrossfireTelemetryValue(3, &value, 4)) {
        Telemetry.gps.latitude = value / 10;
        if (value & (1 << 30))
            Telemetry.gps.latitude = -Telemetry.gps.latitude;   // south negative
        TELEMETRY_SetUpdated(TELEM_GPS_LAT);
      }
      if (getCrossfireTelemetryValue(7, &value, 4)) {
        Telemetry.gps.longitude = value / 10;
        if (value & (1 << 30))
            Telemetry.gps.longitude = -Telemetry.gps.longitude;   // west negative
        TELEMETRY_SetUpdated(TELEM_GPS_LONG);
      }
      if (getCrossfireTelemetryValue(11, &value, 2)) {
        Telemetry.gps.velocity = value;
        TELEMETRY_SetUpdated(TELEM_GPS_SPEED);
      }
      if (getCrossfireTelemetryValue(13, &value, 2)) {
        Telemetry.gps.heading = value;
        TELEMETRY_SetUpdated(TELEM_GPS_HEADING);
      }
      if (getCrossfireTelemetryValue(15, &value, 2)) {
        Telemetry.gps.altitude = value - 1000;
        TELEMETRY_SetUpdated(TELEM_GPS_ALT);
      }
      if (getCrossfireTelemetryValue(17, &value, 1)) {
        Telemetry.gps.satcount = value;
        TELEMETRY_SetUpdated(TELEM_GPS_SATCOUNT);
      }
      break;

    case TYPE_LINK:
      for (i=1; i <= TELEM_CRSF_TX_SNR; i++) {
        if (getCrossfireTelemetryValue(2+i, &value, 1)) {   // payload starts at third byte of rx packet
          if (i == TELEM_CRSF_TX_POWER) {
            static const s32 power_values[] = { 0, 10, 25, 100, 500, 1000, 2000, 250 };
            if ((uint8_t)value >= (sizeof power_values / sizeof (s32)))
              continue;
            value = power_values[value];
          }
          set_telemetry(i, value);
        }
      }
      break;

    case TYPE_BATTERY:
      if (getCrossfireTelemetryValue(3, &value, 2))
        set_telemetry(TELEM_CRSF_BATT_VOLTAGE, value);
      if (getCrossfireTelemetryValue(5, &value, 2))
        set_telemetry(TELEM_CRSF_BATT_CURRENT, value);
      if (getCrossfireTelemetryValue(7, &value, 3))
        set_telemetry(TELEM_CRSF_BATT_CAPACITY, value);
      break;

    case TYPE_ATTITUDE:
      if (getCrossfireTelemetryValue(3, &value, 2))
        set_telemetry(TELEM_CRSF_ATTITUDE_PITCH, value/10);
      if (getCrossfireTelemetryValue(5, &value, 2))
        set_telemetry(TELEM_CRSF_ATTITUDE_ROLL, value/10);
      if (getCrossfireTelemetryValue(7, &value, 2))
        set_telemetry(TELEM_CRSF_ATTITUDE_YAW, value/10);
      break;

    case TYPE_FLIGHT_MODE:  // string - save first four bytes for now
      memcpy(&value, &telemetryRxBuffer[3], 4);
      set_telemetry(TELEM_CRSF_FLIGHT_MODE, value);
      break;
  }
}

// serial data receive ISR callback
static void processCrossfireTelemetryData(uint8_t data, uint8_t status) {
  (void)status;

  if (telemetryRxBufferCount == 0 && data != ADDR_RADIO) {
    return;
  }

  if (telemetryRxBufferCount == 1 && (data < 2 || data > TELEMETRY_RX_PACKET_SIZE-2)) {
    telemetryRxBufferCount = 0;
    return;
  }

  if (telemetryRxBufferCount < TELEMETRY_RX_PACKET_SIZE) {
    telemetryRxBuffer[telemetryRxBufferCount++] = data;
  } else {
    telemetryRxBufferCount = 0;
    return;
  }

  if ((telemetryRxBuffer[1] + 2) == telemetryRxBufferCount) {
    if (checkCrossfireTelemetryFrameCRC()) {
      if (telemetryRxBuffer[2] < TYPE_PING_DEVICES) {
        processCrossfireTelemetryFrame();     // Broadcast frame
//#if SUPPORT_CRSF_CONFIG
//      } else {
//        CRSF_serial_rcv(telemetryRxBuffer+2, telemetryRxBuffer[1]-1);  // Extended frame
//#endif
      }
    }
    telemetryRxBufferCount = 0;
  }
}
*/
#endif  //defined(FRSKY)// HAS_EXTENDED_TELEMETRY

static uint8_t packetCRSF[CRSF_PACKET_SIZE];
uint8_t crsfTxBufferCount = 25;

/* from CRSF document
Center (1500us) = 992
TICKS_TO_US(x) ((x - 992) * 5 / 8 + 1500)
US_TO_TICKS(x) ((x - 1500) * 8 / 5 + 992)
*/
//#define STICK_SCALE    869  // full scale at +-125
#define STICK_SCALE    800  // +/-100 gives 2000/1000 us
static uint8_t build_rcdata_pkt()
{
    int i;
	uint16_t channelsCRSF[CRSF_CHANNELS];

    for (i=0; i < CRSF_CHANNELS; i++) {
        if (i < CRSF_CHANNELS)//if (i < Model.num_channels)
            channelsCRSF[i] = (uint16_t)(FULL_CHANNEL_OUTPUTS(i) * STICK_SCALE / CHAN_MAX_VALUE + 992);
        else
            channelsCRSF[i] = 992;  // midpoint
    }

    packetCRSF[0] = ADDR_MODULE;
    packetCRSF[1] = 24;   // length of type + payload + crc
    packetCRSF[2] = TYPE_CHANNELS;

    packetCRSF[3]  = (uint8_t) ((channelsCRSF[0] & 0x07FF));
    packetCRSF[4]  = (uint8_t) ((channelsCRSF[0] & 0x07FF)>>8   | (channelsCRSF[1] & 0x07FF)<<3);
    packetCRSF[5]  = (uint8_t) ((channelsCRSF[1] & 0x07FF)>>5   | (channelsCRSF[2] & 0x07FF)<<6);
    packetCRSF[6]  = (uint8_t) ((channelsCRSF[2] & 0x07FF)>>2);
    packetCRSF[7]  = (uint8_t) ((channelsCRSF[2] & 0x07FF)>>10  | (channelsCRSF[3] & 0x07FF)<<1);
    packetCRSF[8]  = (uint8_t) ((channelsCRSF[3] & 0x07FF)>>7   | (channelsCRSF[4] & 0x07FF)<<4);
    packetCRSF[9]  = (uint8_t) ((channelsCRSF[4] & 0x07FF)>>4   | (channelsCRSF[5] & 0x07FF)<<7);
    packetCRSF[10] = (uint8_t) ((channelsCRSF[5] & 0x07FF)>>1);
    packetCRSF[11] = (uint8_t) ((channelsCRSF[5] & 0x07FF)>>9   | (channelsCRSF[6] & 0x07FF)<<2);
    packetCRSF[12] = (uint8_t) ((channelsCRSF[6] & 0x07FF)>>6   | (channelsCRSF[7] & 0x07FF)<<5);
    packetCRSF[13] = (uint8_t) ((channelsCRSF[7] & 0x07FF)>>3);
    packetCRSF[14] = (uint8_t) ((channelsCRSF[8] & 0x07FF));
    packetCRSF[15] = (uint8_t) ((channelsCRSF[8] & 0x07FF)>>8   | (channelsCRSF[9] & 0x07FF)<<3);
    packetCRSF[16] = (uint8_t) ((channelsCRSF[9] & 0x07FF)>>5   | (channelsCRSF[10] & 0x07FF)<<6);
    packetCRSF[17] = (uint8_t) ((channelsCRSF[10] & 0x07FF)>>2);
    packetCRSF[18] = (uint8_t) ((channelsCRSF[10] & 0x07FF)>>10 | (channelsCRSF[11] & 0x07FF)<<1);
    packetCRSF[19] = (uint8_t) ((channelsCRSF[11] & 0x07FF)>>7  | (channelsCRSF[12] & 0x07FF)<<4);
    packetCRSF[20] = (uint8_t) ((channelsCRSF[12] & 0x07FF)>>4  | (channelsCRSF[13] & 0x07FF)<<7);
    packetCRSF[21] = (uint8_t) ((channelsCRSF[13] & 0x07FF)>>1);
    packetCRSF[22] = (uint8_t) ((channelsCRSF[13] & 0x07FF)>>9  | (channelsCRSF[14] & 0x07FF)<<2);
    packetCRSF[23] = (uint8_t) ((channelsCRSF[14] & 0x07FF)>>6  | (channelsCRSF[15] & 0x07FF)<<5);
    packetCRSF[24] = (uint8_t) ((channelsCRSF[15] & 0x07FF)>>3);

    packetCRSF[25] = crsf_crc8(&packetCRSF[2], CRSF_PACKET_SIZE-3);

    for ( i = 0; i < 25; i++) {
      uint16_t pulse = packetCRSF[i];//limit(0, ((FULL_CHANNEL_OUTPUTS(i)*13)>>5)+512,1023);
      Usart0TxBuffer[--crsfTxBufferCount] = (i<<2) | ((pulse>>8)&0x03); // Encoded channel + upper 2 bits pulse width.
      Usart0TxBuffer[--crsfTxBufferCount] = pulse & 0xff; // Low byte
    }
    Usart0TxBufferCount = 25; // Indicates data to transmit.

    return CRSF_PACKET_SIZE;
}

/*
#ifdef EMULATOR
static const uint8_t rxframes[][64];
#endif //EMULATOR
*/

static enum {
    ST_DATA1,
    ST_DATA2,
} state;

static uint16_t mixer_runtime;
static uint16_t CRSF_SERIAL_cb()
{
    uint8_t length;

    switch (state) {
    case ST_DATA1:
        //CLOCK_RunMixer();    // clears mixer_sync, which is then set when mixer update complete
        state = ST_DATA2;
        return mixer_runtime;

    case ST_DATA2:
//        if (mixer_sync != MIX_DONE && mixer_runtime < 2000) mixer_runtime += 50;
//#if defined(SUPPORT_CRSF_CONFIG)
//        length = CRSF_serial_txd(packetCRSF, sizeof packetCRSF);
//        if (length == 0) {
//            length = build_rcdata_pkt();
//        }
//#else
        length = build_rcdata_pkt();
//#endif
        //UART_Send(packetCRSF, length);
#if !defined(SIMU)
        USART_TRANSMIT_BUFFER(CRSF_USART);
#endif

        state = ST_DATA1;

        return CRSF_FRAME_PERIOD - mixer_runtime;
    }

    return CRSF_FRAME_PERIOD;   // avoid compiler warning
}


/*
static uint16_t CRSF_cb()
{
  // Schedule next Mixer calculations.
  SCHEDULE_MIXER_END_IN_US(22000);

  Usart0TxBufferCount = 0;

  uint8_t dsmTxBufferCount = 14;

  uint8_t dsm_header;

  if(g_model.rfSubType == Sub_LP45)
    dsm_header = 0x00;
  else if(g_model.rfSubType == Sub_DSM2)
    dsm_header = 0x10;
  else dsm_header = 0x10 | DSMX_BIT; // PROTO_DSM2_DSMX

  if(dsmBind)
    dsm_header |= DSM2_SEND_BIND;
  else if(dsmRange)
    dsm_header |= DSM2_SEND_RANGECHECK;

  Usart0TxBuffer[--dsmTxBufferCount] = dsm_header;

  Usart0TxBuffer[--dsmTxBufferCount] = g_model.modelId; // DSM2 Header. Second byte for model match.

#if defined(X_ANY)
  Xany_scheduleTx_AllInstance();
#endif

  for (uint8_t i = 0; i < DSM2_CHANS; i++) {
    uint16_t pulse = limit(0, ((FULL_CHANNEL_OUTPUTS(i)*13)>>5)+512,1023);
    Usart0TxBuffer[--dsmTxBufferCount] = (i<<2) | ((pulse>>8)&0x03); // Encoded channel + upper 2 bits pulse width.
    Usart0TxBuffer[--dsmTxBufferCount] = pulse & 0xff; // Low byte
  }
  Usart0TxBufferCount = 14; // Indicates data to transmit.

#if !defined(SIMU)
  USART_TRANSMIT_BUFFER(DSM_USART);
#endif

  heartbeat |= HEART_TIMER_PULSES;
  CALCULATE_LAT_JIT(); // Calculate latency and jitter.
  return 22000U *2; // 22 mSec Frame.
}
*/

const pm_char STR_400K[] PROGMEM = "400k";
const pm_char STR_115K[] PROGMEM = "115k";
static void CRSF_init()
{
	// 400K 8N1
	//UART_Initialize();
	//UART_SetDataRate(CRSF_DATARATE);
	//UART_SetDuplex(UART_DUPLEX_HALF);
	switch (g_model.rfSubType)
	{
    case 400:USART_SET_BAUD_400K(CRSF_USART);displayPopup(STR_400K);
    case 115:USART_SET_BAUD_115K2(CRSF_USART);displayPopup(STR_115K);
    default:USART_SET_BAUD_400K(CRSF_USART);
	}
	//USART_SET_BAUD_400K(CRSF_USART);
  USART_SET_MODE_8N1(CRSF_USART);
  USART_ENABLE_TX(CRSF_USART);
	Usart0TxBufferCount = 0;

#if defined(FRSKY)//HAS_EXTENDED_TELEMETRY
  if (XTELEMETRY) // telemetry on?
    {
      //UART_StartReceive(processCrossfireTelemetryData);
      USART_ENABLE_RX(CRSF_USART);
      //frskyX_check_telemetry(packet_p2M, len);
    }
#endif
  state = ST_DATA1;
}

static uint16_t CRSF_bind_cb()
{
  SCHEDULE_MIXER_END_IN_US(18000); // Schedule next Mixer calculations.
  CRSF_SERIAL_cb();//build_rcdata_pkt();//CRSF_send_bind_packet();
  heartbeat |= HEART_TIMER_PULSES;
  CALCULATE_LAT_JIT(); // Calculate latency and jitter.
  return 18000U *2;
}

static uint16_t CRSF_cb()
{
  SCHEDULE_MIXER_END_IN_US(12000); // Schedule next Mixer calculations.
  CRSF_SERIAL_cb();//build_rcdata_pkt();//CRSF_send_data_packet();
  heartbeat |= HEART_TIMER_PULSES;
  CALCULATE_LAT_JIT(); // Calculate latency and jitter.
  return 12000U *2;
}


static void CRSF_initialize(uint8_t bind)
{

  CRSF_init();
  if (bind) {
  PROTO_Start_Callback( CRSF_bind_cb);
  } else {
  PROTO_Start_Callback( CRSF_cb);
  }
}

const void *CRSF_Cmds(enum ProtoCmds cmd)
{
  switch(cmd) {
  case PROTOCMD_INIT:
    CRSF_initialize(0);
    return 0;
  case PROTOCMD_RESET:
    PROTO_Stop_Callback();
    CRSF_Reset();
    return 0;
  case PROTOCMD_BIND:
    CRSF_initialize(1);
    return 0;
  case PROTOCMD_GETOPTIONS:
          SetRfOptionSettings(pgm_get_far_address(RfOpt_CRSF_Ser),
                        STR_CRSF_SPEEDS,      //Sub proto
                        STR_DUMMY,      //Option 1 (int)
                        STR_DUMMY,      //Option 2 (int)
                        STR_RFPOWER,      //Option 3 (uint 0 to 31)
                        STR_TELEMETRY,  //OptionBool 1
                        STR_DUMMY,      //OptionBool 2
                        STR_DUMMY       //OptionBool 3
                        );
    return 0;
  default:
    break;
  }
  return 0;
}

