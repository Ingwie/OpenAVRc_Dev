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


#include "OpenAVRc.h"
#include <stdio.h>

uint8_t logDelay = 0;
tmr10ms_t lastLogTime = 0;

#define get2PosState(sw) (switchState(SW_ ## sw) ? -1 : 1)

#define get3PosState(sw) (switchState(SW_ ## sw ## 0) ? -1 : (switchState(SW_ ## sw ## 2) ? 1 : 0))

uint8_t openLogs()
{
  char *filename = reusableBuffer.modelsel.mainname; // modelnamex-2013-01-01.log
  uint8_t ret = 0;

  if (!sdMounted())
    {
      POPUP_WARNING(STR_NO_SDCARD);
      return ret;
    }

  if (sdOpenLogsDir())
    {
      /* Build the name of the file */
      memcpy(filename, g_model.name, sizeof(g_model.name));
      filename[sizeof(g_model.name)] = '\0';
      uint8_t len = setSdModelName(filename, g_eeGeneral.currModel);
#if defined(RTCLOCK)
      char * tmp = strAppendDate(&filename[len]);
#else
      char * tmp = &filename[len];
#endif
      strcpy_P(tmp, STR_LOGS_EXT);

      /* Create the file */
      uint8_t state = fat_create_file(SD_dir, filename, &SD_dir_entry);

      //if (!state) POPUP_WARNING(PSTR("nocreatefilefail"));//debug

      SD_file = fat_open_file(SD_filesystem, &SD_dir_entry);

      if (state == 1) // = File not already exist
        {
          if (writeHeader())
            {
              ret = 1;
            }
          //else POPUP_WARNING(PSTR("nowriteheader"));//debug
        }
      else if (state == 2) // = File already exist
        {
          int32_t seekofs = 0;
          if(fat_seek_file(SD_file, &seekofs, FAT_SEEK_END)) // Seek at offset 0 from the end of the file
            {
              ret = 1;
            }
          //else POPUP_WARNING(PSTR("noseek"));//debug
        }
    }
  return ret;
}

void closeLogs()
{
  /* close file */
  fat_close_file(SD_file);
  SD_file = 0;
  lastLogTime = 0;
  logDelay = 0;
  memclear(&reusableBuffer, sizeof(reusableBuffer));
}

void closeLogIfActived()
{
  if (logDelay)
    {
      closeLogs();
    }
}

getvalue_t getConvertedTelemetryValue(getvalue_t val, uint8_t unit)
{
  convertUnit(val, unit);
  return val;
}


#if defined(RTCLOCK)
  #define TR_LOGS_TIME "Date,Time,"
#else
  #define TR_LOGS_TIME "Time,"
#endif
const pm_char STR_LOGS_TIME[] PROGMEM = TR_LOGS_TIME;

#if defined(FRSKY)
  #define TR_LOGS_TELEMETRYHUB  "Buffer,RX,TX,A1,A2,GPS Date,GPS Time,Long,Lat,Course,GPS Speed(kts),GPS Alt,Baro Alt(" TELEMETRY_BARO_ALT_UNIT "),Vertical Speed,Air Speed(kts),Temp1,Temp2,RPM,Fuel," TELEMETRY_CELLS_LABEL "Current,Consumption,Vfas,AccelX,AccelY,AccelZ,"
  const pm_char STR_LOGS_TELEMETRYHUB[] PROGMEM = TR_LOGS_TELEMETRYHUB;
  #if defined(WS_HOW_HIGH)
    #define TR_LOGS_TELEMETRYWSHH  "Buffer,RX,TX,A1,A2,WSHH Alt,"
    const pm_char STR_LOGS_TELEMETRYWSHH[] PROGMEM = TR_LOGS_TELEMETRYWSHH;
  #endif
#endif

#define TR_LOGS_COMMANDS  "Rud,Ele,Thr,Ail,P1,P2,P3,THR,RUD,ELE,3POS,AIL,GEA,TRN\n"
const pm_char STR_LOGS_COMMANDS[] PROGMEM = TR_LOGS_COMMANDS;

