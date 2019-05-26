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

TelemetryData telemetryData;

void telemetryPPMInit()
{
//9600 8N1 - "D" 57600 8N1 -> "S.port"
  if IS_USR_PROTO_SMART_PORT()
    {
      Usart0Set57600BAUDS();
    }
  else
    {
      Usart0Set9600BAUDS();
    }
  Usart0Set8N1();
  Usart0EnableRx(); // enable FrSky-Telemetry reception
}

void telemetryResetValue()
{
  memclear(&telemetryData, sizeof(telemetryData));
  frskyStreaming = 0; // reset counter only if valid frsky packets are being detected
}

NOINLINE void parseTelemFrskyByte(uint8_t data)
{
  static uint8_t dataState = STATE_DATA_IDLE;
  static uint8_t BufferCount;
  static uint8_t Buffer[TELEM_RX_PACKET_SIZE];

  switch (dataState)
    {
    case STATE_DATA_START:
      if (data == START_STOP)
        {
          if (IS_USR_PROTO_SMART_PORT())
            {
              BufferCount = 0;
            }
        }
      else
        {
          if (BufferCount < TELEM_RX_PACKET_SIZE)
            {
              Buffer[BufferCount++] = data;
            }
        }
      dataState = STATE_DATA_IN_FRAME ;
      break;

    case STATE_DATA_IN_FRAME:
      if (data == BYTESTUFF)
        {
          dataState = STATE_DATA_XOR; // XOR next byte
        }
      else if (data == START_STOP)
        {
          if (IS_USR_PROTO_SMART_PORT())
            {
              dataState = STATE_DATA_IN_FRAME ;
              BufferCount = 0;
            }
          else
            {
              // end of frame detected in "D" mode
              //frskyDProcessPacket(Buffer);
              LoadTelemBuffer(Buffer);
              dataState = STATE_DATA_IDLE;
              break;
            }
        }
      else if (BufferCount < TELEM_RX_PACKET_SIZE)
        {
          Buffer[BufferCount++] = data;
        }
      break;

    case STATE_DATA_XOR:
      if (BufferCount < TELEM_RX_PACKET_SIZE)
        {
          Buffer[BufferCount++] = data ^ STUFF_MASK;
        }
      dataState = STATE_DATA_IN_FRAME;
      break;

    case STATE_DATA_IDLE:
      if (data == START_STOP)
        {
          BufferCount = 0;
          dataState = STATE_DATA_START;
        }
      break;

    } // switch

  if (IS_USR_PROTO_SMART_PORT() && BufferCount >= (IS_SPIMODULES_PROTOCOL(g_model.rfProtocol)? TELEM_RX_PACKET_SIZE - 1:TELEM_RX_PACKET_SIZE))
    {
      //processSportPacket(Buffer);
      LoadTelemBuffer(Buffer);
      dataState = STATE_DATA_IDLE;
    }
}

uint8_t checkSportPacket(uint8_t *packet)
{
  uint16_t crc = 0;
  for (uint8_t i=1; i<TELEM_RX_PACKET_SIZE; i++)
    {
      crc += packet[i]; //0-1FF
      crc += crc >> 8; //0-100
      crc &= 0x00ff;
      crc += crc >> 8; //0-0FF
      crc &= 0x00ff;
    }
  return (crc == 0x00ff);
}

void setMinMaxAltitude()
{
  if (telemetryData.value.baroAltitude > telemetryData.value.maxAltitude)
    telemetryData.value.maxAltitude = telemetryData.value.baroAltitude;
  if (telemetryData.value.baroAltitude < telemetryData.value.minAltitude)
    telemetryData.value.minAltitude = telemetryData.value.baroAltitude;
}

void manageBaroAltitude()
{
  // First received barometer altitude => Altitude offset
  if (!telemetryData.value.baroAltitudeOffset)
    telemetryData.value.baroAltitudeOffset = -telemetryData.value.baroAltitude;
  telemetryData.value.baroAltitude += telemetryData.value.baroAltitudeOffset;
  setMinMaxAltitude();
}

#if defined(GPS)
void manageGpsFix()
{
  if (telemetryData.value.gpsLongitudeEW && telemetryData.value.gpsLatitudeNS)
    {
      if (telemetryData.value.gpsGetFirstData)
        {
          telemetryData.value.gpsFix = 1;
          telemetryData.value.gpsGetFirstData = 0;
        }
      else
        telemetryData.value.gpsGetFirstData = 1;
    }
  else
    {
      telemetryData.value.gpsFix = 0;
      telemetryData.value.gpsGetFirstData = 0;
    }
}

