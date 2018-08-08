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


// From https://github.com/g4lvanix/I2C-master-lib

#if !defined(SIMU)
#include <avr/io.h>
#include <util/twi.h>
#endif

#include "i2c_master.h"

// DS3231M = 400KHz
// DS3231  = 400KHz
// FM24W256 = 1MHz
// 400KHz SCL ... TWPS =0b01 in register TWSR(1:0), TWBR =3.
// 1MHz SCL ... TWPS =0b00 in register TWSR(1:0), TWBR =0.
/*
 *        FCPU
 * --------------------  = SCL frequency
 * 16 + 2(TWBR) .4^TWPS
 */

#if defined(CPUM2560)
void i2c_init()
{
  I2C_SPEED_888K();
}

void i2c_Wait()
{
  // Poll TWI Interrupt flag.
  while(! (TWCR & (1<<TWINT)) );
}

uint8_t i2c_start(uint8_t address)
{
  // transmit START condition
  TWCR = (1<<TWINT) | (1<<TWSTA) | (1<<TWEN);
  // i2c_Wait for end of transmission
  i2c_Wait();

  // check if the start condition was successfully transmitted
  uint8_t twst = TW_STATUS & 0xF8;
  if(! ((twst == TW_START) || (twst == TW_REP_START)) ) {
    i2c_stop();
    return 1;
  }

  // load slave address into data register
  TWDR = address;
  // start transmission of address
  TWCR = (1<<TWINT) | (1<<TWEN);
  // i2c_Wait for end of transmission
  i2c_Wait();
  // check if the device has acknowledged the READ / WRITE mode
  twst = TW_STATUS & 0xF8;
  if ( (twst == TW_MT_SLA_ACK) || (twst == TW_MR_SLA_ACK) ) return 0;
  else {
      i2c_stop();
      return 1;
  }
}

uint8_t i2c_write(uint8_t data)
{
  // load data into data register
  TWDR = data;
  // start transmission of data
  TWCR = (1<<TWINT) | (1<<TWEN);
  // i2c_Wait for end of transmission
  i2c_Wait();

  if( (TWSR & 0xF8) != TW_MT_DATA_ACK ) {
    return 1;
  }
  return 0;
}

uint8_t i2c_read_ack()
{
  // start TWI module and acknowledge data after reception
  TWCR = (1<<TWINT) | (1<<TWEN) | (1<<TWEA);
  // i2c_Wait for end of transmission
  i2c_Wait();
  // return received data from TWDR
  return TWDR;
}

uint8_t i2c_read_nack()
{
  // start receiving without acknowledging reception
  TWCR = (1<<TWINT) | (1<<TWEN);
  // i2c_Wait for end of transmission
  i2c_Wait();
  // return received data from TWDR
  return TWDR;
}

void i2c_stop()
{
  // transmit STOP condition
  TWCR = (1<<TWINT) | (1<<TWSTO) | (1<<TWEN);
  while SIMU_UNLOCK_MACRO(TWCR & (1<<TWSTO));
}

inline void i2c_writeISR(uint8_t data)
{
  // load data into data register
  TWDR = data;
  // start transmission of data with isr activated
  TWCR = (1<<TWINT) | (1<<TWEN) | (1<<TWIE);
  // Send data and enable IRQ
}

extern uint8_t * eeprom_buffer_data;
extern volatile uint8_t eeprom_buffer_size;
ISR(TWI_vect)
{
  if (--eeprom_buffer_size) {
    i2c_writeISR(*eeprom_buffer_data);
    ++eeprom_buffer_data;
  } else {
    i2c_stop(); // This resets the TWINT Flag.
    TWCR &= ~(1<<TWIE); // Disable TWI interrupts.
  }
}
#endif


uint8_t i2c_transmit(uint8_t address, uint8_t* data, uint16_t length)
{
  if (i2c_start(address | I2C_WRITE)) return 1;

  for (uint16_t i = 0; i < length; i++) {
    if (i2c_write(data[i])) return 1;
  }

  i2c_stop();

  return 0;
}

uint8_t i2c_receive(uint8_t address, uint8_t* data, uint16_t length)
{
  if (i2c_start(address | I2C_READ)) return 1;

  for (uint16_t i = 0; i < (length-1); i++) {
    data[i] = i2c_read_ack();
  }
  data[(length-1)] = i2c_read_nack();

  i2c_stop();

  return 0;
}

uint8_t i2c_writeReg(uint8_t devaddr, uint8_t regaddr, uint8_t* data, uint16_t length)
{
  if (i2c_start(devaddr | I2C_WRITE)) return 1;

  i2c_write(regaddr);

  for (uint16_t i = 0; i < length; i++) {
    if (i2c_write(data[i])) return 1;
  }

  i2c_stop();

  return 0;
}

uint8_t i2c_readReg(uint8_t devaddr, uint8_t regaddr, uint8_t* data, uint16_t length)
{
  if (i2c_start(devaddr | I2C_WRITE)) return 1;

  i2c_write(regaddr);

  if (i2c_start(devaddr | I2C_READ)) return 1;

  for (uint16_t i = 0; i < (length-1); i++) {
    data[i] = i2c_read_ack();
  }
  data[(length-1)] = i2c_read_nack();

  i2c_stop();

  return 0;
}
