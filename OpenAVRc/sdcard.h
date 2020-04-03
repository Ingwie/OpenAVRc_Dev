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

#ifndef _SDCARD_H_
#define _SDCARD_H_

#if !defined(SIMU)
  #include "fat.h"
  #include "fat_config.h"
  #include "partition.h"
  #include "sd_raw.h"
  #include "sd_raw_config.h"
#else
  #include "inttypes.h"
#endif

#define ROOT_PATH           "/"
#define MODELS_PATH         "MODELS"      // no trailing slash = important
#define LOGS_PATH           "LOGS"
#define VOICETXT_PATH       "VOICE"
#if defined(SIMU)
#define sdMounted()         (1)
#else
#define sdMounted()         (SD_filesystem)
#endif

#define VOICETXT_FILE       "list.txt"

#define MODELS_EXT          ".bin"
#define LOGS_EXT            ".csv"
#define TEXT_EXT            ".txt"

#define REUSED_SD_RAWBLOCK_BUFFER_SIZE 512 // 512 bytes of LCD buffer reused

extern struct partition_struct* SD_partition;
extern struct fat_fs_struct*    SD_filesystem;
extern struct fat_dir_entry_struct SD_dir_entry;
extern struct fat_dir_struct* SD_dir;
extern struct fat_file_struct* SD_file;

uint8_t MountSD();
void UmountSD();
void sdCreateSystemDir();
uint8_t sdChangeCurDir(const char* path);
uint8_t sdOpenModelsDir();
uint8_t sdOpenLogsDir();


uint8_t sdFindFileStruct(const char* name);
uint8_t sdDeleteFile(const char* name);


extern uint8_t logDelay;
uint8_t openLogs();
uint8_t writeHeader();
void closeLogs();
void closeLogIfActived();
void writeLogs();
void SdBufferClear();

#define OAVRC_FOURCC 0x16180339 // OAVRC ID

extern uint8_t listSdFiles(const char *path, const char *extension, const uint8_t maxlen, const char *selection, uint8_t flags);

const char *fileCopy(const char *filename, const char *srcDir, const char *destDir);

uint8_t setSdModelName(char *filename, uint8_t nummodel);

uint8_t sdOpenDir(const char* path);

#endif