void manageGpsAltitude()
{
  IF_GPS_IS_NOT_FIXED manageGpsFix();

  if (!telemetryData.value.gpsAltitudeOffset)
    {
      telemetryData.value.gpsAltitudeOffset = -telemetryData.value.gpsAltitude;
    }

  if (!telemetryData.value.baroAltitudeOffset)
  // Use GPS value for min-max if no baro is detected
    {
      int16_t gpsRelativeAlt = telemetryData.value.gpsAltitude + telemetryData.value.gpsAltitudeOffset;
      telemetryData.value.baroAltitude = gpsRelativeAlt; // Use GPS alt for ALT
      if (gpsRelativeAlt > telemetryData.value.maxAltitude)
        telemetryData.value.maxAltitude = gpsRelativeAlt;
      else if (gpsRelativeAlt < telemetryData.value.minAltitude)
        telemetryData.value.minAltitude = gpsRelativeAlt;
    }

  if (telemetryData.value.gpsFix && !telemetryData.value.pilotLatitude && !telemetryData.value.pilotLongitude)
    {
      // First received GPS position => Pilot GPS position
      getGpsPilotPosition();
    }
  else if (menuHandlers[menuLevel] == menuTelemetryFrsky)
    {
      getGpsDistance();
    }
}
#endif

void checkMaxTemperature1()
{
  if (telemetryData.value.temperature1 > telemetryData.value.maxTemperature1)
    telemetryData.value.maxTemperature1 = telemetryData.value.temperature1;
}

void checkMaxTemperature2()
{
  if (telemetryData.value.temperature2 > telemetryData.value.maxTemperature2)
    telemetryData.value.maxTemperature2 = telemetryData.value.temperature2;
}

void checkMaxRpm()
{
  if (telemetryData.value.rpm > telemetryData.value.maxRpm)
    telemetryData.value.maxRpm = telemetryData.value.rpm;
}

void checkMaxGpsSpeed()
{
  if (telemetryData.value.gpsSpeed_bp > telemetryData.value.maxGpsSpeed)
    telemetryData.value.maxGpsSpeed = telemetryData.value.gpsSpeed_bp;
}

void checkMaxCurrent()
{
  if (telemetryData.value.current > telemetryData.value.maxCurrent)
    telemetryData.value.maxCurrent = telemetryData.value.current;
}

