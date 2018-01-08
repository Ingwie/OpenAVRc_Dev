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

uint8_t frskyStreaming = 0;

#if defined(WS_HOW_HIGH)
  uint8_t frskyUsrStreaming = 0;
#endif

uint8_t link_counter = 0;

FrskyData frskyData;

#define IS_FRSKY_D_PROTOCOL()     (0)
#define IS_FRSKY_SPORT_PROTOCOL() (1)

#if (defined(FRSKY_HUB) || defined(WS_HOW_HIGH))
void checkMinMaxAltitude()
{
  if (TELEMETRY_RELATIVE_BARO_ALT_BP > frskyData.hub.maxAltitude)
    frskyData.hub.maxAltitude = TELEMETRY_RELATIVE_BARO_ALT_BP;
  if (TELEMETRY_RELATIVE_BARO_ALT_BP < frskyData.hub.minAltitude)
    frskyData.hub.minAltitude = TELEMETRY_RELATIVE_BARO_ALT_BP;
}
#endif

#if defined(FRSKY_HUB)
void processHubPacket(uint8_t id, uint16_t value)
{
#if defined(GPS)
  if ((uint8_t)id == offsetof(FrskySerialData, gpsLatitude_bp)) {
    if (value)
      frskyData.hub.gpsFix = 1;
    else if (frskyData.hub.gpsFix > 0 && frskyData.hub.gpsLatitude_bp > 1)
      frskyData.hub.gpsFix = 0;
  } else if ((uint8_t)id == offsetof(FrskySerialData, gpsLongitude_bp)) {
    if (value)
      frskyData.hub.gpsFix = 1;
    else if (frskyData.hub.gpsFix > 0 && frskyData.hub.gpsLongitude_bp > 1)
      frskyData.hub.gpsFix = 0;
  }

  if ((uint8_t)id == offsetof(FrskySerialData, gpsAltitude_bp) ||
      ((uint8_t)id >= offsetof(FrskySerialData, gpsAltitude_ap) && (uint8_t)id <= offsetof(FrskySerialData, gpsLatitudeNS) && (uint8_t)id != offsetof(FrskySerialData, baroAltitude_bp) && (uint8_t)id != offsetof(FrskySerialData, baroAltitude_ap))) {
    // If we don't have a fix, we may discard the value
    if (frskyData.hub.gpsFix <= 0)
      return;
  }
#endif  // #if defined(GPS)

  ((uint16_t*)&frskyData.hub)[id] = value;

  switch ((uint8_t)id) {

  case offsetof(FrskySerialData, rpm):
    frskyData.hub.rpm *= (uint8_t)60/(g_model.frsky.blades+2);
    if (frskyData.hub.rpm > frskyData.hub.maxRpm)
      frskyData.hub.maxRpm = frskyData.hub.rpm;
    break;

  case offsetof(FrskySerialData, temperature1):
    if (frskyData.hub.temperature1 > frskyData.hub.maxTemperature1)
      frskyData.hub.maxTemperature1 = frskyData.hub.temperature1;
    break;

  case offsetof(FrskySerialData, temperature2):
    if (frskyData.hub.temperature2 > frskyData.hub.maxTemperature2)
      frskyData.hub.maxTemperature2 = frskyData.hub.temperature2;
    break;

  case offsetof(FrskySerialData, current):
    if ((int16_t)frskyData.hub.current > 0 && ((int16_t)frskyData.hub.current + g_model.frsky.fasOffset) > 0)
      frskyData.hub.current += g_model.frsky.fasOffset;
    else
      frskyData.hub.current = 0;
    if (frskyData.hub.current > frskyData.hub.maxCurrent)
      frskyData.hub.maxCurrent = frskyData.hub.current;
    break;

  case offsetof(FrskySerialData, currentConsumption):
    // we receive data from openXsensor. stops the calculation of consumption and power
    frskyData.hub.openXsensor = 1;
    break;

  case offsetof(FrskySerialData, volts_ap):
#if defined(FAS_BSS)
    frskyData.hub.vfas = (frskyData.hub.volts_bp * 10 + frskyData.hub.volts_ap);
#else
    frskyData.hub.vfas = ((frskyData.hub.volts_bp * 100 + frskyData.hub.volts_ap * 10) * 21) / 110;
#endif
    /* TODO later if (!frskyData.hub.minVfas || frskyData.hub.minVfas > frskyData.hub.vfas)
      frskyData.hub.minVfas = frskyData.hub.vfas; */
    break;

  case offsetof(FrskySerialData, baroAltitude_bp):
    // First received barometer altitude => Altitude offset
    if (!frskyData.hub.baroAltitudeOffset)
      frskyData.hub.baroAltitudeOffset = -frskyData.hub.baroAltitude_bp;
    frskyData.hub.baroAltitude_bp += frskyData.hub.baroAltitudeOffset;
    checkMinMaxAltitude();
    break;

#if defined(GPS)
  case offsetof(FrskySerialData, gpsAltitude_ap):
    if (!frskyData.hub.gpsAltitudeOffset)
      frskyData.hub.gpsAltitudeOffset = -frskyData.hub.gpsAltitude_bp;
    frskyData.hub.gpsAltitude_bp += frskyData.hub.gpsAltitudeOffset;
    if (!frskyData.hub.baroAltitudeOffset) {
      if (frskyData.hub.gpsAltitude_bp > frskyData.hub.maxAltitude)
        frskyData.hub.maxAltitude = frskyData.hub.gpsAltitude_bp;
      if (frskyData.hub.gpsAltitude_bp < frskyData.hub.minAltitude)
        frskyData.hub.minAltitude = frskyData.hub.gpsAltitude_bp;
    }
    if (!frskyData.hub.pilotLatitude && !frskyData.hub.pilotLongitude) {
      // First received GPS position => Pilot GPS position
      getGpsPilotPosition();
    } else if (frskyData.hub.gpsDistNeeded || menuHandlers[menuLevel] == menuTelemetryFrsky) {
      getGpsDistance();
    }
    break;

  case offsetof(FrskySerialData, gpsSpeed_bp):
    // Speed => Max speed
    if (frskyData.hub.gpsSpeed_bp > frskyData.hub.maxGpsSpeed)
      frskyData.hub.maxGpsSpeed = frskyData.hub.gpsSpeed_bp;
    break;
#endif

  case offsetof(FrskySerialData, volts):
    frskyUpdateCells();
    break;

#if defined(GPS)
  case offsetof(FrskySerialData, hour):
    frskyData.hub.hour = ((uint8_t)(frskyData.hub.hour + g_eeGeneral.timezone + 24)) % 24;
    break;
#endif

  case offsetof(FrskySerialData, accelX):
  case offsetof(FrskySerialData, accelY):
  case offsetof(FrskySerialData, accelZ):
    *(int16_t*)(&((uint8_t*)&frskyData.hub)[id]) /= 10;
    break;
  }
}
#endif
#if defined(WS_HOW_HIGH)
void parseTelemWSHowHighByte(uint8_t byte)
{
  if (frskyUsrStreaming < (WSHH_TIMEOUT10ms - 10)) {
    ((uint8_t*)&frskyData.hub)[offsetof(FrskySerialData, baroAltitude_bp)] = byte;
    checkMinMaxAltitude();
  } else {
    // At least 100mS passed since last data received
    ((uint8_t*)&frskyData.hub)[offsetof(FrskySerialData, baroAltitude_bp)+1] = byte;
  }
  // baroAltitude_bp unit here is feet!
  frskyUsrStreaming = WSHH_TIMEOUT10ms; // reset counter
}
#endif

