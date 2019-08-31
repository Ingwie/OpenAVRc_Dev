#ifndef XMODEM_CFG_H
#define XMODEM_CFG_H

/* Note: this configuration file is MANDATORY if xmodem module is used in a non-arduino environment */
/* otherwise, the content of this file doesn't care and this file can be absent */

/* OpenAVRc environment */
#include "../OpenAVRc.h"
#include "sdcard.h"

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
 * \file   xmodem_cfg.h
 * \fn     static uint8_t getDirAndBaseName(char *FullFileName, char *DirName, char *BaseName)
 * \brief  Split a full file path into a directory name (DirName) and a file name (BaseName)
 *
 * \param  FullFileName: pointer to the full file path string (eg: /MY_DIR/MY_FILE.TXT)
 * \param  DirName:  pointer to the DirName string (eg: /MY_DIR/, or NULL if root)
 * \param  BaseName: pointer to the BaseName string (eg: MY_FILE.TXT)
 * \return 0: Error, 1: OK.
 */
static uint8_t getDirAndBaseName(char *FullFileName, char *DirName, char *BaseName)
{
  uint8_t FullFileNameLen;
  uint8_t Ret = 0;

  if(FullFileName) //  Check for NULL pointer
  {
    if(FullFileName[0] == '/') // First char SHALL be '/'
    {
      FullFileNameLen = strlen(FullFileName);
      DirName  = NULL; // Initialize DirName  to NULL (means DirName is root)
      BaseName = NULL; // Initialize BaseName to NULL
      for(uint8_t Idx = FullFileNameLen - 1; Idx; Idx--)
      {
        if(FullFileName[Idx] == '/')
        {
          if(Idx)
          {
            DirName = FullFileName;
            FullFileName[Idx] = 0; // Replace '/' with End of String
          }
          BaseName = FullFileName + Idx + 1;
          if(BaseName[0]) Ret = 1; // OK, BsaeName is at least one letter
          else            FullFileName[Idx] = '/'; //restore '/'
          break;
        }
      }
    }
  }
  return(Ret);
}

/**
 * \file   xmodem_cfg.h
 * \fn     static uint8_t sdFileExists(char *FullFileName)
 * \brief  Return if a file exists
 *
 * \param  FullFileName: pointer to the full file path string (eg: /MY_DIR/MY_FILE.TXT)
 * \return 0: Doesn't exist, 1: exists.
 */
static uint8_t sdFileExists(char *FullFileName)
{
  char   *DirName  = NULL; // If full path is /MY_DIR/MY_FILE.TXT, DirName  is /MY_DIR
  char   *BaseName = NULL; // If full path is /MY_DIR/MY_FILE.TXT, BaseName is MY_FILE.TXT
  uint8_t Ret = 0;

  if(getDirAndBaseName(FullFileName, DirName, BaseName))
  {
    if(sdChangeCurDir(!DirName ? ROOT_PATH: DirName))
    {
      Ret = sdFindFileStruct(BaseName);
    }
  }

  return(Ret);
}

/**
 * \file   xmodem_cfg.h
 * \fn     static fat_file_struct *sdFileOpen(char *FullFileName)
 * \brief  Open a file (for read or write) and return a file descriptor of the file passed as argument
 *
 * \param  FullFileName: pointer to the full file path string (eg: /MY_DIR/MY_FILE.TXT)
 * \return NULL: Error, non NULL: pointer on the file descriptor.
 */
static fat_file_struct *sdFileOpenForRead(char *FullFileName)
{
  char *DirName       = NULL; // If full path is /MY_DIR/MY_FILE.TXT, DirName  is /MY_DIR
  char *BaseName      = NULL; // If full path is /MY_DIR/MY_FILE.TXT, BaseName is MY_FILE.TXT
  fat_file_struct *fd = NULL;

  if(getDirAndBaseName(FullFileName, DirName, BaseName))
  {
    if(sdChangeCurDir(!DirName ? ROOT_PATH: DirName))
    {
      if(sdFindFileStruct(BaseName))
      {
        fd = fat_open_file(SD_filesystem, &SD_dir_entry);
      }
    }
  }

  return(fd);
}

/**
 * \file   xmodem_cfg.h
 * \fn     static fat_file_struct *sdFileOpen(char *FullFileName)
 * \brief  Open a file (for read or write) and return a file descriptor of the file passed as argument
 *
 * \param  FullFileName: pointer to the full file path string (eg: /MY_DIR/MY_FILE.TXT)
 * \return NULL: Error, non NULL: pointer on the file descriptor.
 */
static fat_file_struct *sdFileOpenForWrite(char *FullFileName)
{
  char *DirName       = NULL; // If full path is /MY_DIR/MY_FILE.TXT, DirName  is /MY_DIR
  char *BaseName      = NULL; // If full path is /MY_DIR/MY_FILE.TXT, BaseName is MY_FILE.TXT
  fat_file_struct *fd = NULL;

  if(getDirAndBaseName(FullFileName, DirName, BaseName))
  {
    if(sdOpenDir(!DirName ? ROOT_PATH: DirName))
    {
      if(fat_create_file(SD_dir, BaseName, &SD_dir_entry))
      {
        fd = fat_open_file(SD_filesystem, &SD_dir_entry);
      }
    }
  }

  return(fd);
}

#endif