void processSportPacket(uint8_t *sport_packet)
{
  /* uint8_t  dataId = sport_packet[0]; */
  uint8_t  prim   = sport_packet[1];
  uint16_t appId  = *((uint16_t *)(sport_packet+2));

  if (!IS_SPIMODULES_PROTOCOL(g_model.rfProtocol))
    {
      if (!checkSportPacket(sport_packet)) // check only in serial mode, in SPI mode this test is already done
        {
          return;
        }
      frskyStreaming = frskyStreaming ? FRSKY_TIMEOUT10ms : FRSKY_TIMEOUT_FIRST; // Reset counter only in serial mode
      // frskyStreaming gets decremented every 10ms, FRSKY_TIMEOUT_FIRST value is detected to play connection prompt.
    }
#if defined(SIMU) // decrement frskyStreaming in simu for spi protocols
  if (IS_SPIMODULES_PROTOCOL(g_model.rfProtocol))
    frskyStreaming = frskyStreaming ? FRSKY_TIMEOUT10ms : FRSKY_TIMEOUT_FIRST; // Reset counter only in serial mode
#endif

  if (prim != DATA_FRAME)
    return;

  if (appId == RSSI_ID)
    {
      telemetryData.rssi[0].set(SPORT_DATA_U8(sport_packet));
    }
  if (appId == SWR_ID)
    {
      telemetryData.rssi[1].set(SPORT_DATA_U8(sport_packet));
    }
  else if (appId == ADC1_ID || appId == ADC2_ID)
    {
      // A1/A2 of DxR receivers
      telemetryData.analog[appId-ADC1_ID].set(SPORT_DATA_U8(sport_packet),g_model.telemetry.channels[appId-ADC1_ID].type);
#if defined(VARIO)
      uint8_t varioSource = g_model.telemetry.varioSource - VARIO_SOURCE_A1;
      if (varioSource == appId-ADC1_ID)
        {
          telemetryData.value.varioSpeed = applyChannelRatio(varioSource, telemetryData.analog[varioSource].value);
        }
#endif
    }
  else if (appId == BATT_ID)
    {
      telemetryData.analog[0].set(SPORT_DATA_U8(sport_packet),UNIT_VOLTS);
    }
  else if ((appId >> 8) == 0)
    {
      // The old FrSky IDs
      uint16_t value = HUB_DATA_U16(sport_packet);
      processHubPacket((uint8_t)appId, value);
    }
  else if (appId == BETA_BARO_ALT_ID)
    {
      telemetryData.value.baroAltitude = ((SPORT_DATA_S32(sport_packet) >> 8)/10);
      manageBaroAltitude();
    }
  else if (appId == BETA_VARIO_ID)
    {
      int32_t varioSpeed = SPORT_DATA_S32(sport_packet);
      telemetryData.value.varioSpeed = 10 * (varioSpeed >> 8);
    }
  else if IS_IN_RANGE(appId, T1_FIRST_ID, T1_LAST_ID)
    {
      telemetryData.value.temperature1 = SPORT_DATA_S32(sport_packet);
      checkMaxTemperature1();
    }
  else if IS_IN_RANGE(appId, T2_FIRST_ID, T2_LAST_ID)
    {
      telemetryData.value.temperature2 = SPORT_DATA_S32(sport_packet);
      checkMaxTemperature2();
    }
  else if IS_IN_RANGE(appId, RPM_FIRST_ID, RPM_LAST_ID)
    {
      telemetryData.value.rpm = SPORT_DATA_U32(sport_packet) / (g_model.telemetry.blades+2);
      checkMaxRpm();
    }
  else if IS_IN_RANGE(appId, FUEL_FIRST_ID, FUEL_LAST_ID)
    {
      telemetryData.value.fuelLevel = SPORT_DATA_U32(sport_packet);
    }
  else if IS_IN_RANGE(appId, ALT_FIRST_ID, ALT_LAST_ID)
    {
      telemetryData.value.baroAltitude = SPORT_DATA_S32(sport_packet)/100;
      manageBaroAltitude();
    }
  else if IS_IN_RANGE(appId, VARIO_FIRST_ID, VARIO_LAST_ID)
    {
      telemetryData.value.varioSpeed = SPORT_DATA_S32(sport_packet);
    }
  else if IS_IN_RANGE(appId, ACCX_FIRST_ID, ACCX_LAST_ID)
    {
      telemetryData.value.accelX = SPORT_DATA_S32(sport_packet);
    }
  else if IS_IN_RANGE(appId, ACCY_FIRST_ID, ACCY_LAST_ID)
    {
      telemetryData.value.accelY = SPORT_DATA_S32(sport_packet);
    }
  else if IS_IN_RANGE(appId, ACCZ_FIRST_ID, ACCZ_LAST_ID)
    {
      telemetryData.value.accelZ = SPORT_DATA_S32(sport_packet);
    }
  else if IS_IN_RANGE(appId, CURR_FIRST_ID, CURR_LAST_ID)
    {
      telemetryData.value.current = SPORT_DATA_U32(sport_packet);
      checkMaxCurrent();
    }
  else if IS_IN_RANGE(appId, VFAS_FIRST_ID, VFAS_LAST_ID)
    {
      telemetryData.value.vfas = SPORT_DATA_U32(sport_packet)/10;   //TODO: remove /10 and display with PREC2 when using SPORT
    }
#if defined(GPS)
  else if IS_IN_RANGE(appId, GPS_SPEED_FIRST_ID, GPS_SPEED_LAST_ID)
    {
      IF_GPS_IS_FIXED telemetryData.value.gpsSpeed_bp = SPORT_DATA_U32(sport_packet)/100;
      checkMaxGpsSpeed();
    }
  else if IS_IN_RANGE(appId, GPS_TIME_DATE_FIRST_ID, GPS_TIME_DATE_LAST_ID)
    {
      uint32_t gps_time_date = SPORT_DATA_U32(sport_packet);
      if (gps_time_date & 0x000000ff)
        {
          telemetryData.value.year = (uint8_t) (gps_time_date >> 24);
          telemetryData.value.month = (uint8_t) (gps_time_date >> 16);
          telemetryData.value.day = (uint8_t) (gps_time_date >> 8);
        }
      else
        {
          telemetryData.value.hour = (uint8_t) (gps_time_date >> 24);
          telemetryData.value.min = (uint8_t) (gps_time_date >> 16);
          telemetryData.value.sec = (uint8_t) (gps_time_date >> 8);
          telemetryData.value.hour = ((uint8_t) (telemetryData.value.hour + g_eeGeneral.timezone + 24)) % 24;

#if defined(RTCLOCK)
          if (g_eeGeneral.adjustRTC)
            {
              adjustRTChour();
            }
#endif
        }
    }
  else if IS_IN_RANGE(appId, GPS_COURS_FIRST_ID, GPS_COURS_LAST_ID)
    {
      uint32_t course = SPORT_DATA_U32(sport_packet);
      IF_GPS_IS_FIXED {
      telemetryData.value.gpsCourse_bp = course / 100;
      telemetryData.value.gpsCourse_ap = course % 100;
      }
    }
  else if IS_IN_RANGE(appId, GPS_ALT_FIRST_ID, GPS_ALT_LAST_ID)
    {
      IF_GPS_IS_FIXED telemetryData.value.gpsAltitude = SPORT_DATA_S32(sport_packet)/100;
      manageGpsAltitude();
    }
  else if IS_IN_RANGE(appId, GPS_LONG_LATI_FIRST_ID, GPS_LONG_LATI_LAST_ID)
    {
      uint32_t gps_long_lati_data = SPORT_DATA_U32(sport_packet);
      uint32_t gps_long_lati_b1w, gps_long_lati_a1w;
      gps_long_lati_b1w = (gps_long_lati_data & 0x3fffffff) / 10000;
      gps_long_lati_a1w = (gps_long_lati_data & 0x3fffffff) % 10000;
      switch ((gps_long_lati_data & 0xc0000000) >> 30)
        {
        case 0:
          telemetryData.value.gpsLatitude_bp = (gps_long_lati_b1w / 60 * 100) + (gps_long_lati_b1w % 60);
          telemetryData.value.gpsLatitude_ap = gps_long_lati_a1w;
          telemetryData.value.gpsLatitudeNS = 'N';
          break;
        case 1:
          telemetryData.value.gpsLatitude_bp = (gps_long_lati_b1w / 60 * 100) + (gps_long_lati_b1w % 60);
          telemetryData.value.gpsLatitude_ap = gps_long_lati_a1w;
          telemetryData.value.gpsLatitudeNS = 'S';
          break;
        case 2:
          telemetryData.value.gpsLongitude_bp = (gps_long_lati_b1w / 60 * 100) + (gps_long_lati_b1w % 60);
          telemetryData.value.gpsLongitude_ap = gps_long_lati_a1w;
          telemetryData.value.gpsLongitudeEW = 'E';
          break;
        case 3:
          telemetryData.value.gpsLongitude_bp = (gps_long_lati_b1w / 60 * 100) + (gps_long_lati_b1w % 60);
          telemetryData.value.gpsLongitude_ap = gps_long_lati_a1w;
          telemetryData.value.gpsLongitudeEW = 'W';
          break;
        }
    }
#endif
  else if IS_IN_RANGE(appId, CELLS_FIRST_ID, CELLS_LAST_ID)
    {
      uint32_t cells = SPORT_DATA_U32(sport_packet);
      uint8_t battnumber = cells & 0xF;
      uint32_t minCell, minCellNum;

      //TODO: Use reported total voltages (bits 4-7)?
      telemetryData.value.cellVolts[battnumber] = ((cells & 0x000FFF00) >> 8) / 10;
      telemetryData.value.cellVolts[battnumber+1] = ((cells & 0xFFF00000) >> 20) / 10;

      if (telemetryData.value.cellsCount < battnumber+2)
        telemetryData.value.cellsCount = battnumber+2;
      if (telemetryData.value.cellVolts[battnumber+1] == 0)
        --telemetryData.value.cellsCount;

      if ((telemetryData.value.cellVolts[battnumber] < telemetryData.value.cellVolts[battnumber+1]) || (telemetryData.value.cellVolts[battnumber+1] == 0))
        {
          minCell = telemetryData.value.cellVolts[battnumber];
          minCellNum = battnumber;
        }
      else
        {
          minCell = telemetryData.value.cellVolts[battnumber+1];
          minCellNum = battnumber+1;
        }

      if (!telemetryData.value.minCellVolts || minCell < telemetryData.value.minCellVolts || minCellNum==telemetryData.value.minCellIdx)
        {
          telemetryData.value.minCellIdx = minCellNum;
          telemetryData.value.minCellVolts = minCell;
        }
    }
}