#if defined(FRSKY_HUB)
void parseTelemHubByte(uint8_t byte)
{
  static uint8_t structPos;
  static uint8_t lowByte;
  static TS_STATE state = TS_IDLE;

  if (byte == 0x5e) {
    state = TS_DATA_ID;
    return;
  }
  if (state == TS_IDLE) {
    return;
  }
  if (state & TS_XOR) {
    byte = byte ^ 0x60;
    state = (TS_STATE)(state - TS_XOR);
  } else if (byte == 0x5d) {
    state = (TS_STATE)(state | TS_XOR);
    return;
  }
  if (state == TS_DATA_ID) {
    if (byte > 0x3f) {
      state = TS_IDLE;
    } else {
      structPos = byte * 2;
      state = TS_DATA_LOW;
    }
    return;
  }
  if (state == TS_DATA_LOW) {
    lowByte = byte;
    state = TS_DATA_HIGH;
    return;
  }

  state = TS_IDLE;

  processHubPacket(structPos,byte<<8 | lowByte);

}
#endif  // #if defined(FRSKY_HUB)

void setBaroAltitude(int32_t baroAltitude) //S.port function
{
  // First received barometer altitude => Altitude offset
  if (!frskyData.hub.baroAltitudeOffset)
    frskyData.hub.baroAltitudeOffset = -baroAltitude;

  baroAltitude += frskyData.hub.baroAltitudeOffset;
  frskyData.hub.baroAltitude_bp = baroAltitude;

  baroAltitude /= 100;
  if (baroAltitude > frskyData.hub.maxAltitude)
    frskyData.hub.maxAltitude = baroAltitude;
  if (baroAltitude < frskyData.hub.minAltitude)
    frskyData.hub.minAltitude = baroAltitude;
}

