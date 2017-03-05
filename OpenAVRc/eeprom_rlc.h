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


#ifndef eeprom_rlc_h
#define eeprom_rlc_h

#include "OpenAVRc.h"
#include "timers.h"


// TODO duplicated
#ifndef PACK
#define PACK( __Declaration__ ) __Declaration__ __attribute__((__packed__))
#endif

#if defined(CPUM2560) || defined(CPUM2561) || defined(CPUM128)
#if defined(EXTERNALEEPROM)
#define blkid_t    uint16_t
#define EESIZE     4096*2
#define EEFS_VERS  6
#define MAXFILES   36*2
#define BS         16
#else
#define blkid_t    uint8_t
#define EESIZE     4096
#define EEFS_VERS  5
#define MAXFILES   36
#define BS         16
#endif
#else
#define blkid_t    uint8_t
#define EESIZE     2048
#define EEFS_VERS  4
#define MAXFILES   20
#define BS         16
#endif


PACK(struct DirEnt { // File header
  blkid_t  startBlk;
  uint16_t size:12;
  uint16_t typ:4;
});

#define EEFS_EXTRA_FIELDS

PACK(struct EeFs {
  uint8_t  version;
  blkid_t  mySize;
  blkid_t  freeList;
  uint8_t  bs;
  EEFS_EXTRA_FIELDS
  DirEnt files[MAXFILES];
});



extern EeFs eeFs;

#define FILE_TYP_GENERAL 1
#define FILE_TYP_MODEL   2

/// fileId of general file
#define FILE_GENERAL   0
/// convert model number 0..MAX_MODELS-1  int fileId
#define FILE_MODEL(n) (1+(n))
#define FILE_TMP      (1+MAX_MODELS)

#define RESV          sizeof(EeFs)  //reserv for eeprom header with directory (eeFs)

#if defined(CPUM64)
#define FIRSTBLK      (RESV/BS)
#define BLOCKS        (EESIZE/BS)
#define BLOCKS_OFFSET 0
#else
#define FIRSTBLK      1
#define BLOCKS        (1+(EESIZE-RESV)/BS)
#define BLOCKS_OFFSET (RESV-BS)
#endif


void eepromFormat();
uint16_t EeFsGetFree();

extern volatile int8_t eeprom_buffer_size;

class EFile
{
public:

  ///remove contents of given file
  static void rm(uint8_t i_fileId);

  ///swap contents of file1 with them of file2
  static void swap(uint8_t i_fileId1, uint8_t i_fileId2);

  ///return true if the file with given fileid exists
  static bool exists(uint8_t i_fileId);

  ///open file for reading, no close necessary
  void openRd(uint8_t i_fileId);

  uint8_t read(uint8_t *buf, uint8_t len);

//  protected:

  uint8_t  m_fileId;    //index of file in directory = filename
  uint16_t m_pos;       //over all filepos
  blkid_t  m_currBlk;   //current block.id
  uint8_t  m_ofs;       //offset inside of the current block
};

#define eeFileSize(f)   eeFs.files[f].size
#define eeModelSize(id) eeFileSize(FILE_MODEL(id))

#define ERR_NONE 0
#define ERR_FULL 1
extern uint8_t  s_write_err;    // error reasons

extern uint8_t  s_sync_write;
#define ENABLE_SYNC_WRITE(val) s_sync_write = val;
#define IS_SYNC_WRITE_ENABLE() s_sync_write

///deliver current errno, this is reset in open
inline uint8_t write_errno()
{
  return s_write_err;
}

class RlcFile: public EFile
{
  uint8_t  m_bRlc;      // control byte for run length decoder
  uint8_t  m_zeroes;

  uint8_t m_flags;
#define WRITE_FIRST_LINK               0x01
#define WRITE_NEXT_LINK_1              0x02
#define WRITE_NEXT_LINK_2              0x03
#define WRITE_START_STEP               0x10
#define WRITE_FREE_UNUSED_BLOCKS_STEP1 0x20
#define WRITE_FREE_UNUSED_BLOCKS_STEP2 0x30
#define WRITE_FINAL_DIRENT_STEP        0x40
#define WRITE_TMP_DIRENT_STEP          0x50
  uint8_t m_write_step;
  uint16_t m_rlc_len;
  uint8_t * m_rlc_buf;
  uint8_t m_cur_rlc_len;
  uint8_t m_write1_byte;
  uint8_t m_write_len;
  uint8_t * m_write_buf;
#if defined (EEPROM_PROGRESS_BAR)
  uint8_t m_ratio;
#endif

public:

  void openRlc(uint8_t i_fileId);

  void create(uint8_t i_fileId, uint8_t typ, uint8_t sync_write);

  /// copy contents of i_fileSrc to i_fileDst
  bool copy(uint8_t i_fileDst, uint8_t i_fileSrc);

  inline bool isWriting()
  {
    return m_write_step != 0;
  }
  void write(uint8_t *buf, uint8_t i_len);
  void write1(uint8_t b);
  void nextWriteStep();
  void nextRlcWriteStep();
  void writeRlc(uint8_t i_fileId, uint8_t typ, uint8_t *buf, uint16_t i_len, uint8_t sync_write);

  // flush the current write operation if any
  void flush();

  // read from opened file and decode rlc-coded data
  uint16_t readRlc(uint8_t *buf, uint16_t i_len);

#if defined (EEPROM_PROGRESS_BAR)
  void DisplayProgressBar(uint8_t x);
#endif
};

extern RlcFile theFile;  //used for any file operation

inline void eeFlush()
{
  theFile.flush();
}

inline bool eepromIsWriting()
{
  return theFile.isWriting();
}

inline void eepromWriteProcess()
{
  theFile.nextWriteStep();
}

#if defined (EEPROM_PROGRESS_BAR)
#define DISPLAY_PROGRESS_BAR(x) theFile.DisplayProgressBar(x)
#else
#define DISPLAY_PROGRESS_BAR(x)
#endif

#define eeCopyModel(dst, src) theFile.copy(FILE_MODEL(dst), FILE_MODEL(src))
#define eeSwapModels(id1, id2) EFile::swap(FILE_MODEL(id1), FILE_MODEL(id2))
#define eeDeleteModel(idx) EFile::rm(FILE_MODEL(idx))

#if defined(SDCARD)
const pm_char * eeBackupModel(uint8_t i_fileSrc);
const pm_char * eeRestoreModel(uint8_t i_fileDst, char *model_name);
#endif

// For conversions

bool eepromOpen();
void eeLoadModelName(uint8_t id, char *name);
bool eeLoadGeneral();

// For EEPROM backup/restore

#endif