void frskyDProcessPacket(uint8_t *d_packet)
{
  // What type of d_packet?
  switch (d_packet[0])
    {
    case LINKPKT:   // A1/A2/RSSI values
    {
      telemetryData.analog[TELEM_ANA_A1].set(d_packet[1], g_model.telemetry.channels[TELEM_ANA_A1].type);
      telemetryData.analog[TELEM_ANA_A2].set(d_packet[2], g_model.telemetry.channels[TELEM_ANA_A2].type);
      telemetryData.rssi[0].set(d_packet[3]);
      telemetryData.rssi[1].set(d_packet[4] / 2);
      frskyStreaming = frskyStreaming ? FRSKY_TIMEOUT10ms : FRSKY_TIMEOUT_FIRST; // Reset counter only in serial mode
      // frskyStreaming gets decremented every 10ms, FRSKY_TIMEOUT_FIRST value is detected to play connection prompt.

#if defined(VARIO)
      uint8_t varioSource = g_model.telemetry.varioSource - VARIO_SOURCE_A1;
      if (varioSource < 2)
        {
          telemetryData.value.varioSpeed = applyChannelRatio(varioSource, telemetryData.analog[varioSource].value);
        }
#endif
      break;
    }

    case USRPKT: // User Data d_packet
      uint8_t numBytes = 3 + (d_packet[1] & 0x07); // sanitize in case of data corruption leading to buffer overflow
      for (uint8_t i=3; i<numBytes; ++i)
        {
          if (IS_USR_PROTO_FRSKY_HUB())
            {
              parseTelemHubByte(d_packet[i]);
            }
#if defined(WS_HOW_HIGH)
          if (IS_USR_PROTO_WS_HOW_HIGH())
            {
              parseTelemWSHowHighByte(d_packet[i]);
            }
#endif
        }
      break;
    }
}

