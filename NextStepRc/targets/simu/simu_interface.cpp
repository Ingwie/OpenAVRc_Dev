/*
 *************************************************************
 *                      NEXTSTEPRC                           *
 *                                                           *
 *             -> Build your DIY MEGA 2560 TX                *
 *                                                           *
 *      Based on code named                                  *
 *      OpenTx - https://github.com/opentx/opentx            *
 *                                                           *
 *         Only avr code here for lisibility ;-)             *
 *                                                           *
 *  License GPLv2: http://www.gnu.org/licenses/gpl-2.0.html  *
 *                                                           *
 *************************************************************
 */

 #include "../../nextsteprc.h"


 void eepromReadBlock (uint8_t * pointer_ram, uint32_t pointer_eeprom, uint32_t size)
{
 /* assert(size);

  if (fp) {
    // TRACE("EEPROM read (pos=%d, size=%d)", pointer_eeprom, size);
    if (fseek(fp, (long)pointer_eeprom, SEEK_SET)==-1) perror("error in fseek");
    if (fread(pointer_ram, size, 1, fp) <= 0) perror("error in fread");
  }
  else {
    memcpy(pointer_ram, &eeprom[(uint64_t)pointer_eeprom], size);
  }*/
}

