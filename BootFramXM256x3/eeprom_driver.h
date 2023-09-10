/************************************************************************/
/* XMEGA EEPROM Driver                                                  */
/*                                                                      */
/* eeprom.h                                                             */
/*                                                                      */
/* Alex Forencich <alex@alexforencich.com>                              */
/*                                                                      */
/* Copyright (c) 2011 Alex Forencich                                    */
/*                                                                      */
/* Permission is hereby granted, free of charge, to any person          */
/* obtaining a copy of this software and associated documentation       */
/* files(the "Software"), to deal in the Software without restriction,  */
/* including without limitation the rights to use, copy, modify, merge, */
/* publish, distribute, sublicense, and/or sell copies of the Software, */
/* and to permit persons to whom the Software is furnished to do so,    */
/* subject to the following conditions:                                 */
/*                                                                      */
/* The above copyright notice and this permission notice shall be       */
/* included in all copies or substantial portions of the Software.      */
/*                                                                      */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,      */
/* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF   */
/* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND                */
/* NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS  */
/* BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN   */
/* ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN    */
/* CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE     */
/* SOFTWARE.                                                            */
/*                                                                      */
/************************************************************************/

#ifndef __EEPROM_DRIVER_H
#define __EEPROM_DRIVER_H


#include "xboot.h"

#include <avr/eeprom.h>



#ifndef EEPROM_PAGE_SIZE
#define EEPROM_PAGE_SIZE E2PAGESIZE
#endif

#ifdef __AVR_XMEGA__
// AVR1008 fix
//#define USE_AVR1008_EEPROM

#endif // __AVR_XMEGA__


#if defined(USE_FRAM_EE)
#define FRAM_TWI_ADDRESS  (0x50 << 1) // 0x50 with all strap closed on ZS042 module, EEPROM FM24W256, see datasheet.
// Prototypes
char fram_read_byte(unsigned int addr);
void fram_write_byte(unsigned int addr, char value);
void fram_read_block(uint8_t *destbuf, const uint16_t addr, uint16_t len);
void fram_write_block(uint8_t *srcbuf, const uint16_t addr, uint16_t len);

#define EEPROM_read_byte(addr)              fram_read_byte(addr)
#define EEPROM_write_byte(addr, value)      fram_write_byte(addr, value)
#define EEPROM_read_block(addr, dest, len)  fram_read_block((dest), (const uint16_t)(addr), (len))
#define EEPROM_write_block(addr, src, len)  fram_write_block((src), (const uint16_t)(addr), (len))
#else
#define EEPROM_read_byte(addr)              eeprom_read_byte((const uint8_t *)((uint16_t)(addr)))
#define EEPROM_write_byte(addr, value)      eeprom_write_byte((uint8_t *)((uint16_t)(addr)), (value))
#define EEPROM_read_block(addr, dest, len)  eeprom_read_block((dest), (const void *)((uint16_t)(addr)), (len))
#define EEPROM_write_block(addr, src, len)  eeprom_write_block((src), (void *)((uint16_t)(addr)), (len))
#endif

void EEPROM_erase_all(void);


#endif // __EEPROM_DRIVER_H
