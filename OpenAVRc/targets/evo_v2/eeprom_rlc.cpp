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

#if defined(CPUXMEGA)
#define NVM_EXEC()  _PROTECTED_WRITE(NVM.CTRLA, 0b1);
void EEPROM_WaitForNVM(void);
void EEPROM_FlushBuffer(void);
#endif

uint8_t   s_write_err = 0;    // error reasons
RlcFile   theFile;  //used for any file operation
EeFs      eeFs;

#if defined(EXTERNALEEPROM)
  blkid_t   freeBlocks = 0;
#endif

uint8_t  s_sync_write = false;

uint8_t * eeprom_buffer_data;
volatile uint8_t eeprom_buffer_size = 0;

#if !defined(EXTERNALEEPROM) || defined(SIMU)
#if defined(CPUM2560) || defined(SIMU)
uint16_t eeprom_pointer;

inline void eeprom_write_byte()
{
  EEAR = eeprom_pointer;
  EEDR = *eeprom_buffer_data;
  EECR |= 1<<EEMPE;
  EECR |= 1<<EEPE;
#if defined(SIMU) //Use virtual register as possible ;-)
  simu_eeprom[EEAR] = EEDR;
#endif
  eeprom_pointer++;
  eeprom_buffer_data++;
}

ISR(EE_READY_vect)
{
  if (--eeprom_buffer_size)
    {
      eeprom_write_byte();
    }
  else
    {
      TRACE("ISR 'EE_READY_vect' Cleared : Eeprom is saved");
      EECR &= ~(1<<EERIE);
    }
}

void eepromWriteBlock(uint8_t * i_pointer_ram, uint16_t i_pointer_eeprom, size_t size)
{
  ASSERT(!eeprom_buffer_size);

  eeprom_pointer = i_pointer_eeprom;
  eeprom_buffer_data = i_pointer_ram;
  eeprom_buffer_size = size+1;
  EECR |= (1<<EERIE);

  if (s_sync_write)
    {
      while (eeprom_buffer_size)
        MYWDT_RESET(EE_READY_vect()); //Simulate ISR in Simu mode, else reset watchdog
    }
}
#endif // defined(CPUM2560) || defined(SIMU)

#if defined(CPUXMEGA)
void eepromWriteBlock(uint8_t * ram_ptr, const uint16_t eeprom_addrs, size_t len)
{
  ASSERT(!eeprom_buffer_size);

  eeprom_buffer_size = len;
  uint8_t * eep_map_ptr = (uint8_t *) MAPPED_EEPROM_START;
  eep_map_ptr += eeprom_addrs;

  while(len) {

    EEPROM_FlushBuffer();
    NVM.CMD = NVM_CMD_LOAD_EEPROM_BUFFER_gc;

    // Load address registers.
    NVM.ADDR0 = (uint16_t) eep_map_ptr & 0x00FF;
    NVM.ADDR1 = ((uint16_t) eep_map_ptr & MAPPED_EEPROM_END) >> 8;
    NVM.ADDR2 = 0x00;

    // Load page buffer via EEPROM memory mapping *** ONE PAGE AT ONCE ***.
    while(len && (uint16_t) eep_map_ptr <= ( (uint16_t) eep_map_ptr | 0x1f) ) {
      *eep_map_ptr++ = *ram_ptr++;
      len--;
    }

    // EEPROM atomic write (erase & write).
    NVM.CMD = NVM_CMD_ERASE_WRITE_EEPROM_PAGE_gc; // 0x35
    NVM_EXEC();
    EEPROM_WaitForNVM(); // Wait before loading a new page.

    eeprom_buffer_size = len;
  }
}

