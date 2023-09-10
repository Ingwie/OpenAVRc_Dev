/************************************************************************/
/* XMEGA EEPROM Driver                                                  */
/*                                                                      */
/* eeprom.c                                                             */
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

#include "eeprom_driver.h"
#include "string.h"
#include "i2c_master.h"

#ifdef __AVR_XMEGA__

#ifdef USE_AVR1008_EEPROM
#include <avr/sleep.h>
#endif // USE_AVR1008_EEPROM

#else
#include <avr/boot.h>
#include <avr/io.h>
#endif // __AVR_XMEGA__


#ifdef __AVR_XMEGA__

// NVM call
static inline void NVM_EXEC(void)
{
  void *z = (void *)&NVM_CTRLA;

  __asm__ volatile("out %[ccp], %[ioreg]" "\n\t"
      "st z, %[cmdex]"
      :
      : [ccp] "I" (_SFR_IO_ADDR(CCP)),
      [ioreg] "d" (CCP_IOREG_gc),
      [cmdex] "r" (NVM_CMDEX_bm),
      [z] "z" (z)
  );
}

#ifdef USE_AVR1008_EEPROM

// Interrupt handler for the EEPROM write "done" interrupt
ISR(NVM_EE_vect)
{
  // Disable the EEPROM interrupt
  NVM.INTCTRL = (NVM.INTCTRL & ~NVM_EELVL_gm);
}

// AVR1008 fix
static inline void NVM_EXEC_WRAPPER(void)
{
  // Save the Sleep register
  uint8_t sleepCtr = SLEEP.CTRL;
  // Set sleep mode to IDLE
  SLEEP.CTRL = (SLEEP.CTRL & ~SLEEP.CTRL) | SLEEP_SMODE_IDLE_gc;
  // Save the PMIC Status and control registers
  uint8_t statusStore = PMIC.STATUS;
  uint8_t pmicStore = PMIC.CTRL;

  // Enable only the highest level of interrupts
  PMIC.CTRL = (PMIC.CTRL & ~(PMIC_MEDLVLEN_bm | PMIC_LOLVLEN_bm)) | PMIC_HILVLEN_bm;
  // Save SREG for later use
  uint8_t globalInt = SREG;
  // Enable global interrupts
  sei();
  // Set sleep enabled
  SLEEP.CTRL |= SLEEP_SEN_bm;
  // Save eeprom interrupt settings for later
  uint8_t eepromintStore = NVM.INTCTRL;
  NVM_EXEC();
  // Enable EEPROM interrupt
  NVM.INTCTRL = NVM_EELVL0_bm | NVM_EELVL1_bm;
  // Sleep before 2.5uS has passed
  sleep_cpu();
  // Restore sleep settings
  SLEEP.CTRL = sleepCtr;
  // Restore PMIC status and control registers
  PMIC.STATUS = statusStore;
  PMIC.CTRL = pmicStore;
  // Restore EEPROM interruptsettings
  NVM.INTCTRL = eepromintStore;
  // Restore global interrupt settings
  SREG = globalInt;
}

#else

#define NVM_EXEC_WRAPPER NVM_EXEC

#endif // USE_AVR1008_EEPROM


void wait_for_nvm(void)
{
  while (NVM.STATUS & NVM_NVMBUSY_bm) {};
}

void EEPROM_erase_all(void)
{
  wait_for_nvm();

  NVM.CMD = NVM_CMD_ERASE_EEPROM_gc;
  NVM_EXEC_WRAPPER();
}

#else // __AVR_XMEGA__

void EEPROM_erase_all(void) {
  uint8_t hfuse = boot_lock_fuse_bits_get(GET_HIGH_FUSE_BITS);
  if ((hfuse & (1 << 3)) != 0) {
    for (uint16_t i = 0; i < E2END; i++) {
      eeprom_update_byte((uint8_t *) i, 0xff);
    }
  }
}
#endif // __AVR_XMEGA__


#if defined(USE_FRAM_EE)

void fram_write_byte(const unsigned int addr, char value) {
  i2c_start(FRAM_TWI_ADDRESS + I2C_WRITE); // Set device address and write mode.
  i2c_write((addr & 0xFF00) >> 8); // MSB write address
  i2c_write(addr & 0x00FF); // LSB write address
  i2c_write(value); // Write value to FRAM.
  i2c_stop(); // Send stop condition = release bus
}

char fram_read_byte(const unsigned int addr) {
  i2c_start(FRAM_TWI_ADDRESS + I2C_WRITE);
  i2c_write((addr & 0xFF00) >> 8);
  i2c_write(addr & 0x00FF);
  i2c_start(FRAM_TWI_ADDRESS + I2C_READ); // Set device address and read mode.
  char temp = i2c_read_nack(); // Read last value from FRAM.
  i2c_stop();
  return temp;
}

void fram_write_block(unsigned char *srcbuf, const uint16_t fram_addr, unsigned int len) {
  i2c_start(FRAM_TWI_ADDRESS + I2C_WRITE); // Set device address and write mode.

//  uint16_t fram_addr =  *((uint16_t *)addr);

  i2c_write( (fram_addr & 0xFF00) >> 8); // MSB write address
  i2c_write(  fram_addr & 0x00FF      ); // LSB write address
  while (len--) {
    i2c_write(*srcbuf++); // Write value to FRAM.
  }
  i2c_stop(); // Send stop condition = release bus
}

void fram_read_block(unsigned char *destbuf, const uint16_t fram_addr, unsigned int len) {
  i2c_start(FRAM_TWI_ADDRESS + I2C_WRITE);

//  uint16_t fram_addr =  *((uint16_t *)addr);

  i2c_write( (fram_addr & 0xFF00) >> 8);
  i2c_write(  fram_addr & 0x00FF      );
  i2c_start(FRAM_TWI_ADDRESS + I2C_READ); // Set device address and read mode.
  do {
    *destbuf++ = i2c_read_ack(); // Read value from FRAM.
    len--;
  } while (len > 1);
  *destbuf = i2c_read_nack(); // Read last value from FRAM.
  i2c_stop();
}
#endif // FRAM_EE