uint8_t writeHeader()
{
  char *text = reusableBuffer.logsbuffer.data;

  strcpy_P(text,STR_LOGS_TIME);

  /* write text to file */
  if(fat_write_file(SD_file, (uint8_t*)text, PSIZE(TR_LOGS_TIME)) != PSIZE(TR_LOGS_TIME))
    {
      return 0;
    }

  uint8_t len;

#if defined(FRSKY)
  if (IS_USR_PROTO_FRSKY_HUB() || IS_USR_PROTO_SMART_PORT())
    {
      strcpy_P(text,STR_LOGS_TELEMETRYHUB);
      len = PSIZE(TR_LOGS_TELEMETRYHUB);
    }
#if defined(WS_HOW_HIGH)
  if (IS_USR_PROTO_WS_HOW_HIGH())
    {
      strcpy_P(text,STR_LOGS_TELEMETRYWSHH);
      len = PSIZE(TR_LOGS_TELEMETRYWSHH);
    }
#endif

  /* write text to file */
  if(fat_write_file(SD_file, (uint8_t*)text, len) != len)
    {
      return 0;
    }
#endif

  strcpy_P(text,STR_LOGS_COMMANDS);
  len = PSIZE(TR_LOGS_COMMANDS);

  /* write text to file */
  if(fat_write_file(SD_file, (uint8_t*)text, len) != len)
    {
      return 0;
    }
  return 1;
}


#if defined(RTCLOCK)
#define TR_RTC_MASK "%4d-%02d-%02d,%02d:%02d:%02d,"
const pm_char STR_RTC_MASK[] PROGMEM = TR_RTC_MASK;
#endif
#if defined(FRSKY)
#define TR_FRSKY_RSSI_MASK "%d,%d,%d,"
const pm_char STR_FRSKY_RSSI_MASK[] PROGMEM = TR_FRSKY_RSSI_MASK;
#define TR_FRSKY_ANA_MASK "%d.%02d,"
const pm_char STR_FRSKY_ANA_MASK[] PROGMEM = TR_FRSKY_ANA_MASK;
#define TR_FRSKY_TELE_MASK "%4d-%02d-%02d,%02d:%02d:%02d,%03d.%04d%c,%03d.%04d%c,%03d.%02d," TELEMETRY_GPS_SPEED_FORMAT TELEMETRY_GPS_ALT_FORMAT TELEMETRY_BARO_ALT_FORMAT TELEMETRY_VSPEED_FORMAT TELEMETRY_ASPEED_FORMAT "%d,%d,%d,%d," TELEMETRY_CELLS_FORMAT TELEMETRY_CURRENT_FORMAT "%d," TELEMETRY_VFAS_FORMAT "%d,%d,%d,"
const pm_char STR_FRSKY_TELE_MASK[] PROGMEM = TR_FRSKY_TELE_MASK;
#endif
#define TR_D_MASK "%d,"
const pm_char STR_D_MASK[] PROGMEM = TR_D_MASK;
#define TR_SWITCHS_MASK "%d,%d,%d,%d,%d,%d,%d\n"
const pm_char STR_SWITCHS_MASK[] PROGMEM = TR_SWITCHS_MASK;