void parseTelemHubByte(uint8_t byte)
{
  static uint8_t structPos;
  static uint8_t lowByte;
  static TS_STATE state = TS_IDLE;

  if (byte == 0x5e)
    {
      state = TS_DATA_ID;
      return;
    }
  if (state == TS_IDLE)
    {
      return;
    }
  if (state & TS_XOR)
    {
      byte = byte ^ 0x60;
      state = (TS_STATE)(state - TS_XOR);
    }
  else if (byte == 0x5d)
    {
      state = (TS_STATE)(state | TS_XOR);
      return;
    }
  if (state == TS_DATA_ID)
    {
      if (byte > 0x3f)
        {
          state = TS_IDLE;
        }
      else
        {
          structPos = byte * 2;
          state = TS_DATA_LOW;
        }
      return;
    }
  if (state == TS_DATA_LOW)
    {
      lowByte = byte;
      state = TS_DATA_HIGH;
      return;
    }

  state = TS_IDLE;

  processHubPacket(structPos,byte<<8 | lowByte);
}

void processHubPacket(uint8_t id, uint16_t value)
{
  switch (id)
    {
#if defined(GPS)
    case GPS_ALT_BP_ID:
      IF_GPS_IS_FIXED telemetryData.value.gpsAltitude = (int16_t)value;
      manageGpsAltitude();
      break;
#endif
    case TEMP1_ID:
      telemetryData.value.temperature1 = (int16_t)value;
      checkMaxTemperature1();
      break;

    case RPM_ID:
      telemetryData.value.rpm = value * (uint8_t)60/(g_model.telemetry.blades+2);
      checkMaxRpm();
      break;

    case FUEL_ID:
      telemetryData.value.fuelLevel = value;
      break;

    case TEMP2_ID:
      telemetryData.value.temperature2 = (int16_t)value;
      checkMaxTemperature2();
      break;

    case VOLTS_ID:
      telemetryData.value.volts = value;
      frskyUpdateCells();
      break;

    case BARO_ALT_BP_ID:
      telemetryData.value.baroAltitude = (int16_t)value;
      manageBaroAltitude();
      break;
#if defined(GPS)
    case GPS_SPEED_BP_ID:
      IF_GPS_IS_FIXED telemetryData.value.gpsSpeed_bp = value*10 + telemetryData.value.gpsSpeed_ap;
      // Speed => Max speed
      checkMaxGpsSpeed();
      break;

    case GPS_LONG_BP_ID:
      telemetryData.value.gpsLongitude_bp = value;
      break;

    case GPS_LAT_BP_ID:
      telemetryData.value.gpsLatitude_bp = value;
      break;

    case GPS_COURS_BP_ID:
      IF_GPS_IS_FIXED telemetryData.value.gpsCourse_bp = value;
      break;

    case GPS_DAY_MONTH_ID:
      telemetryData.value.day = value & 0xFF;
      telemetryData.value.month = value>>8;
      break;

    case GPS_YEAR_ID:
      telemetryData.value.year = value;
      break;

    case GPS_HOUR_MIN_ID:
      telemetryData.value.hour = value & 0xFF;
      telemetryData.value.min = value>>8;
      telemetryData.value.hour = ((uint8_t)(telemetryData.value.hour + g_eeGeneral.timezone + 24)) % 24;
#if defined(RTCLOCK)
      if (g_eeGeneral.adjustRTC)
        {
          adjustRTChour();
        }
#endif
      break;

    case GPS_SEC_ID:
      telemetryData.value.sec = value;
      break;

    case GPS_SPEED_AP_ID:
      IF_GPS_IS_FIXED telemetryData.value.gpsSpeed_ap = value/10;
      break;

    case GPS_LONG_AP_ID:
      telemetryData.value.gpsLongitude_ap = value;
      break;

    case GPS_LAT_AP_ID:
      telemetryData.value.gpsLatitude_ap = value;
      break;

    case GPS_COURS_AP_ID:
      IF_GPS_IS_FIXED telemetryData.value.gpsCourse_ap = value;
      break;
#endif
#if defined(GPS)
    case GPS_LONG_EW_ID:
      telemetryData.value.gpsLongitudeEW = value;
      break;

    case GPS_LAT_NS_ID:
      telemetryData.value.gpsLatitudeNS = value;
      break;
#endif
    case ACCEL_X_ID:
      telemetryData.value.accelX = (int16_t)value/10;
      break;

    case ACCEL_Y_ID:
      telemetryData.value.accelY = (int16_t)value/10;
      break;

    case ACCEL_Z_ID:
      telemetryData.value.accelZ = (int16_t)value/10;
      break;

    case CURRENT_ID:
      telemetryData.value.current = value;
      if ((int16_t)telemetryData.value.current > 0 && ((int16_t)telemetryData.value.current + g_model.telemetry.fasOffset) > 0)
        telemetryData.value.current += g_model.telemetry.fasOffset;
      else
        telemetryData.value.current = 0;
      checkMaxCurrent();
      break;

    case VARIO_ID:
      telemetryData.value.varioSpeed = (int16_t)value;
      break;

    case OXS_MAH_ID:
      telemetryData.value.currentConsumption = value;
      // we receive data from openXsensor. stops the calculation of consumption and power
      telemetryData.value.openXsensor = 1;
      break;

    case OXS_POWER_ID:
      telemetryData.value.power = value;
      break;

    case VFAS_ID:
      telemetryData.value.vfas = value;
      break;

    case VOLTS_BP_ID:
      telemetryData.value.volts_bp = value;
      break;

    case VOLTS_AP_ID:
      telemetryData.value.volts_ap = value;
      telemetryData.value.vfas = ((telemetryData.value.volts_bp * 100 + telemetryData.value.volts_ap * 10) * 21) / 110;
      if (!telemetryData.value.minVfas || telemetryData.value.minVfas > telemetryData.value.vfas)
        telemetryData.value.minVfas = telemetryData.value.vfas;
      break;

    default:
      break;
    }
}

