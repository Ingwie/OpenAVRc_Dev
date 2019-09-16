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

/* Note: this file is MANDATORY if xmodem module is used in a non-arduino environment */
/* otherwise, the content of this file doesn't care and this file can be absent */

/* OpenAVRc environment */
#include "../OpenAVRc.h"
#include "../sdcard.h"

#define FILE_DESC                          struct fat_file_struct*
#define FILE_EXISTS(FullFileName)          sdFileExists((char *)FullFileName)
#define FILE_OPEN_FOR_READ(FullFileName)   sdFileOpenForRead((char *)FullFileName)
#define FILE_OPEN_FOR_WRITE(FullFileName)  sdFileOpenForWrite((char *)FullFileName)
#define FILE_SEEK(fd, Pos)                 fat_seek_file(fd, (int32_t *)&Pos, FAT_SEEK_SET)
#define FILE_READ_CHUNK(fd,  Buf, Size)    fat_read_file(fd, (uint8_t *)Buf, Size)
#define FILE_WRITE_CHUNK(fd, Buf, Size)    fat_write_file(fd, (const uint8_t *)Buf, Size)
#define FILE_SIZE(fd)                      fd->dir_entry.file_size /* /!\ TBC /!\ */
#define FILE_CLOSE(fd)                     fat_close_file(fd);
#define FILE_DELETE(FullFileName)          sdDeleteFile((const char *)FullFileName)

#define DELAY_MS(ms)                       _delay_ms(ms)
#define GET_TICK()                         (uint32_t)GET_10MS_TICK()
#define MS_TO_TICK(ms)                     MS_TO_10MS_TICK(ms)

#define YIELD_TO_PRIO_TASK()               YIELD_TO_TASK(checkMixer());

/* File management is not so obvious in OpenAVRc, so create here some functions to open files */
/**
 * \file   xmodem_cfg.cpp
 * \fn     static uint8_t getDirAndBaseName(char *FullFileName)
 * \brief  Split a full file path into a directory name (DirName) and a file name (BaseName)
 *
 * \param  FullFileName: pointer to the full file path string (eg: /MY_DIR/MY_FILE.TXT)
 * \return Offset to the BaseName string (eg: MY_FILE.TXT), FullFileName become a pointer to the DirName string (eg: /MY_DIR, or NULL if root)
 */
static uint8_t getDirAndBaseName(char *FullFileName)
{
  uint8_t FullFileNameLen;
  uint8_t Ret = 0;

  if(FullFileName) //  Check for NULL pointer
  {
    if(FullFileName[0] == '/') // First char SHALL be '/'
    {
      FullFileNameLen = strlen(FullFileName);
      char *BaseName = NULL; // Initialize BaseName to NULL
      for(uint8_t Idx = FullFileNameLen - 1; Idx; Idx--)
      {
        if(FullFileName[Idx] == '/')
        {
          if(Idx)
          {
            FullFileName[Idx] = 0; // Replace '/' with End of String
          }
          BaseName = FullFileName + Idx + 1;
          if(BaseName[0]) Ret = Idx+1; // OK, BaseName is at least one letter
          else            FullFileName[Idx] = '/'; //restore '/'
          break;
        }
      }
    }
  }
  return(Ret);
}

/**
 * \file   xmodem_cfg.cpp
 * \fn     static uint8_t sdFileExists(char *FullFileName)
 * \brief  Return if a file exists
 *
 * \param  FullFileName: pointer to the full file path string (eg: /MY_DIR/MY_FILE.TXT)
 * \return 0: Doesn't exist, 1: exists.
 */
static uint8_t sdFileExists(char *FullFileName)
{
  uint8_t Ret = 0;

  if(uint8_t ofs = getDirAndBaseName(FullFileName))
  {
    if(sdChangeCurDir(!FullFileName ? ROOT_PATH: FullFileName))
    {
      Ret = sdFindFileStruct(FullFileName + ofs);
    }
  }

  return(Ret);
}

/**
 * \file   xmodem_cfg.cpp
 * \fn     static fat_file_struct *sdFileOpen(char *FullFileName)
 * \brief  Open a file (for read or write) and return a file descriptor of the file passed as argument
 *
 * \param  FullFileName: pointer to the full file path string (eg: /MY_DIR/MY_FILE.TXT)
 * \return NULL: Error, non NULL: pointer on the file descriptor.
 */
static fat_file_struct *sdFileOpenForRead(char *FullFileName)
{
  fat_file_struct *fd = NULL;

  if(uint8_t ofs = getDirAndBaseName(FullFileName))
  {
    if(sdChangeCurDir(!FullFileName ? ROOT_PATH: FullFileName))
    {
      if(sdFindFileStruct(FullFileName + ofs))
      {
        fd = fat_open_file(SD_filesystem, &SD_dir_entry);
      }
    }
  }

  return(fd);
}

/**
 * \file   xmodem_cfg.cpp
 * \fn     static fat_file_struct *sdFileOpen(char *FullFileName)
 * \brief  Open a file (for read or write) and return a file descriptor of the file passed as argument
 *
 * \param  FullFileName: pointer to the full file path string (eg: /MY_DIR/MY_FILE.TXT)
 * \return NULL: Error, non NULL: pointer on the file descriptor.
 */
static fat_file_struct *sdFileOpenForWrite(char *FullFileName)
{
  fat_file_struct *fd = NULL;

  if(uint8_t ofs = getDirAndBaseName(FullFileName))
  {
    if(sdOpenDir(!FullFileName ? ROOT_PATH: FullFileName))
    {
      if(fat_create_file(SD_dir, (FullFileName + ofs), &SD_dir_entry))
      {
        fd = fat_open_file(SD_filesystem, &SD_dir_entry);
      }
    }
  }

  return(fd);
}