void writeLogs()
{
  char *text = reusableBuffer.logsbuffer.data;
  uint8_t pos = 0;

  if (logDelay)
    {
      tmr10ms_t tmr10ms = getTmr10ms();
      if (lastLogTime == 0 || (tmr10ms_t)(tmr10ms - lastLogTime) >= (tmr10ms_t)logDelay*100)
        {
          lastLogTime = tmr10ms;

          if (!SD_file)
            {
              if (!openLogs())
                {
                  closeLogs(); // force re entrance
                  return;
                }
              return;
            }

#if defined(RTCLOCK)
          {
            static struct tm * utm;
            static time_t lastRtcTime = 0;
            if ( g_rtcTime != lastRtcTime )
              {
                lastRtcTime = g_rtcTime;
                utm = localtime(&g_rtcTime);
              }
            pos = sprintf_P(text, STR_RTC_MASK, utm->tm_year+T1900_OFFSET, utm->tm_mon+1, utm->tm_mday, utm->tm_hour, utm->tm_min, utm->tm_sec);
          }
#else
          pos = sprintf_P(text, STR_D_MASK, tmr10ms);
#endif

#if defined(FRSKY)
          pos += sprintf_P(&text[pos], STR_FRSKY_RSSI_MASK, frskyStreaming, RAW_FRSKY_MINMAX(telemetryData.rssi[0]), RAW_FRSKY_MINMAX(telemetryData.rssi[1]));
          for (uint8_t i=0; i<MAX_FRSKY_A_CHANNELS; i++)
            {
              int16_t converted_value = applyChannelRatio(i, RAW_FRSKY_MINMAX(telemetryData.analog[i]));
              pos += sprintf_P(&text[pos], STR_FRSKY_ANA_MASK, converted_value/100, converted_value%100);
            }

          if (IS_USR_PROTO_FRSKY_HUB() || IS_USR_PROTO_SMART_PORT())
            {
              pos += sprintf_P(&text[pos], STR_FRSKY_TELE_MASK,
                             telemetryData.value.year+2000,
                             telemetryData.value.month,
                             telemetryData.value.day,
                             telemetryData.value.hour,
                             telemetryData.value.min,
                             telemetryData.value.sec,
                             telemetryData.value.gpsLongitude_bp,
                             telemetryData.value.gpsLongitude_ap,
                             telemetryData.value.gpsLongitudeEW ? telemetryData.value.gpsLongitudeEW : '-',
                             telemetryData.value.gpsLatitude_bp,
                             telemetryData.value.gpsLatitude_ap,
                             telemetryData.value.gpsLatitudeNS ? telemetryData.value.gpsLatitudeNS : '-',
                             telemetryData.value.gpsCourse_bp,
                             telemetryData.value.gpsCourse_ap,
                             TELEMETRY_GPS_SPEED_ARGS
                             TELEMETRY_GPS_ALT_ARGS
                             TELEMETRY_BARO_ALT_ARGS
                             TELEMETRY_VSPEED_ARGS
                             TELEMETRY_ASPEED_ARGS
                             telemetryData.value.temperature1,
                             telemetryData.value.temperature2,
                             telemetryData.value.rpm,
                             telemetryData.value.fuelLevel,
                             TELEMETRY_CELLS_ARGS
                             TELEMETRY_CURRENT_ARGS
                             telemetryData.value.currentConsumption,
                             TELEMETRY_VFAS_ARGS
                             telemetryData.value.accelX,
                             telemetryData.value.accelY,
                             telemetryData.value.accelZ);
            }

#if defined(WS_HOW_HIGH)
          if (IS_USR_PROTO_WS_HOW_HIGH())
            {
              pos += sprintf_P(&text[pos], STR_D_MASK, TELEMETRY_RELATIVE_BARO_ALT_BP);
            }
#endif
#endif

          for (uint8_t i=0; i<NUM_STICKS+NUM_POTS; i++)
            {
              pos += sprintf_P(&text[pos], STR_D_MASK, calibratedStick[i]);
            }

          pos += sprintf_P(&text[pos], STR_SWITCHS_MASK,
                         get2PosState(THR),
                         get2PosState(RUD),
                         get2PosState(ELE),
                         get3PosState(ID),
                         get2PosState(AIL),
                         get2PosState(GEA),
                         get2PosState(TRN));

          if(fat_write_file(SD_file, (uint8_t*)text, pos) != pos)
            {
              POPUP_WARNING(STR_SDCARD_ERROR);
              closeLogs();
            }
        }
      if (!isFunctionActive(FUNCTION_LOGS))
        {
          closeLogs();
        }
    }
}



