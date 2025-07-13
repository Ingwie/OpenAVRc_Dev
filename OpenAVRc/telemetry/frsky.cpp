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
      USART_SET_BAUD_57K6(FRSKY_USART);
    }
  else
    {
      USART_SET_BAUD_9K6(FRSKY_USART);
    }
  USART_SET_MODE_8N1(FRSKY_USART);
  USART_ENABLE_RX(FRSKY_USART); // Enable FrSky Telemetry reception.
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
  static uint8_t Buffer[FRSKY_TLM_PKT_SIZE];

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
          if (BufferCount < FRSKY_TLM_PKT_SIZE)
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
      else if (BufferCount < FRSKY_TLM_PKT_SIZE)
        {
          Buffer[BufferCount++] = data;
        }
      break;

    case STATE_DATA_XOR:
      if (BufferCount < FRSKY_TLM_PKT_SIZE)
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

  if (IS_USR_PROTO_SMART_PORT() && BufferCount >= (IS_SPIMODULES_PROTOCOL(g_model.rfProtocol)? FRSKY_TLM_PKT_SIZE - 1:FRSKY_TLM_PKT_SIZE))
    {
      LoadTelemBuffer(Buffer);
      dataState = STATE_DATA_IDLE;
    }
}

uint8_t checkSportPacket(uint8_t *packet)
{
  uint16_t crc = 0;
  for (uint8_t i=1; i<FRSKY_TLM_PKT_SIZE; i++)
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
  if (telemetryData.value.gpsLongitudeEW || telemetryData.value.gpsLatitudeNS)
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

void checkOffsetAndMaxCurrent()
{
  if ((int16_t)telemetryData.value.current > 0 && ((int16_t)telemetryData.value.current + g_model.telemetry.fasOffset) > 0)
    telemetryData.value.current += g_model.telemetry.fasOffset;
  else
    telemetryData.value.current = 0;

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

  if ((appId & 0xF000) == 0xF000)
  {
    if (appId == X_RSSI_ID)
    {
      if(IS_MULTIMODULE_PROTOCOL (s_current_protocol))
      { // Multiprotocol specific RSSI / LQI.
        telemetryData.rssi[0].set((sport_packet[5]) >> 1); // Multi TX_RSSI - units Decibel (dB).
        telemetryData.rssi[1].set(sport_packet[7]); // Multi TX_LQI - No units.
      }
      else
        telemetryData.rssi[0].set(SPORT_DATA_U8(sport_packet));
    }
    else if (appId == X_SWR_ID && (!IS_MULTIMODULE_PROTOCOL (s_current_protocol)))
      telemetryData.rssi[1].set(SPORT_DATA_U8(sport_packet));
    else if (appId == X_ADC1_ID || appId == X_ADC2_ID)
    {
    // A1/A2 of DxR receivers
      telemetryData.analog[appId-X_ADC1_ID].set(SPORT_DATA_U8(sport_packet),g_model.telemetry.channels[appId-X_ADC1_ID].type);

#if defined(VARIO)
       uint8_t varioSource = g_model.telemetry.varioSource - VARIO_SOURCE_A1;
       if (varioSource == appId-X_ADC1_ID)
         telemetryData.value.varioSpeed = applyChannelRatio(varioSource, telemetryData.analog[varioSource].value);
#endif
    }
    else if (appId == X_BATT_ID)
      telemetryData.analog[0].set(SPORT_DATA_U8(sport_packet),UNIT_VOLTS);
  }
    else if ((appId >> 8) == 0)
    {
      // The old FrSky IDs
      uint16_t value = HUB_DATA_U16(sport_packet);
      processHubPacket((uint8_t)appId, value);
    }
  /*else if (appId == BETA_BARO_ALT_ID) KEEP this ?
    {
      telemetryData.value.baroAltitude = ((SPORT_DATA_S32(sport_packet) >> 8)/10);
      manageBaroAltitude();
    }
  else if (appId == BETA_VARIO_ID)
    {
      int32_t varioSpeed = SPORT_DATA_S32(sport_packet);
      telemetryData.value.varioSpeed = 10 * (varioSpeed >> 8);
    }*/

//  if (appId & 0xF000)
//    return;  // Discard other "0xX000" value (todo use some other)

  uint8_t smallId = (uint8_t)(appId >> 4); // Forget last 4 bits -> We accept just one sensor by type

  switch (smallId)
    {
    case X_T1_ID :
      telemetryData.value.temperature1 = SPORT_DATA_S32(sport_packet);
      checkMaxTemperature1();
      break;
    case X_T2_ID :
      telemetryData.value.temperature2 = SPORT_DATA_S32(sport_packet);
      checkMaxTemperature2();
      break;
    case X_RPM_ID :
      telemetryData.value.rpm = SPORT_DATA_U32(sport_packet) / (g_model.telemetry.blades+2);
      checkMaxRpm();
      break;
    case X_FUEL_ID :
      telemetryData.value.fuelLevel = SPORT_DATA_U32(sport_packet);
      break;
    case X_ALT_ID :
      telemetryData.value.baroAltitude = SPORT_DATA_S32(sport_packet)/100;
      manageBaroAltitude();
      break;
    case X_VARIO_ID :
      telemetryData.value.varioSpeed = SPORT_DATA_S32(sport_packet);
      break;
    case X_ACCX_ID :
      telemetryData.value.accelX = SPORT_DATA_S32(sport_packet)/10;
      break;
    case X_ACCY_ID :
      telemetryData.value.accelY = SPORT_DATA_S32(sport_packet)/10;
      break;
    case X_ACCZ_ID :
      telemetryData.value.accelZ = SPORT_DATA_S32(sport_packet)/10;
      break;
    case X_CURR_ID :
      telemetryData.value.current = SPORT_DATA_U32(sport_packet);
      checkOffsetAndMaxCurrent();
      break;
    case X_VFAS_ID :
      telemetryData.value.vfas = SPORT_DATA_U32(sport_packet)/10;   //TODO: remove /10 and display with PREC2 when using SPORT
      break;
#if defined(GPS)
    case X_GPS_SPEED_ID :
      IF_GPS_IS_FIXED telemetryData.value.gpsSpeed_bp = SPORT_DATA_U32(sport_packet)/100;
      checkMaxGpsSpeed();
      break;
    case X_GPS_TIME_DATE_ID :
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
    break;
    case X_GPS_COURS_ID :
    {
      uint32_t course = SPORT_DATA_U32(sport_packet);
      IF_GPS_IS_FIXED
      {
        telemetryData.value.gpsCourse_bp = course / 100;
        telemetryData.value.gpsCourse_ap = course % 100;
      }
    }
    break;
    case X_GPS_ALT_ID :
      IF_GPS_IS_FIXED telemetryData.value.gpsAltitude = SPORT_DATA_S32(sport_packet)/100;
      manageGpsAltitude();
      break;
    case X_GPS_LONG_LATI_ID :
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
    break;
#endif
    case X_CELLS_ID :
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
      checkOffsetAndMaxCurrent();
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


void process_ibus_telem(void)
{
  // Over The Air packet format.
  // 0    1234   5678   9           10         11       12                 ......36
  // AC | TXID | rx_id | sensor id | sensor # | length | bytes | sensor id ......

  // AA | TXID | rx_id | sensor id | sensor # | value 16 bit   | sensor id ......
  // Max 7 sensors per packet, but first packet has 5 Receiver internal sensors.


  if (ibus_telem_buffer[0] == 0xAA) // 0xAA Normal telemetry.
  {
    uint8_t linkOk = 0;
    uint8_t temperature_sensor_count = 0;

    for (uint8_t sensor = 0; sensor < 7; sensor++)
    {
      uint8_t index = 1 + (4 * sensor); // Skip header.
      uint16_t data16 = ibus_telem_buffer[index + 3] << 8 | (ibus_telem_buffer[index + 2]);

      switch (ibus_telem_buffer[index])
      {
        case AFHDS2A_SENSOR_RX_VOLTAGE:
          if (ibus_telem_buffer[index + 1] == 0)
          { // Voltage is sent in two bytes as 0.01 Volt value.
            telemetryData.analog[TELEM_ANA_A1].set(
                ibus_telem_buffer[index + 3] << 6 | (ibus_telem_buffer[index + 2] >> 2), UNIT_VOLTS);
            linkOk = 1;
          }
          break;

        case AFHDS2A_SENSOR_RX_RSSI:
          telemetryData.rssi[1].set(-(ibus_telem_buffer[index + 2]));
          linkOk = 1;
          break;

        case AFHDS2A_SENSOR_TEMPERATURE:
          temperature_sensor_count++;
          if (temperature_sensor_count == 1)
            telemetryData.value.temperature1 = (data16 - 400) / 10;
          if (temperature_sensor_count == 2)
            telemetryData.value.temperature2 = (data16 - 400) / 10;
          break;

        case AFHDS2A_SENSOR_RPM:
          telemetryData.value.rpm = data16;
          break;

        case AFHDS2A_SENSOR_EXT_VOLTAGE:
//        if (ibus_telem_buffer[index + 1] == 1) // Sensor #1. Is this the first in the Daisy Chain of Sensors ?.
//        { // FS-CVT01 Voltage is sent as int16_t 0.01 Volt value.
          telemetryData.value.vfas = (int16_t) data16 / 10;
//        }
          break;

//    case AFHDS2A_SENSOR_RX_ERR_RATE:
//      if(ibus_telem_buffer[index+2]<=100)  RX_LQI=ibus_telem_buffer[index+2];
//      linkOk = 1;
//      break;

        case AFHDS2A_SENSOR_CLIMB_RATE:
          telemetryData.value.varioSpeed = (int16_t) data16;
          break;

        case AFHDS2A_SENSOR_COG:
          telemetryData.value.gpsCourse_bp = data16 / 100;
          telemetryData.value.gpsCourse_ap = data16 % 100;
          break;

        case AFHDS2A_SENSOR_GROUND_SPEED:
          telemetryData.value.gpsSpeed_bp = data16 / 100;
          telemetryData.value.gpsSpeed_ap = data16 % 100;
          break;


        case 0xff: // end of sensor data
          break;
      }

    }
    if (linkOk)
      frskyStreaming = frskyStreaming ? FRSKY_TIMEOUT10ms : FRSKY_TIMEOUT_FIRST;
    // frskyStreaming gets decremented every 10ms, FRSKY_TIMEOUT_FIRST value is detected to play connection prompt.
  }
  else if (ibus_telem_buffer[0] == 0xAC) // 0xAC Extended telemetry.
  {
    uint8_t * sensor = ibus_telem_buffer + 1; // Skip header Byte.

    while (sensor[0] != 0xFF)
    {
      if (sensor[2] != 4) break; // Len = 4 ... 32 bit sensor.
      int32_t dataS32 = *(int32_t *)(sensor+3);

      telemetryData.value.gpsFix =1;

      switch (sensor[0])
      {

        uint32_t decum, temp;

      case AFHDS2A_SENSOR_GPS_LAT:
        telemetryData.value.gpsLatitudeNS = (dataS32 > 0) ? 'N' : 'S';

        decum = labs(dataS32);
        temp = decum % 10000000; // Decimal Degrees.
        decum -= temp;
        telemetryData.value.gpsLatitude_bp = (decum / 10000000) * 100; // Integer Degrees stored * 100 (Frsky Hub format).
        temp *= 60;
        telemetryData.value.gpsLatitude_bp += temp / 10000000; // Stored as integer Minutes.

        temp %= 10000000;
        temp /= 1000;
        telemetryData.value.gpsLatitude_ap = temp; // Decimal Minutes.
        break;

      case AFHDS2A_SENSOR_GPS_LON:

        telemetryData.value.gpsLongitudeEW = (dataS32 > 0) ? 'E' : 'W';

        decum = labs(dataS32);
        temp = decum % 10000000; // Decimal Degrees.
        decum -= temp;
        telemetryData.value.gpsLongitude_bp = (decum / 10000000) * 100; // Integer Degrees stored * 100 (Frsky Hub format).
        temp *= 60;
        telemetryData.value.gpsLongitude_bp += temp / 10000000; // Stored as integer Minutes.

        temp %= 10000000;
        temp /= 1000;
        telemetryData.value.gpsLongitude_ap = temp; // Decimal Minutes.
        break;

      case AFHDS2A_SENSOR_GPS_ALT:
        //IF_GPS_IS_FIXED
        telemetryData.value.gpsAltitude = dataS32 / 100;
        //manageGpsAltitude();
        break;

      case AFHDS2A_SENSOR_ALT:
        telemetryData.value.baroAltitude = dataS32 / 100;
        break;
      }

      sensor += 7;
      if (sensor > ibus_telem_buffer + 28) break;
    }
  }
}


void telemetryInterrupt10ms()
{
#if 0 // defined(SPIMODULES)
  if (IS_SPIMODULES_PROTOCOL(g_model.rfProtocol))
  {
    if(g_model.rfProtocol == PROTOCOL_AFHDS2A)
      {
        if(ibus_telem_buffer[0]) process_AFHDS2A_telem();
        memclear(ibus_telem_buffer, 1); // Reset buffer.
      }
  }
#endif

  if (IS_USR_PROTO_IBUS())
  {
    if(ibus_telem_buffer[0]) process_ibus_telem();
    memclear(ibus_telem_buffer, 1); // Reset buffer.
  }
  else  if (IS_USR_PROTO_SMART_PORT())
  {
    for (uint8_t i = 0; i < NUM_TELEM_RX_BUFFER; ++i)
    {
      if (TelemetryRxBuffer[i][0] || TelemetryRxBuffer[i][1]) // Check if buffer data are present
      {
        processSportPacket (TelemetryRxBuffer[i]);
        memclear(TelemetryRxBuffer[i], FRSKY_TLM_PKT_SIZE); // Reset buffer.
      }
    }
  }
  else if (IS_USR_PROTO_FRSKY_HUB() || IS_USR_PROTO_WS_HOW_HIGH())
  {
    for (uint8_t i = 0; i < NUM_TELEM_RX_BUFFER; ++i)
    {
      if (TelemetryRxBuffer[i][0] || TelemetryRxBuffer[i][1]) // Check if buffer data are present
      {
        frskyDProcessPacket (TelemetryRxBuffer[i]);
        memclear(TelemetryRxBuffer[i], FRSKY_TLM_PKT_SIZE); // Reset buffer.
      }
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


void LoadAFHDS2ATelemBuffer(uint8_t *data)
{
  if (!ibus_telem_buffer[0])
  {
    memcpy(ibus_telem_buffer, data, IBUS_TLM_PACKET_SIZE - IBUS_TLM_HEADER +1);
    return;
  }
}


void LoadTelemBuffer(uint8_t *data)
{
  for (uint8_t i=0; i<NUM_TELEM_RX_BUFFER; ++i)
    {
      if (!(TelemetryRxBuffer[i][0] || TelemetryRxBuffer[i][1])) // Check buffer is free
        {
          memcpy(TelemetryRxBuffer[i], data, FRSKY_TLM_PKT_SIZE);
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

