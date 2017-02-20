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


#ifndef _SDCARD_H_
#define _SDCARD_H_

#include "thirdparty/FatFs/ff.h"
#include "translations.h"

#define ROOT_PATH           "/"
#define MODELS_PATH         ROOT_PATH "MODELS"      // no trailing slash = important
#define LOGS_PATH           ROOT_PATH "LOGS"
#define VOICETXT_PATH       ROOT_PATH "VOICE"

#define VOICETXT_FILE       "list.txt"

#define MODELS_EXT          ".bin"
#define LOGS_EXT            ".csv"
#define TEXT_EXT            ".txt"

extern FATFS g_FATFS_Obj;
extern FIL g_oLogFile;

extern uint8_t logDelay;
const pm_char *openLogs();
void writeHeader();
void closeLogs();
void writeLogs();

uint32_t sdGetNoSectors();
uint32_t sdGetSize();
uint32_t sdGetFreeSectors();

inline const pm_char *SDCARD_ERROR(FRESULT result)
{
  if (result == FR_NOT_READY)
    return STR_NO_SDCARD;
  else
    return STR_SDCARD_ERROR;
}

#define O9X_FOURCC 0x3178396F // o9x for gruvin9x/MEGA2560

extern bool listSdFiles(const char *path, const char *extension, const uint8_t maxlen, const char *selection, uint8_t flags);

const char *fileCopy(const char *filename, const char *srcDir, const char *destDir);

#endif