#if defined(WS_HOW_HIGH)
uint8_t frskyUsrStreaming = 0;

void parseTelemWSHowHighByte(uint8_t byte)
{
  if (frskyUsrStreaming < (WSHH_TIMEOUT10ms - 10))
    {
      telemetryData.value.baroAltitude = byte;
      setMinMaxAltitude();
    }
  else
    {
      // At least 100mS passed since last data received
      telemetryData.value.baroAltitude += byte<<8;
    }
  // baroAltitude unit here is feet!
  frskyUsrStreaming = WSHH_TIMEOUT10ms; // reset counter
}
#endif

void telemetryInterrupt10ms()
{

  for (uint8_t i=0; i<NUM_TELEM_RX_BUFFER; ++i)
    {
      if (TelemetryRxBuffer[i][0] || TelemetryRxBuffer[i][1]) // Check if buffer data are present
        {
          if (IS_USR_PROTO_SMART_PORT())
            {
              processSportPacket(TelemetryRxBuffer[i]);
            }
          else if (IS_USR_PROTO_FRSKY_HUB() || IS_USR_PROTO_WS_HOW_HIGH())
            {
              frskyDProcessPacket(TelemetryRxBuffer[i]);
            }
          memclear(TelemetryRxBuffer[i], TELEM_RX_PACKET_SIZE); // Reset buffer
        }
    }

  uint16_t voltage = 0; /* unit: 1/10 volts */
  for (uint8_t i=0; i<telemetryData.value.cellsCount; i++)
    voltage += telemetryData.value.cellVolts[i];
  voltage /= (10 / TELEMETRY_CELL_VOLTAGE_MUTLIPLIER);
  telemetryData.value.cellsSum = voltage;

  if (telemetryData.value.cellsSum < telemetryData.value.minCells)
    {
      telemetryData.value.minCells = telemetryData.value.cellsSum;
    }

  if (TELEMETRY_STREAMING())
    {
      if (!TELEMETRY_OPENXSENSOR())
        {
          // power calculation
          uint8_t channel = g_model.telemetry.voltsSource;
          if (channel <= FRSKY_VOLTS_SOURCE_A2)
            {
              voltage = applyChannelRatio(channel, telemetryData.analog[channel].value) / 10;
            }

          else if (channel == FRSKY_VOLTS_SOURCE_FAS)
            {
              voltage = telemetryData.value.vfas;
            }

          uint16_t current = telemetryData.value.current; /* unit: 1/10 amps */

          channel = g_model.telemetry.currentSource - FRSKY_CURRENT_SOURCE_A1;
          if (channel < MAX_FRSKY_A_CHANNELS)
            {
              current = applyChannelRatio(channel, telemetryData.analog[channel].value) / 10;
            }

          telemetryData.value.power = ((current>>1) * (voltage>>1)) / 25;

          telemetryData.value.currentPrescale += current;
          if (telemetryData.value.currentPrescale >= 3600)
            {
              telemetryData.value.currentConsumption += 1;
              telemetryData.value.currentPrescale -= 3600;
            }
        }

      if (telemetryData.value.power > telemetryData.value.maxPower)
        {
          telemetryData.value.maxPower = telemetryData.value.power;
        }
#if defined(VOICE)
      if (frskyStreaming == FRSKY_TIMEOUT_FIRST)
        {
          PLAY_TELEMETRY_GET();
        }
      else if (frskyStreaming == 1)
        {
          PLAY_TELEMETRY_LOSS();
        }
#endif
    }

#if defined(WS_HOW_HIGH)
  if (frskyUsrStreaming)
    {
      --frskyUsrStreaming;
    }
#endif

  if (frskyStreaming)
    {
      --frskyStreaming;
    }
  else
    {
      telemetryData.rssi[0].set(0);
      telemetryData.rssi[1].set(0);
    }
}

