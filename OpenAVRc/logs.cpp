/*
**************************************************************************
*                                                                        *
*              This file is part of the OpenAVRc project.                *
*                                                                        *
*                         Based on code named                            *
*             OpenTx - https://github.com/opentx/opentx                  *
*                                                                        *
*                Only AVR code here for lisibility ;-)                   *
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
#include "thirdparty/FatFs/ff.h"

FIL g_oLogFile = {0};
uint8_t logDelay;

#define get2PosState(sw) (switchState(SW_ ## sw) ? -1 : 1)

#define get3PosState(sw) (switchState(SW_ ## sw ## 0) ? -1 : (switchState(SW_ ## sw ## 2) ? 1 : 0))

const pm_char *openLogs()
{
  // Determine and set log file filename
  FRESULT result;
  DIR folder;
  char filename[34]; // /LOGS/modelnamexxx-2013-01-01.log

  if (!sdMounted())
    return STR_NO_SDCARD;

  if (sdGetFreeSectors() == 0)
    return STR_SDCARD_FULL;

  // check and create folder here
  strcpy_P(filename, STR_LOGS_PATH);
  result = f_opendir(&folder, filename);
  if (result != FR_OK) {
    if (result == FR_NO_PATH)
      result = f_mkdir(filename);
    if (result != FR_OK)
      return SDCARD_ERROR(result);
  }

  filename[sizeof(LOGS_PATH)-1] = '/';
  memcpy(&filename[sizeof(LOGS_PATH)], g_model.header.name, sizeof(g_model.header.name));
  filename[sizeof(LOGS_PATH)+sizeof(g_model.header.name)] = '\0';

  uint8_t i = sizeof(LOGS_PATH)+sizeof(g_model.header.name)-1;
  uint8_t len = 0;
  while (i>sizeof(LOGS_PATH)-1) {
    if (!len && filename[i])
      len = i+1;
    if (len) {
      if (filename[i])
        filename[i] = idx2char(filename[i]);
      else
        filename[i] = '_';
    }
    i--;
  }

  if (len == 0) {
    uint8_t num = g_eeGeneral.currModel + 1;
    strcpy_P(&filename[sizeof(LOGS_PATH)], STR_MODEL);
    filename[sizeof(LOGS_PATH) + PSIZE(TR_MODEL)] = (char)((num / 10) + '0');
    filename[sizeof(LOGS_PATH) + PSIZE(TR_MODEL) + 1] = (char)((num % 10) + '0');
    len = sizeof(LOGS_PATH) + PSIZE(TR_MODEL) + 2;
  }


#if defined(RTCLOCK)
  char * tmp = strAppendDate(&filename[len]);
#else
  char * tmp = &filename[len];
#endif

  strcpy_P(tmp, STR_LOGS_EXT);

  result = f_open(&g_oLogFile, filename, FA_OPEN_ALWAYS | FA_WRITE);
  if (result != FR_OK) {
    return SDCARD_ERROR(result);
  }

  if (f_size(&g_oLogFile) == 0) {
    writeHeader();
  } else {
    result = f_lseek(&g_oLogFile, f_size(&g_oLogFile)); // append
    if (result != FR_OK) {
      return SDCARD_ERROR(result);
    }
  }

  return NULL;
}

tmr10ms_t lastLogTime = 0;

void closeLogs()
{
  if (f_close(&g_oLogFile) != FR_OK) {
    // close failed, forget file
    g_oLogFile.fs = 0;
  }
  lastLogTime = 0;
}

getvalue_t getConvertedTelemetryValue(getvalue_t val, uint8_t unit)
{
  convertUnit(val, unit);
  return val;
}

void writeHeader()
{
#if defined(RTCLOCK)
  f_puts("Date,Time,", &g_oLogFile);
#else
  f_puts("Time,", &g_oLogFile);
#endif

#if defined(FRSKY)
  f_puts("Buffer,RX,TX,A1,A2,", &g_oLogFile);
#if defined(FRSKY_HUB)
  if (IS_USR_PROTO_FRSKY_HUB()) {
    f_puts("GPS Date,GPS Time,Long,Lat,Course,GPS Speed(kts),GPS Alt,Baro Alt(", &g_oLogFile);
    f_puts(TELEMETRY_BARO_ALT_UNIT, &g_oLogFile);
    f_puts("),Vertical Speed,Air Speed(kts),Temp1,Temp2,RPM,Fuel," TELEMETRY_CELLS_LABEL "Current,Consumption,Vfas,AccelX,AccelY,AccelZ,", &g_oLogFile);
  }
#endif
#if defined(WS_HOW_HIGH)
  if (IS_USR_PROTO_WS_HOW_HIGH()) {
    f_puts("WSHH Alt,", &g_oLogFile);
  }
#endif
#endif

  f_puts("Rud,Ele,Thr,Ail,P1,P2,P3,THR,RUD,ELE,3POS,AIL,GEA,TRN\n", &g_oLogFile);
}

void writeLogs()
{
  static const pm_char * error_displayed = NULL;

  if (isFunctionActive(FUNCTION_LOGS) && logDelay > 0) {
    tmr10ms_t tmr10ms = get_tmr10ms();
    if (lastLogTime == 0 || (tmr10ms_t)(tmr10ms - lastLogTime) >= (tmr10ms_t)logDelay*10) {
      lastLogTime = tmr10ms;

      if (!g_oLogFile.fs) {
        const pm_char * result = openLogs();
        if (result != NULL) {
          if (result != error_displayed) {
            error_displayed = result;
            POPUP_WARNING(result);
          }
          return;
        }
      }

#if defined(RTCLOCK)
      {
        static struct gtm utm;
        static gtime_t lastRtcTime = 0;
        if ( g_rtcTime != lastRtcTime ) {
          lastRtcTime = g_rtcTime;
          gettime(&utm);
        }
        f_printf(&g_oLogFile, "%4d-%02d-%02d,%02d:%02d:%02d.%02d0,", utm.tm_year+1900, utm.tm_mon+1, utm.tm_mday, utm.tm_hour, utm.tm_min, utm.tm_sec, g_ms100);
      }
#else
      f_printf(&g_oLogFile, "%d,", tmr10ms);
#endif

#if defined(FRSKY)
      f_printf(&g_oLogFile, "%d,%d,%d,", frskyStreaming, RAW_FRSKY_MINMAX(frskyData.rssi[0]), RAW_FRSKY_MINMAX(frskyData.rssi[1]));
      for (uint8_t i=0; i<MAX_FRSKY_A_CHANNELS; i++) {
        int16_t converted_value = applyChannelRatio(i, RAW_FRSKY_MINMAX(frskyData.analog[i]));
        f_printf(&g_oLogFile, "%d.%02d,", converted_value/100, converted_value%100);
      }

#if defined(FRSKY_HUB)
      TELEMETRY_BARO_ALT_PREPARE();

      if (IS_USR_PROTO_FRSKY_HUB()) {
        f_printf(&g_oLogFile, "%4d-%02d-%02d,%02d:%02d:%02d,%03d.%04d%c,%03d.%04d%c,%03d.%02d," TELEMETRY_GPS_SPEED_FORMAT TELEMETRY_GPS_ALT_FORMAT TELEMETRY_BARO_ALT_FORMAT TELEMETRY_VSPEED_FORMAT TELEMETRY_ASPEED_FORMAT "%d,%d,%d,%d," TELEMETRY_CELLS_FORMAT TELEMETRY_CURRENT_FORMAT "%d," TELEMETRY_VFAS_FORMAT "%d,%d,%d,",
                 frskyData.hub.year+2000,
                 frskyData.hub.month,
                 frskyData.hub.day,
                 frskyData.hub.hour,
                 frskyData.hub.min,
                 frskyData.hub.sec,
                 frskyData.hub.gpsLongitude_bp,
                 frskyData.hub.gpsLongitude_ap,
                 frskyData.hub.gpsLongitudeEW ? frskyData.hub.gpsLongitudeEW : '-',
                 frskyData.hub.gpsLatitude_bp,
                 frskyData.hub.gpsLatitude_ap,
                 frskyData.hub.gpsLatitudeNS ? frskyData.hub.gpsLatitudeNS : '-',
                 frskyData.hub.gpsCourse_bp,
                 frskyData.hub.gpsCourse_ap,
                 TELEMETRY_GPS_SPEED_ARGS
                 TELEMETRY_GPS_ALT_ARGS
                 TELEMETRY_BARO_ALT_ARGS
                 TELEMETRY_VSPEED_ARGS
                 TELEMETRY_ASPEED_ARGS
                 frskyData.hub.temperature1,
                 frskyData.hub.temperature2,
                 frskyData.hub.rpm,
                 frskyData.hub.fuelLevel,
                 TELEMETRY_CELLS_ARGS
                 TELEMETRY_CURRENT_ARGS
                 frskyData.hub.currentConsumption,
                 TELEMETRY_VFAS_ARGS
                 frskyData.hub.accelX,
                 frskyData.hub.accelY,
                 frskyData.hub.accelZ);
      }
#endif

#if defined(WS_HOW_HIGH)
      if (IS_USR_PROTO_WS_HOW_HIGH()) {
        f_printf(&g_oLogFile, "%d,", TELEMETRY_RELATIVE_BARO_ALT_BP);
      }
#endif
#endif

      for (uint8_t i=0; i<NUM_STICKS+NUM_POTS; i++) {
        f_printf(&g_oLogFile, "%d,", calibratedStick[i]);
      }

      int result = f_printf(&g_oLogFile, "%d,%d,%d,%d,%d,%d,%d\n",
                            get2PosState(THR),
                            get2PosState(RUD),
                            get2PosState(ELE),
                            get3PosState(ID),
                            get2PosState(AIL),
                            get2PosState(GEA),
                            get2PosState(TRN));

      if (result<0 && !error_displayed) {
        error_displayed = STR_SDCARD_ERROR;
        POPUP_WARNING(STR_SDCARD_ERROR);
        closeLogs();
      }
    }
  } else {
    error_displayed = NULL;
    if (g_oLogFile.fs) {
      closeLogs();
    }
  }
}