void EEPROM_WaitForNVM(void)
{
  while (NVM.STATUS & NVM_NVMBUSY_bm) {
    MYWDT_RESET(EE_READY_vect();
  }
}

void EEPROM_FlushBuffer(void)
{
  /* Wait until NVM is not busy. */
  EEPROM_WaitForNVM();

  /* Flush EEPROM page buffer if necessary. */
  if (NVM.STATUS & NVM_EELOAD_bm) {
  NVM.CMD = NVM_CMD_ERASE_EEPROM_BUFFER_gc;
  NVM_EXEC();
  }
}
#endif // defined(CPUXMEGA)
#endif // !defined(EXTERNALEEPROM)

#if defined(EXTERNALEEPROM) // Use external FRAM EEPROM IIC Mode

void Ext_eeprom_read_block(uint8_t * pointer_ram, uint16_t pointer_eeprom, size_t size)
{
  i2c_start(ADDRESS_EXTERN_EEPROM+I2C_WRITE);     // set device address and write mode
  i2c_write((uint8_t)(pointer_eeprom >> 8));      //MSB write address
  i2c_write((uint8_t)pointer_eeprom);             //LSB write address
  i2c_start(ADDRESS_EXTERN_EEPROM+I2C_READ);      // set device address and write mode
  while (--size)                                  // more than one value to read
      {
        *pointer_ram++ = i2c_read_ack();          // read value from EEPROM
      }
  *pointer_ram = i2c_read_nack();                 // read last value
  i2c_stop();                                     // set stop conditon = release bus
}

void eepromWriteBlock(uint8_t * i_pointer_ram, uint16_t i_pointer_eeprom, size_t size) // Write I2C eeprom
{
  ASSERT(!eeprom_buffer_size);

  eeprom_buffer_data = i_pointer_ram;
  eeprom_buffer_size = size;

  i2c_start(ADDRESS_EXTERN_EEPROM+I2C_WRITE);     // set device address and write mode
  i2c_write((uint8_t)(i_pointer_eeprom >> 8)); //MSB write address
  i2c_write((uint8_t)i_pointer_eeprom); //LSB write address
  i2c_writeAndActiveISR(*eeprom_buffer_data);    // write value to EEPROM
  if (s_sync_write)
    {
      while (eeprom_buffer_size)
        MYWDT_RESET(); // Wait completion and reset watchdog
    }
}
#endif

static uint8_t EeFsRead(blkid_t blk, uint8_t ofs)
{
  uint8_t ret;
  EEPROMREADBLOCK(&ret, (uint16_t)(blk*BS+ofs+BLOCKS_OFFSET), 1);
  return ret;
}

static blkid_t EeFsGetLink(blkid_t blk)
{
#if defined(EXTERNALEEPROM)
  blkid_t ret;
  EEPROMREADBLOCK((uint8_t *)&ret, blk*BS+BLOCKS_OFFSET, sizeof(blkid_t));
  return ret;
#else
  return EeFsRead(blk, 0);
#endif
}

static void EeFsSetLink(blkid_t blk, blkid_t val)
{
  static blkid_t s_link; // we write asynchronously, then nothing on the stack!
  s_link = val;
  eepromWriteBlock((uint8_t *)&s_link, (blk*BS)+BLOCKS_OFFSET, sizeof(blkid_t));
}

static uint8_t EeFsGetDat(blkid_t blk, uint8_t ofs)
{
  return EeFsRead(blk, ofs+sizeof(blkid_t));
}

static void EeFsSetDat(blkid_t blk, uint8_t ofs, uint8_t *buf, uint8_t len)
{
  eepromWriteBlock(buf, (blk*BS)+ofs+sizeof(blkid_t)+BLOCKS_OFFSET, len);
}

static void EeFsFlushFreelist()
{
  eepromWriteBlock((uint8_t *)&eeFs.freeList, offsetof(EeFs, freeList), sizeof(eeFs.freeList));
}

static void EeFsFlushDirEnt(uint8_t i_fileId)
{
  eepromWriteBlock((uint8_t *)&eeFs.files[i_fileId], offsetof(EeFs, files) + sizeof(DirEnt)*i_fileId, sizeof(DirEnt));
}

static void EeFsFlush()
{
  eepromWriteBlock((uint8_t *)&eeFs, 0, sizeof(eeFs));
}

uint16_t EeFsGetFree()
{
#if defined(EXTERNALEEPROM)
  int32_t ret = freeBlocks * (BS-sizeof(blkid_t));
#else
  int16_t ret = 0;
  blkid_t i = eeFs.freeList;
  while (i)
    {
      ret += BS-sizeof(blkid_t);
      i = EeFsGetLink(i);
    }
#endif
  ret += eeFs.files[FILE_TMP].size;
  ret -= eeFs.files[FILE_MODEL(g_eeGeneral.currModel)].size;
  return (ret > 0 ? ret : 0);
}

/// free one or more blocks
static void EeFsFree(blkid_t blk)
{
  blkid_t i = blk;
  blkid_t tmp;

#if defined(EXTERNALEEPROM)
  freeBlocks++;
#endif

  while ((tmp=EeFsGetLink(i)))
    {
      i = tmp;
#if defined(EXTERNALEEPROM)
      freeBlocks++;
#endif
    }

  EeFsSetLink(i, eeFs.freeList);
  eeFs.freeList = blk; //chain in front
  EeFsFlushFreelist();
}

void eepromCheck()
{
  ENABLE_SYNC_WRITE(true);

  uint8_t *bufp = (uint8_t *)&g_model;
  memclear(bufp, BLOCKS);
  blkid_t blk ;

#if defined(EXTERNALEEPROM)
  blkid_t blocksCount;
#endif
  for (uint8_t i=0; i<=MAXFILES; i++)
    {
#if defined(EXTERNALEEPROM)
      blocksCount = 0;
#endif
      blkid_t *startP = (i==MAXFILES ? &eeFs.freeList : &eeFs.files[i].startBlk);
      blkid_t lastBlk = 0;
      blk = *startP;
      while (blk)
        {
          if (blk < FIRSTBLK || // bad blk index
              blk >= BLOCKS  || // bad blk indexchan
              bufp[blk])        // blk double usage
            {
              if (lastBlk)
                {
                  EeFsSetLink(lastBlk, 0);
                }
              else
                {
                  *startP = 0; // interrupt chain at startpos
                  EeFsFlush();
                }
              blk = 0; // abort
            }
          else
            {
#if defined(EXTERNALEEPROM)
              blocksCount++;
#endif
              bufp[blk] = i+1;
              lastBlk   = blk;
              blk       = EeFsGetLink(blk);
            }
        }
    }

#if defined(EXTERNALEEPROM)
  freeBlocks = blocksCount;
#endif
  for (blk=FIRSTBLK; blk<BLOCKS; blk++)
    {
      if (!bufp[blk])   // unused block
        {
#if defined(EXTERNALEEPROM)
          freeBlocks++;
#endif
          EeFsSetLink(blk, eeFs.freeList);
          eeFs.freeList = blk; // chain in front
          EeFsFlushFreelist();
        }
    }

  ENABLE_SYNC_WRITE(false);
}

void eepromFormat()
{
  ENABLE_SYNC_WRITE(true);

  uint8_t fil[4] = {0xFF,0xFF,0xFF,0xFF};

  for (uint16_t i=0; i<(EESIZE/DIM(fil)); i+=DIM(fil)) // erase eeprom
    {
      eepromWriteBlock((uint8_t*)&fil, i, DIM(fil));
    }

#if defined(SIMU)
  // write zero to the end of the new EEPROM file to set it's proper size
  uint8_t dummy = 0;
  eepromWriteBlock(&dummy, EESIZE-1, 1);
#endif

  memclear(&eeFs, sizeof(eeFs));
  eeFs.version  = EEFS_VERS;
  eeFs.mySize   = sizeof(eeFs);
  eeFs.freeList = 0;
  eeFs.bs       = BS;
  for (blkid_t i=FIRSTBLK; i<BLOCKS-1; i++)
    {
      EeFsSetLink(i, i+1);
    }
  EeFsSetLink(BLOCKS-1, 0);
  eeFs.freeList = FIRSTBLK;
#if defined(EXTERNALEEPROM)
  freeBlocks = BLOCKS;
#endif
  EeFsFlush();

  ENABLE_SYNC_WRITE(false);
}

bool eepromOpen()
{
  EEPROMREADBLOCK((uint8_t *)&eeFs, 0, sizeof(eeFs));

#if defined(SIMU)
  if (eeFs.version != EEFS_VERS)
    {
      TRACE("bad eeFs.version (%d instead of %d)", eeFs.version, EEFS_VERS);
    }
  if (eeFs.mySize != sizeof(eeFs))
    {
      TRACE("bad eeFs.mySize (%d instead of %d)", (int)eeFs.mySize, (int)sizeof(eeFs));
    }
#endif

  if (eeFs.version != EEFS_VERS || eeFs.mySize != sizeof(eeFs))
    {
      return false;
    }

  eepromCheck();
  return true;
}

bool EFile::exists(uint8_t i_fileId)
{
  return eeFs.files[i_fileId].startBlk;
}

/*
 * Swap two files in eeprom
 */
void EFile::swap(uint8_t i_fileId1, uint8_t i_fileId2)
{
  DirEnt            tmp = eeFs.files[i_fileId1];
  eeFs.files[i_fileId1] = eeFs.files[i_fileId2];
  eeFs.files[i_fileId2] = tmp;

  ENABLE_SYNC_WRITE(true);
  EeFsFlushDirEnt(i_fileId1);
  EeFsFlushDirEnt(i_fileId2);
  ENABLE_SYNC_WRITE(false);
}

void EFile::rm(uint8_t i_fileId)
{
  blkid_t i = eeFs.files[i_fileId].startBlk;
  memclear(&eeFs.files[i_fileId], sizeof(eeFs.files[i_fileId]));
  ENABLE_SYNC_WRITE(true);
  EeFsFlushDirEnt(i_fileId);
  if (i)
    EeFsFree(i); //chain in
  ENABLE_SYNC_WRITE(false);
}

/*
 * Open file i_fileId for reading.
 * Return the file's type
 */
void EFile::openRd(uint8_t i_fileId)
{
  m_fileId = i_fileId;
  m_pos      = 0;
  m_currBlk  = eeFs.files[m_fileId].startBlk;
  m_ofs      = 0;
  s_write_err = ERR_NONE;       // error reasons */
}

void RlcFile::openRlc(uint8_t i_fileId)
{
  EFile::openRd(i_fileId);
  m_zeroes   = 0;
  m_bRlc     = 0;
}

uint8_t EFile::read(uint8_t *buf, uint8_t i_len)
{
  uint16_t len = eeFs.files[m_fileId].size - m_pos;
  if (i_len > len)
    i_len = len;

  uint8_t remaining = i_len;
  while (remaining)
    {
      if (!m_currBlk)
        break;

      *buf++ = EeFsGetDat(m_currBlk, m_ofs++);
      if (m_ofs >= BS-sizeof(blkid_t))
        {
          m_ofs = 0;
          m_currBlk = EeFsGetLink(m_currBlk);
        }
      remaining--;
    }

  i_len -= remaining;
  m_pos += i_len;
  return i_len;
}

/*
 * Read runlength (RLE) compressed bytes into buf.
 */
uint16_t RlcFile::readRlc(uint8_t *buf, uint16_t i_len)
{
  uint16_t i = 0;
  for( ; 1; )
    {
      uint8_t ln = min<uint16_t>(m_zeroes, i_len-i);
      memclear(&buf[i], ln);
      i        += ln;
      m_zeroes -= ln;
      if (m_zeroes)
        break;

      ln = min<uint16_t>(m_bRlc, i_len-i);
      uint8_t lr = read(&buf[i], ln);
      i        += lr ;
      m_bRlc   -= lr;
      if(m_bRlc)
        break;

      if (read(&m_bRlc, 1) !=1)
        break; // read how many bytes to read

      ASSERT(m_bRlc & 0x7f);

      if (m_bRlc&0x80)   // if contains high byte
        {
          m_zeroes  =(m_bRlc>>4) & 0x7;
          m_bRlc    = m_bRlc & 0x0f;
        }
      else if(m_bRlc&0x40)
        {
          m_zeroes  = m_bRlc & 0x3f;
          m_bRlc    = 0;
        }
    }
  return i;
}

void RlcFile::write1(uint8_t b)
{
  m_write1_byte = b;
  write(&m_write1_byte, 1);
}

void RlcFile::write(uint8_t *buf, uint8_t i_len)
{
  m_write_len = i_len;
  m_write_buf = buf;

  do
    {
      nextWriteStep();
    }
  while (IS_SYNC_WRITE_ENABLE() && m_write_len && !s_write_err);
}

void RlcFile::nextWriteStep()
{
  if (!m_currBlk && m_pos==0)
    {
      eeFs.files[FILE_TMP].startBlk = m_currBlk = eeFs.freeList;
      if (m_currBlk)
        {
#if defined(EXTERNALEEPROM)
          freeBlocks--;
#endif
          eeFs.freeList = EeFsGetLink(m_currBlk);
          m_write_step |= WRITE_FIRST_LINK;
          EeFsFlushFreelist();
          return;
        }
    }

  if ((m_write_step & 0x0f) == WRITE_FIRST_LINK)
    {
      m_write_step -= WRITE_FIRST_LINK;
      EeFsSetLink(m_currBlk, 0);
      return;
    }

  while (m_write_len)
    {
      if (!m_currBlk)
        {
          s_write_err = ERR_FULL;
          break;
        }
      if (m_ofs >= (BS-sizeof(blkid_t)))
        {
          m_ofs = 0;
          blkid_t nextBlk = EeFsGetLink(m_currBlk);
          if (!nextBlk)
            {
              if (!eeFs.freeList)
                {
                  s_write_err = ERR_FULL;
                  break;
                }
              m_write_step += WRITE_NEXT_LINK_1;
              EeFsSetLink(m_currBlk, eeFs.freeList);
              // TODO not good
              return;
            }
          m_currBlk = nextBlk;
        }
      switch (m_write_step & 0x0f)
        {
        case WRITE_NEXT_LINK_1:
          m_currBlk = eeFs.freeList;
#if defined(EXTERNALEEPROM)
          freeBlocks--;
#endif
          eeFs.freeList = EeFsGetLink(eeFs.freeList);
          m_write_step += 1;
          EeFsFlushFreelist();
          return;
        case WRITE_NEXT_LINK_2:
          m_write_step -= WRITE_NEXT_LINK_2;
          EeFsSetLink(m_currBlk, 0);
          return;
        }
      uint8_t tmp = BS-sizeof(blkid_t)-m_ofs;
      if(tmp>m_write_len)
        tmp = m_write_len;
      m_write_buf += tmp;
      m_write_len -= tmp;
      m_ofs += tmp;
      m_pos += tmp;
      EeFsSetDat(m_currBlk, m_ofs-tmp, m_write_buf-tmp, tmp);
      return;
    }

  if (s_write_err == ERR_FULL)
    {
      POPUP_WARNING(STR_EEPROMOVERFLOW);
      m_write_step = 0;
      m_write_len = 0;
      m_cur_rlc_len = 0;
    }
  else if (!IS_SYNC_WRITE_ENABLE())
    {
      nextRlcWriteStep();
    }
}

void RlcFile::create(uint8_t i_fileId, uint8_t typ, uint8_t sync_write)
{
  // all write operations will be executed on FILE_TMP
  openRlc(FILE_TMP); // internal use
  eeFs.files[FILE_TMP].typ      = typ;
  eeFs.files[FILE_TMP].size     = 0;
  m_fileId = i_fileId;
  ENABLE_SYNC_WRITE(sync_write);
}

/*
 * Copy file src to dst
 */
bool RlcFile::copy(uint8_t i_fileDst, uint8_t i_fileSrc)
{
  EFile theFile2;
  theFile2.openRd(i_fileSrc);

  create(i_fileDst, FILE_TYP_MODEL/*optimization, only model files are copied. should be eeFs.files[i_fileSrc].typ*/, true);

  uint8_t buf[BS-sizeof(blkid_t)];
  uint8_t len;
  while ((len=theFile2.read(buf, sizeof(buf))))
    {
      write(buf, len);
      if (write_errno() != 0)
        {
          ENABLE_SYNC_WRITE(false);
          return false;
        }
    }

  blkid_t fri=0;
  if (m_currBlk && (fri=EeFsGetLink(m_currBlk)))
    EeFsSetLink(m_currBlk, 0);

  if (fri)
    EeFsFree(fri);  //chain in

  eeFs.files[FILE_TMP].size = m_pos;
  EFile::swap(m_fileId, FILE_TMP);

  ASSERT(!m_write_step);

  // s_sync_write is set to false in swap();
  return true;
}

#if defined(SDCARD)
uint8_t eeBackupModel(uint8_t i_fileSrc)
{
  uint8_t ret = 1;
  char *buf = reusableBuffer.modelsel.mainname;

  // we must close the logs as we reuse the same SD_file structure
  checkLogActived();

  // check and create folder if needed
  if (sdOpenCreateModelsDir())
    {

      eeLoadModelName(i_fileSrc, buf);
      buf[sizeof(g_model.name)] = '\0';

      uint8_t len = setSdModelName(buf, i_fileSrc);

      strcpy_P(&buf[len], STR_MODELS_EXT);

      TRACE("SD-card backup filename=%s", buf);

      /* Create the file */
      if (fat_create_file(SD_dir, buf, &SD_dir_entry))
        {

          SD_file = fat_open_file(SD_filesystem, &SD_dir_entry);

          EFile theFile2;
          theFile2.openRd(FILE_MODEL(i_fileSrc));

          *(uint32_t*)&buf[0] = OAVRC_FOURCC;
          buf[4] = g_eeGeneral.version;
          buf[5] = 'M';
          *(uint16_t*)&buf[6] = eeModelSize(i_fileSrc);

          /* write header to file */
          if(fat_write_file(SD_file, (uint8_t*)buf, 8) != 8)
            {
              fat_close_file(SD_file);
              ret = 0;
            }

          /* write datas to file */
          while ((len=theFile2.read((uint8_t *)buf, 15)))
            {
              if(fat_write_file(SD_file, (uint8_t*)buf, len) != len)
                {
                  fat_close_file(SD_file);
                  ret = 0;
                }
            }
        }  else { ret = 0; }
    } else { ret = 0; }

  fat_close_file(SD_file);
  return ret;
}

const pm_char * eeRestoreModel(uint8_t i_fileDst, char *model_name)
{
  char *buf = reusableBuffer.modelsel.mainname;

  strcpy(buf, model_name);
  strcpy_P(&buf[strlen(buf)], STR_MODELS_EXT);

  // MODELS_PATH Is already the current DIR by listSdFiles function
  if (sdFindFileStruct(buf))
    {
      SD_file = fat_open_file(SD_filesystem, &SD_dir_entry);
    }

  /* read the file header */
  if (fat_read_file(SD_file, (uint8_t*)buf, 8) != 8)
    {
      return STR_SDCARD_ERROR;
    }

  uint8_t version = (uint8_t)buf[4];
  if (*(uint32_t*)&buf[0] != OAVRC_FOURCC || version != EEPROM_VER || buf[5] != 'M')
    {
      return STR_INCOMPATIBLE;
    }

  if (eeModelExists(i_fileDst))
    {
      eeDeleteModel(i_fileDst);
    }

  theFile.create(FILE_MODEL(i_fileDst), FILE_TYP_MODEL, true);

  int8_t read;
  do
    {
      read = fat_read_file(SD_file, (uint8_t*)buf, 15);
      if (read == -1)
        {
          ENABLE_SYNC_WRITE(false);
          return STR_SDCARD_ERROR;
        }
      if (read > 0)
        {
          theFile.write((uint8_t *)buf, read);
          if (write_errno() != 0)
            {
              ENABLE_SYNC_WRITE(false);
              fat_close_file(SD_file);
              return STR_EEPROMOVERFLOW;
            }
        }
    }
  while (read == 15);

  blkid_t fri=0;
  if (theFile.m_currBlk && (fri=EeFsGetLink(theFile.m_currBlk)))
    EeFsSetLink(theFile.m_currBlk, 0);

  if (fri)
    EeFsFree(fri);  //chain in

  eeFs.files[FILE_TMP].size = theFile.m_pos;
  EFile::swap(theFile.m_fileId, FILE_TMP); // s_sync_write is set to false in swap();

  return NULL;
}
#endif

void RlcFile::writeRlc(uint8_t i_fileId, uint8_t typ, uint8_t *buf, uint16_t i_len, uint8_t sync_write)
{
  create(i_fileId, typ, sync_write);

  m_write_step = WRITE_START_STEP;
  m_rlc_buf = buf;
  m_rlc_len = i_len;
  m_cur_rlc_len = 0;
#if defined (EEPROM_PROGRESS_BAR)
  m_ratio = (typ == FILE_TYP_MODEL ? 100 : 10);
#endif

  do
    {
      nextRlcWriteStep();
    }
  while (IS_SYNC_WRITE_ENABLE() && m_write_step && !s_write_err);
}

void RlcFile::nextRlcWriteStep()
{
  uint8_t cnt    = 1;
  uint8_t cnt0   = 0;
  uint16_t i = 0;

  if (m_cur_rlc_len)
    {
      uint8_t tmp1 = m_cur_rlc_len;
      uint8_t *tmp2 = m_rlc_buf;
      m_rlc_buf += m_cur_rlc_len;
      m_cur_rlc_len = 0;
      write(tmp2, tmp1);
      return;
    }

  bool run0 = (m_rlc_buf[0] == 0);

  if (m_rlc_len==0)
    goto close;

  for (i=1; 1; i++)   // !! laeuft ein byte zu weit !!
    {
      bool cur0 = m_rlc_buf[i] == 0;
      if (cur0 != run0 || cnt==0x3f || (cnt0 && cnt==0x0f) || i==m_rlc_len)
        {
          if (run0)
            {
              ASSERT(cnt0==0);
              if (cnt<8 && i!=m_rlc_len)
                cnt0 = cnt; //aufbew fuer spaeter
              else
                {
                  m_rlc_buf+=cnt;
                  m_rlc_len-=cnt;
                  write1(cnt|0x40);
                  return;
                }
            }
          else
            {
              m_rlc_buf+=cnt0;
              m_rlc_len-=cnt0+cnt;
              m_cur_rlc_len=cnt;
              if(cnt0)
                {
                  write1(0x80 | (cnt0<<4) | cnt);
                }
              else
                {
                  write1(cnt);
                }
              return;
            }
          cnt=0;
          if (i==m_rlc_len)
            break;
          run0 = cur0;
        }
      cnt++;
    }

close:

  switch(m_write_step)
    {
    case WRITE_START_STEP:
    {
      blkid_t fri = 0;

      if (m_currBlk && (fri = EeFsGetLink(m_currBlk)))
        {
          // TODO reuse EeFsFree!!!
          blkid_t prev_freeList = eeFs.freeList;
          eeFs.freeList = fri;
#if defined(EXTERNALEEPROM)
          freeBlocks++;
#endif
          while (EeFsGetLink(fri))
            {
              fri = EeFsGetLink(fri);
#if defined(EXTERNALEEPROM)
              freeBlocks++;
#endif
            }
          m_write_step = WRITE_FREE_UNUSED_BLOCKS_STEP1;
          EeFsSetLink(fri, prev_freeList);
          return;
        }
    }

    case WRITE_FINAL_DIRENT_STEP:
    {
      m_currBlk = eeFs.files[FILE_TMP].startBlk;
      DirEnt & f = eeFs.files[m_fileId];
      eeFs.files[FILE_TMP].startBlk = f.startBlk;
      eeFs.files[FILE_TMP].size = f.size;
      f.startBlk = m_currBlk;
      f.size = m_pos;
      f.typ = eeFs.files[FILE_TMP].typ;
      m_write_step = WRITE_TMP_DIRENT_STEP;
      EeFsFlushDirEnt(m_fileId);
      return;
    }

    case WRITE_TMP_DIRENT_STEP:
      m_write_step = 0;
      EeFsFlushDirEnt(FILE_TMP);
      return;

    case WRITE_FREE_UNUSED_BLOCKS_STEP1:
      m_write_step = WRITE_FREE_UNUSED_BLOCKS_STEP2;
      EeFsSetLink(m_currBlk, 0);
      return;

    case WRITE_FREE_UNUSED_BLOCKS_STEP2:
      m_write_step = WRITE_FINAL_DIRENT_STEP;
      EeFsFlushFreelist();
      return;
    }
}

void RlcFile::flush()
{
  while (eeprom_buffer_size)
    MYWDT_RESET(EE_READY_vect()); //Simulate ISR in Simu mode

  ENABLE_SYNC_WRITE(true);

  while (m_write_len && !s_write_err)
    nextWriteStep();

  while (isWriting() && !s_write_err)
    nextRlcWriteStep();

  ENABLE_SYNC_WRITE(false);
}

#if defined (EEPROM_PROGRESS_BAR)
void RlcFile::DisplayProgressBar(uint8_t x)
{
  if (s_eeDirtyMsk || isWriting() || eeprom_buffer_size)
    {
      uint8_t len = s_eeDirtyMsk ? 1 : limit<uint8_t>(1, (7 - (m_rlc_len/m_ratio)), 7);
      lcdDrawFilledRect(x+1, 0, 5, FH, SOLID, ERASE);
      lcdDrawFilledRect(x+2, 7-len, 3, len);
    }
}
#endif

// For conversions ...

bool eeLoadGeneral()
{
  theFile.openRlc(FILE_GENERAL);
  if (theFile.readRlc((uint8_t*)&g_eeGeneral, 1) == 1 && g_eeGeneral.version == EEPROM_VER)
    {
      theFile.openRlc(FILE_GENERAL);
      if (theFile.readRlc((uint8_t*)&g_eeGeneral, sizeof(g_eeGeneral)) <= sizeof(EEGeneral))
        {
          return true;
        }
    }

  TRACE("EEPROM version %d instead of %d", g_eeGeneral.version, EEPROM_VER);
  return false;
}

void eeLoadModelName(uint8_t id, char *name)
{
  memclear(name, sizeof(g_model.name));
  if (id < MAX_MODELS)
    {
      theFile.openRlc(FILE_MODEL(id));
      theFile.readRlc((uint8_t*)name, sizeof(g_model.name));
    }
}

bool eeModelExists(uint8_t id)
{
  return EFile::exists(FILE_MODEL(id));
}

void eeLoadModel(uint8_t id)
{
  if (id<MAX_MODELS)
    {


#if defined(SDCARD)
      checkLogActived();
#endif

      if (pulsesStarted())
        {
          sendStopPulses();
        }

      theFile.openRlc(FILE_MODEL(id));
      uint16_t sz = theFile.readRlc((uint8_t*)&g_model, sizeof(g_model));

      bool newModel = false;

      if (sz < 256)
        {
          modelDefault(id);
          eeCheck(true);
          newModel = true;
        }

      AUDIO_FLUSH();
      flightReset();
      logicalSwitchesReset();
      setGazSource();

      if (pulsesStarted())
        {
#if defined(GUI)
          if (!newModel)
            {
              checkAll();
            }
#endif
          startPulses(PROTOCMD_INIT);
        }

      customFunctionsReset();

      restoreTimers();

      // TODO pulses should be started after mixer calculations ...

    }
}

void eeErase(bool warn)
{
  generalDefault();

  if (warn)
    {
      ALERT(STR_EEPROMWARN, STR_BADEEPROMDATA, AU_BAD_EEPROM);
    }

  MESSAGE(STR_EEPROMWARN, STR_EEPROMFORMATTING, NULL, AU_EEPROM_FORMATTING);
  eepromFormat();
  theFile.writeRlc(FILE_GENERAL, FILE_TYP_GENERAL, (uint8_t*)&g_eeGeneral, sizeof(EEGeneral), true);
  modelDefault(0);
  theFile.writeRlc(FILE_MODEL(0), FILE_TYP_MODEL, (uint8_t*)&g_model, sizeof(g_model), true);
}

void eeCheck(bool immediately)
{
  if (immediately)
    {
      eeFlush();
    }

  if (s_eeDirtyMsk & EE_GENERAL)
    {
      TRACE("eeprom write general");
      s_eeDirtyMsk -= EE_GENERAL;
      theFile.writeRlc(FILE_GENERAL, FILE_TYP_GENERAL, (uint8_t*)&g_eeGeneral, sizeof(EEGeneral), immediately);
      if (!immediately)
        return;
    }

  if (s_eeDirtyMsk & EE_MODEL)
    {
      TRACE("eeprom write model");
      s_eeDirtyMsk = 0;
      theFile.writeRlc(FILE_MODEL(g_eeGeneral.currModel), FILE_TYP_MODEL, (uint8_t*)&g_model, sizeof(g_model), immediately);
    }
}