void frskyUpdateCells()
{
  // Voltage => Cell number + Cell voltage
  uint8_t battnumber = ((telemetryData.value.volts & 0x00F0) >> 4);
  if (battnumber < NUMCELLS)
    {
      if (telemetryData.value.cellsCount < battnumber+1)
        {
          telemetryData.value.cellsCount = battnumber+1;
        }
      uint8_t cellVolts = (uint8_t)(((((telemetryData.value.volts & 0xFF00) >> 8) + ((telemetryData.value.volts & 0x000F) << 8))) / 10);
      telemetryData.value.cellVolts[battnumber] = cellVolts;
      if (!telemetryData.value.minCellVolts || cellVolts<telemetryData.value.minCellVolts || battnumber==telemetryData.value.minCellIdx)
        {
          telemetryData.value.minCellIdx = battnumber;
          telemetryData.value.minCellVolts = cellVolts;
          if (!telemetryData.value.minCell || telemetryData.value.minCellVolts<telemetryData.value.minCell)
            telemetryData.value.minCell = telemetryData.value.minCellVolts;
        }
    }
}

void adjustRTChour()
{
  struct tm * t;
  t = localtime(&g_rtcTime);
  if (abs((t->tm_hour-telemetryData.value.hour)*3600 + (t->tm_min-telemetryData.value.min)*60 + (t->tm_sec-telemetryData.value.sec)) > 20)
    {
      // we adjust RTC only if difference is > 20 seconds
      t->tm_hour = telemetryData.value.hour;
      t->tm_min = telemetryData.value.min;
      t->tm_sec = telemetryData.value.sec;
      g_rtcTime = MKTIME(t); // update OAVRC time
      rtcSetTime(t); // update and save local time in RTC chip
    }
}