void FrskyValueWithMin::set(uint8_t value)
{
  if (this->value == 0) {
    this->value = value;
  } else {
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

#define SPORT_DATA_U8(packet)   (packet[4])
#define SPORT_DATA_S32(packet)  (*((int32_t *)(packet+4)))
#define SPORT_DATA_U32(packet)  (*((uint32_t *)(packet+4)))
#define HUB_DATA_U16(packet)    (*((uint16_t *)(packet+4)))
// FrSky wrong IDs ?
#define BETA_VARIO_ID      0x8030
#define BETA_BARO_ALT_ID   0x8010

bool checkSportPacket(uint8_t *packet)
{
  uint16_t crc = 0;
  for (int i=1; i<USART0_RX_PACKET_SIZE; i++) {
    crc += packet[i]; //0-1FF
    crc += crc >> 8; //0-100
    crc &= 0x00ff;
    crc += crc >> 8; //0-0FF
    crc &= 0x00ff;
  }
  return (crc == 0x00ff);
}

void processSportPacket(uint8_t *packet)
{
  /* uint8_t  dataId = packet[0]; */
  uint8_t  prim   = packet[1];
  uint16_t appId  = *((uint16_t *)(packet+2));

  if (!checkSportPacket(packet))
    return;

  frskyStreaming = FRSKY_TIMEOUT10ms; // reset counter only if valid frsky packets are being detected
  link_counter += 256 / FRSKY_SPORT_AVERAGING;

  switch (prim) {
  case DATA_FRAME:

    if (appId == RSSI_ID) {
      frskyData.rssi[0].set(SPORT_DATA_U8(packet));
    }
    if (appId == SWR_ID) {
      frskyData.rssi[1].set(SPORT_DATA_U8(packet));
    } else if (appId == ADC1_ID || appId == ADC2_ID) {
      // A1/A2 of DxR receivers
      frskyData.analog[appId-ADC1_ID].set(SPORT_DATA_U8(packet),g_model.frsky.channels[appId-ADC1_ID].type);
#if defined(VARIO)
      uint8_t varioSource = g_model.frsky.varioSource - VARIO_SOURCE_A1;
      if (varioSource == appId-ADC1_ID) {
        frskyData.hub.varioSpeed = applyChannelRatio(varioSource, frskyData.analog[varioSource].value);
      }
#endif
    } else if (appId == BATT_ID) {
      frskyData.analog[0].set(SPORT_DATA_U8(packet),UNIT_VOLTS);
    } else if ((appId >> 8) == 0) {
      // The old FrSky IDs
      uint8_t  id = (uint8_t)appId;
      uint16_t value = HUB_DATA_U16(packet);
      processHubPacket(id, value);
    } else if (appId == BETA_BARO_ALT_ID) {
      int32_t baroAltitude = SPORT_DATA_S32(packet);
      setBaroAltitude(10 * (baroAltitude >> 8));
    } else if (appId == BETA_VARIO_ID) {
      int32_t varioSpeed = SPORT_DATA_S32(packet);
      frskyData.hub.varioSpeed = 10 * (varioSpeed >> 8);
    } else if (appId >= T1_FIRST_ID && appId <= T1_LAST_ID) {
      frskyData.hub.temperature1 = SPORT_DATA_S32(packet);
      if (frskyData.hub.temperature1 > frskyData.hub.maxTemperature1)
        frskyData.hub.maxTemperature1 = frskyData.hub.temperature1;
    } else if (appId >= T2_FIRST_ID && appId <= T2_LAST_ID) {
      frskyData.hub.temperature2 = SPORT_DATA_S32(packet);
      if (frskyData.hub.temperature2 > frskyData.hub.maxTemperature2)
        frskyData.hub.maxTemperature2 = frskyData.hub.temperature2;
    } else if (appId >= RPM_FIRST_ID && appId <= RPM_LAST_ID) {
      frskyData.hub.rpm = SPORT_DATA_U32(packet) / (g_model.frsky.blades+2);
      if (frskyData.hub.rpm > frskyData.hub.maxRpm)
        frskyData.hub.maxRpm = frskyData.hub.rpm;
    } else if (appId >= FUEL_FIRST_ID && appId <= FUEL_LAST_ID) {
      frskyData.hub.fuelLevel = SPORT_DATA_U32(packet);
    } else if (appId >= ALT_FIRST_ID && appId <= ALT_LAST_ID) {
      setBaroAltitude(SPORT_DATA_S32(packet));
    } else if (appId >= VARIO_FIRST_ID && appId <= VARIO_LAST_ID) {
      frskyData.hub.varioSpeed = SPORT_DATA_S32(packet);
    } else if (appId >= ACCX_FIRST_ID && appId <= ACCX_LAST_ID) {
      frskyData.hub.accelX = SPORT_DATA_S32(packet);
    } else if (appId >= ACCY_FIRST_ID && appId <= ACCY_LAST_ID) {
      frskyData.hub.accelY = SPORT_DATA_S32(packet);
    } else if (appId >= ACCZ_FIRST_ID && appId <= ACCZ_LAST_ID) {
      frskyData.hub.accelZ = SPORT_DATA_S32(packet);
    } else if (appId >= CURR_FIRST_ID && appId <= CURR_LAST_ID) {
      frskyData.hub.current = SPORT_DATA_U32(packet);
      if (frskyData.hub.current > frskyData.hub.maxCurrent)
        frskyData.hub.maxCurrent = frskyData.hub.current;
    } else if (appId >= VFAS_FIRST_ID && appId <= VFAS_LAST_ID) {
      frskyData.hub.vfas = SPORT_DATA_U32(packet)/10;   //TODO: remove /10 and display with PREC2 when using SPORT
    } else if (appId >= GPS_SPEED_FIRST_ID && appId <= GPS_SPEED_LAST_ID) {
      frskyData.hub.gpsSpeed_bp = SPORT_DATA_U32(packet);
      frskyData.hub.gpsSpeed_bp = (frskyData.hub.gpsSpeed_bp * 46) / 25 / 1000;
      if (frskyData.hub.gpsSpeed_bp > frskyData.hub.maxGpsSpeed)
        frskyData.hub.maxGpsSpeed = frskyData.hub.gpsSpeed_bp;
    } else if (appId >= GPS_TIME_DATE_FIRST_ID && appId <= GPS_TIME_DATE_LAST_ID) {
      uint32_t gps_time_date = SPORT_DATA_U32(packet);
      if (gps_time_date & 0x000000ff) {
        frskyData.hub.year = (uint16_t) ((gps_time_date & 0xff000000) >> 24);
        frskyData.hub.month = (uint8_t) ((gps_time_date & 0x00ff0000) >> 16);
        frskyData.hub.day = (uint8_t) ((gps_time_date & 0x0000ff00) >> 8);
      } else {
        frskyData.hub.hour = (uint8_t) ((gps_time_date & 0xff000000) >> 24);
        frskyData.hub.min = (uint8_t) ((gps_time_date & 0x00ff0000) >> 16);
        frskyData.hub.sec = (uint16_t) ((gps_time_date & 0x0000ff00) >> 8);
        frskyData.hub.hour = ((uint8_t) (frskyData.hub.hour + g_eeGeneral.timezone + 24)) % 24;
      }
    } else if (appId >= GPS_COURS_FIRST_ID && appId <= GPS_COURS_LAST_ID) {
      uint32_t course = SPORT_DATA_U32(packet);
      frskyData.hub.gpsCourse_bp = course / 100;
      frskyData.hub.gpsCourse_ap = course % 100;
    } else if (appId >= GPS_ALT_FIRST_ID && appId <= GPS_ALT_LAST_ID) {
//        frskyData.hub.gpsAltitude = SPORT_DATA_S32(packet);

      if (!frskyData.hub.gpsAltitudeOffset)
//          frskyData.hub.gpsAltitudeOffset = -frskyData.hub.gpsAltitude;

        if (!frskyData.hub.baroAltitudeOffset) {
          int altitude = TELEMETRY_RELATIVE_GPS_ALT_BP;
          if (altitude > frskyData.hub.maxAltitude)
            frskyData.hub.maxAltitude = altitude;
          if (altitude < frskyData.hub.minAltitude)
            frskyData.hub.minAltitude = altitude;
        }

      if (frskyData.hub.gpsFix > 0) {
        if (!frskyData.hub.pilotLatitude && !frskyData.hub.pilotLongitude) {
          // First received GPS position => Pilot GPS position
          getGpsPilotPosition();
        }
//          else if (frskyData.hub.gpsDistNeeded || g_menuStack[g_menuStackPtr] == menuTelemetryFrsky) {
//            getGpsDistance();
//          }
      }
    } else if (appId >= GPS_LONG_LATI_FIRST_ID && appId <= GPS_LONG_LATI_LAST_ID) {
      uint32_t gps_long_lati_data = SPORT_DATA_U32(packet);
      uint32_t gps_long_lati_b1w, gps_long_lati_a1w;
      gps_long_lati_b1w = (gps_long_lati_data & 0x3fffffff) / 10000;
      gps_long_lati_a1w = (gps_long_lati_data & 0x3fffffff) % 10000;
      switch ((gps_long_lati_data & 0xc0000000) >> 30) {
      case 0:
        frskyData.hub.gpsLatitude_bp = (gps_long_lati_b1w / 60 * 100) + (gps_long_lati_b1w % 60);
        frskyData.hub.gpsLatitude_ap = gps_long_lati_a1w;
        frskyData.hub.gpsLatitudeNS = 'N';
        break;
      case 1:
        frskyData.hub.gpsLatitude_bp = (gps_long_lati_b1w / 60 * 100) + (gps_long_lati_b1w % 60);
        frskyData.hub.gpsLatitude_ap = gps_long_lati_a1w;
        frskyData.hub.gpsLatitudeNS = 'S';
        break;
      case 2:
        frskyData.hub.gpsLongitude_bp = (gps_long_lati_b1w / 60 * 100) + (gps_long_lati_b1w % 60);
        frskyData.hub.gpsLongitude_ap = gps_long_lati_a1w;
        frskyData.hub.gpsLongitudeEW = 'E';
        break;
      case 3:
        frskyData.hub.gpsLongitude_bp = (gps_long_lati_b1w / 60 * 100) + (gps_long_lati_b1w % 60);
        frskyData.hub.gpsLongitude_ap = gps_long_lati_a1w;
        frskyData.hub.gpsLongitudeEW = 'W';
        break;
      }
      if (frskyData.hub.gpsLongitudeEW && frskyData.hub.gpsLatitudeNS) {
        frskyData.hub.gpsFix = 1;
      } else if (frskyData.hub.gpsFix > 0) {
        frskyData.hub.gpsFix = 0;
      }
    }

    else if (appId >= CELLS_FIRST_ID && appId <= CELLS_LAST_ID) {
      uint32_t cells = SPORT_DATA_U32(packet);
      uint8_t battnumber = cells & 0xF;
      uint32_t minCell, minCellNum;

      //TODO: Use reported total voltages (bits 4-7)?
      frskyData.hub.cellVolts[battnumber] = ((cells & 0x000FFF00) >> 8) / 10;
      frskyData.hub.cellVolts[battnumber+1] = ((cells & 0xFFF00000) >> 20) / 10;

      if (frskyData.hub.cellsCount < battnumber+2)
        frskyData.hub.cellsCount = battnumber+2;
      if (frskyData.hub.cellVolts[battnumber+1] == 0)
        --frskyData.hub.cellsCount;

      if ((frskyData.hub.cellVolts[battnumber] < frskyData.hub.cellVolts[battnumber+1]) || (frskyData.hub.cellVolts[battnumber+1] == 0)) {
        minCell = frskyData.hub.cellVolts[battnumber];
        minCellNum = battnumber;
      } else {
        minCell = frskyData.hub.cellVolts[battnumber+1];
        minCellNum = battnumber+1;
      }

      if (!frskyData.hub.minCellVolts || minCell < frskyData.hub.minCellVolts || minCellNum==frskyData.hub.minCellIdx) {
        frskyData.hub.minCellIdx = minCellNum;
        frskyData.hub.minCellVolts = minCell;
      }
    }
    break;

  }
}

NOINLINE void processSerialData(uint8_t data)
{
  static uint8_t dataState = STATE_DATA_IDLE;

  switch (dataState) {
  case STATE_DATA_START:
    if (data == START_STOP) {
      if (IS_FRSKY_SPORT_PROTOCOL()) {
        dataState = STATE_DATA_IN_FRAME ;
        Usart0RxBufferCount = 0;
      }
    } else {
      if (Usart0RxBufferCount < USART0_RX_PACKET_SIZE) {
        Usart0RxBuffer[Usart0RxBufferCount++] = data;
      }
      dataState = STATE_DATA_IN_FRAME;
    }
    break;

  case STATE_DATA_IN_FRAME:
    if (data == BYTESTUFF) {
      dataState = STATE_DATA_XOR; // XOR next byte
    } else if (data == START_STOP) {
      if (IS_FRSKY_SPORT_PROTOCOL()) {
        dataState = STATE_DATA_IN_FRAME ;
        Usart0RxBufferCount = 0;
      } else {
        // end of frame detected
        frskyDProcessPacket(Usart0RxBuffer);
        dataState = STATE_DATA_IDLE;
      }
      break;
    } else if (Usart0RxBufferCount < USART0_RX_PACKET_SIZE) {
      Usart0RxBuffer[Usart0RxBufferCount++] = data;
    }
    break;

  case STATE_DATA_XOR:
    if (Usart0RxBufferCount < USART0_RX_PACKET_SIZE) {
      Usart0RxBuffer[Usart0RxBufferCount++] = data ^ STUFF_MASK;
    }
    dataState = STATE_DATA_IN_FRAME;
    break;

  case STATE_DATA_IDLE:
    if (data == START_STOP) {
      Usart0RxBufferCount = 0;
      dataState = STATE_DATA_START;
    }
    break;

  } // switch

  if (IS_FRSKY_SPORT_PROTOCOL() && Usart0RxBufferCount >= FRSKY_SPORT_PACKET_SIZE) {
    processSportPacket(Usart0RxBuffer);
    dataState = STATE_DATA_IDLE;
  }
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

void frskyRFProcessPacket(uint8_t *packet)
{
  // 20 bytes
  /*
  *  pkt 0 = length not counting appended status bytes
  *  pkt 1,2 = fixed_id
  *  pkt 3 = A1 : 52mV per count; 4.5V = 0x56
  *  pkt 4 = A2 : 13.4mV per count; 3.0V = 0xE3 on D6FR
  *  pkt 5 = RSSI
  *  pkt 6 = number of stream bytes
  *  pkt 7 = sequence number increments mod 32 when packet containing stream data acknowledged
  *  pkt 8-(8+(pkt[6]-1)) = stream data
  *  pkt len-2 = Downlink RSSI
  *  pkt len-1 = crc status (bit7 set indicates good), link quality indicator (bits6-0)
  */

  // A1, A2, RSSI values.

  frskyData.analog[TELEM_ANA_A1].set(packet[3], g_model.frsky.channels[TELEM_ANA_A1].type);
  frskyData.analog[TELEM_ANA_A2].set(packet[4], g_model.frsky.channels[TELEM_ANA_A2].type);
  frskyData.rssi[0].set(packet[5]); // RSSI Tx -> Rx.

  frskyData.rssi[1].set(packet[ packet[0]+1 ]); // RSSI Rx -> Tx.

  link_counter += 256 / FRSKY_D_AVERAGING;
}

void telemetryInterrupt10ms()
{
#if defined(SPIMODULES)
  frskyRFProcessPacket(Usart0RxBuffer);
#endif

#if defined(FRSKY_HUB)
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
  if (frskyUsrStreaming) {
    --frskyUsrStreaming;
  }
#endif

  if (frskyStreaming) {
    --frskyStreaming;
  } else {
#if !defined(SIMUaa)
    frskyData.rssi[0].set(0);
    frskyData.rssi[1].set(0);
#endif
  }
}

void telemetryReset()
{
  Usart0DisableTx();
  Usart0DisableRx();
  telemetryResetValue();
}
void telemetryResetValue()
{
  memclear(&frskyData, sizeof(frskyData));


  frskyStreaming = 0; // reset counter only if valid frsky packets are being detected
  link_counter = 0;


#if defined(FRSKY_HUB)
  frskyData.hub.gpsLatitude_bp = 2;
  frskyData.hub.gpsLongitude_bp = 2;
  frskyData.hub.gpsFix = -1;
#endif

  /*#if defined(SIMUaa)

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

    frskyData.hub.currentConsumption = 100;
    frskyData.hub.current = 55;
    frskyData.hub.maxCurrent = 65;
  #endif

   Add some default sensor values to the simulator*/
}

void telemetryInit()
{
//9600 8N1 - "D" 57600 8N1 -> "S.port"
  Usart0Set9600BAUDS();
  Usart0Set8N1();
  Usart0EnableTx(); // enable FrSky-Telemetry emission
  Usart0EnableRx(); // enable FrSky-Telemetry reception}

  Usart0TxBufferCount = 0; // TODO not driver code
}

void parseTelemMMsmartData(uint16_t SP_id, uint32_t SP_data, uint8_t SP_data8)
{
  switch (SP_id) {
  case CURR_FIRST_ID:
    frskyData.hub.current = SP_data;
    if (frskyData.hub.current > frskyData.hub.maxCurrent)
      frskyData.hub.maxCurrent = frskyData.hub.current;
    break;
  case VFAS_FIRST_ID:
    frskyData.hub.vfas = SP_data / 10;
    if (frskyData.hub.vfas < frskyData.hub.minVfas || frskyData.hub.minVfas == 0)
      frskyData.hub.minVfas = frskyData.hub.vfas;
    break;
  case FUEL_FIRST_ID:
    frskyData.hub.currentConsumption = SP_data;
    frskyData.hub.fuelLevel = SP_data8;
    break;
  case RSSI_ID:
    frskyData.rssi[0].value = SP_data8;
    if (frskyData.rssi[0].value < frskyData.rssi[0].min || frskyData.rssi[0].min == 0)
      frskyData.rssi[0].min = frskyData.rssi[0].value;
    frskyData.rssi[1].value = SP_data8;
    if (frskyData.rssi[1].value < frskyData.rssi[1].min || frskyData.rssi[1].min == 0)
      frskyData.rssi[1].min = frskyData.rssi[1].value;
    break;
  case ADC2_ID:

    break;
  case BATT_ID:

    break;
  case A4_FIRST_ID:
    frskyData.hub.minCellVolts = SP_data / 10;
    break;

  }
}

void frskyDProcessPacket(uint8_t *packet)
{
  // What type of packet?
  switch (packet[0]) {
  case LINKPKT: { // A1/A2/RSSI values
    frskyData.analog[TELEM_ANA_A1].set(packet[1], g_model.frsky.channels[TELEM_ANA_A1].type);
    frskyData.analog[TELEM_ANA_A2].set(packet[2], g_model.frsky.channels[TELEM_ANA_A2].type);
    frskyData.rssi[0].set(packet[3]);
    frskyData.rssi[1].set(packet[4] / 2);
    frskyStreaming = FRSKY_TIMEOUT10ms; // reset counter only if valid frsky packets are being detected
    link_counter += 256 / FRSKY_D_AVERAGING;
#if defined(VARIO)
    uint8_t varioSource = g_model.frsky.varioSource - VARIO_SOURCE_A1;
    if (varioSource < 2) {
      frskyData.hub.varioSpeed = applyChannelRatio(varioSource, frskyData.analog[varioSource].value);
    }
#endif
    break;
  }
  case BFSPPKT:
  case RXSPPKT: {
    uint16_t MMSmartPort_id; // = (packet[3] << 8) | packet[2];
    uint32_t MMSmartPort_data;
    MMSmartPort_id = packet[3];
    MMSmartPort_id <<=8;
    MMSmartPort_id |=packet[2];
    MMSmartPort_data = packet[7];
    MMSmartPort_data <<=8;
    MMSmartPort_data |= packet[6];
    MMSmartPort_data <<=8;
    MMSmartPort_data |= packet[5];
    MMSmartPort_data <<=8;
    MMSmartPort_data |= packet[4];
    parseTelemMMsmartData(MMSmartPort_id, MMSmartPort_data, packet[4]);

    frskyStreaming = FRSKY_TIMEOUT10ms; // reset counter only if valid frsky packets are being detected
    link_counter += 256 / FRSKY_D_AVERAGING;

    break;
  }
#if defined(FRSKY_HUB) || defined (WS_HOW_HIGH)
  case USRPKT: // User Data packet
    uint8_t numBytes = 3 + (packet[1] & 0x07); // sanitize in case of data corruption leading to buffer overflow
    for (uint8_t i=3; i<numBytes; i++) {
#if defined(FRSKY_HUB)
      if (IS_USR_PROTO_FRSKY_HUB()) {
        parseTelemHubByte(packet[i]);
      }
#endif
#if defined(WS_HOW_HIGH)
      if (IS_USR_PROTO_WS_HOW_HIGH()) {
        parseTelemWSHowHighByte(packet[i]);
      }
#endif
    }
    break;
#endif  // #if defined(FRSKY_HUB) || defined (WS_HOW_HIGH)
  }
}

#if   defined(FRSKY_HUB)
void frskyUpdateCells()
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
