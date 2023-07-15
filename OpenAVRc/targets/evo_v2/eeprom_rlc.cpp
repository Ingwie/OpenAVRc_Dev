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