void TelemetryValueWithMin::set(uint8_t value)
{
  if (!this->value)
    {
      this->value = value;
    }
  else
    {
      this->value = (((this->value<<1) + value)/3);
      if (this->value<value)
        {
          ++this->value;
        }
    }
  if (!min || value < min)
    {
      min = value;
    }
}

void TelemetryValueWithMinMax::set(uint8_t value, uint8_t unit)
{
  TelemetryValueWithMin::set(value);
  if (unit != UNIT_VOLTS)
    {
      this->value = value;
    }
  if (!max || value > max)
    {
      max = value;
    }
}

void LoadTelemBuffer(uint8_t *data)
{
  for (uint8_t i=0; i<NUM_TELEM_RX_BUFFER; ++i)
    {
      if (!(TelemetryRxBuffer[i][0] || TelemetryRxBuffer[i][1])) // Check buffer is free
        {
          memcpy(TelemetryRxBuffer[i], data, TELEM_RX_PACKET_SIZE);
          return;
        }
    }
}

uint16_t getChannelRatio(source_t channel)
{
  return (uint16_t)g_model.telemetry.channels[channel].ratio << g_model.telemetry.channels[channel].multiplier;
}

lcdint_t applyChannelRatio(source_t channel, lcdint_t val)
{
  return ((int32_t)val+g_model.telemetry.channels[channel].offset) * getChannelRatio(channel) * 2 / 51;
}

#if (0)
/*case BFSPPKT:
case RXSPPKT: {
  uint16_t MMSmartPort_id; // = (d_packet[3] << 8) | d_packet[2];
  uint32_t MMSmartPort_data;
  MMSmartPort_id = d_packet[3];
  MMSmartPort_id <<=8;
  MMSmartPort_id |=d_packet[2];
  MMSmartPort_data = d_packet[7];
  MMSmartPort_data <<=8;
  MMSmartPort_data |= d_packet[6];
  MMSmartPort_data <<=8;
  MMSmartPort_data |= d_packet[5];
  MMSmartPort_data <<=8;
  MMSmartPort_data |= d_packet[4];
  parseTelemMMsmartData(MMSmartPort_id, MMSmartPort_data, d_packet[4]);

  frskyStreaming = FRSKY_TIMEOUT10ms; // reset counter only if valid frsky packets are being detected
  link_counter += 256 / FRSKY_D_AVERAGING;

  break;
}*/



void frskyRFProcessPacket(uint8_t *packet)
{
  // was in frskyDProcessPacket()
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

  telemetryData.analog[TELEM_ANA_A1].set(packet[3], g_model.telemetry.channels[TELEM_ANA_A1].type);
  telemetryData.analog[TELEM_ANA_A2].set(packet[4], g_model.telemetry.channels[TELEM_ANA_A2].type);
  telemetryData.rssi[0].set(packet[5]); // RSSI Tx -> Rx.

  telemetryData.rssi[1].set(packet[ packet[0]+1 ]); // RSSI Rx -> Tx.
}


void parseTelemMMsmartData(uint16_t SP_id, uint32_t SP_data, uint8_t SP_data8)
{
  switch (SP_id)
    {
    case CURR_FIRST_ID:
      telemetryData.value.current = SP_data;
      if (telemetryData.value.current > telemetryData.value.maxCurrent)
        telemetryData.value.maxCurrent = telemetryData.value.current;
      break;
    case VFAS_FIRST_ID:
      telemetryData.value.vfas = SP_data / 10;
      if (telemetryData.value.vfas < telemetryData.value.minVfas || telemetryData.value.minVfas == 0)
        telemetryData.value.minVfas = telemetryData.value.vfas;
      break;
    case FUEL_FIRST_ID:
      telemetryData.value.currentConsumption = SP_data;
      telemetryData.value.fuelLevel = SP_data8;
      break;
    case RSSI_ID:
      telemetryData.rssi[0].value = SP_data8;
      if (telemetryData.rssi[0].value < telemetryData.rssi[0].min || telemetryData.rssi[0].min == 0)
        telemetryData.rssi[0].min = telemetryData.rssi[0].value;
      telemetryData.rssi[1].value = SP_data8;
      if (telemetryData.rssi[1].value < telemetryData.rssi[1].min || telemetryData.rssi[1].min == 0)
        telemetryData.rssi[1].min = telemetryData.rssi[1].value;
      break;
    case ADC2_ID:

      break;
    case BATT_ID:

      break;
    case A4_FIRST_ID:
      telemetryData.value.minCellVolts = SP_data / 10;
      break;

    }
}
#endif
