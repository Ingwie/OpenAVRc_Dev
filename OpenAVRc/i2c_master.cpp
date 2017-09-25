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

void i2c_init(void)
{
// 1MHz
  TWBR = (uint8_t) 0;
  TWSR |= 0b00; // Prescaler.
  TWSR &= 0b11111100;
}

inline void wait()
{
  // Poll TWI Interrupt flag.
  while(! (TWCR & (1<<TWINT)) );
}

uint8_t i2c_start(uint8_t address)
{
  // reset TWI control register
  TWCR = 0;
  // transmit START condition
  TWCR = (1<<TWINT) | (1<<TWSTA) | (1<<TWEN);
  // wait for end of transmission
  wait();

  // check if the start condition was successfully transmitted
  if((TWSR & 0xF8) != TW_START) {
    return 1;
  }

  // load slave address into data register
  TWDR = address;
  // start transmission of address
  TWCR = (1<<TWINT) | (1<<TWEN);
  // wait for end of transmission
  wait();
  // check if the device has acknowledged the READ / WRITE mode
  uint8_t twst = TW_STATUS & 0xF8;
  if ( (twst != TW_MT_SLA_ACK) && (twst != TW_MR_SLA_ACK) ) return 1;

  return 0;
}

uint8_t i2c_write(uint8_t data)
{
  // load data into data register
  TWDR = data;
  // start transmission of data
  TWCR = (1<<TWINT) | (1<<TWEN);
  // wait for end of transmission
  wait();

  if( (TWSR & 0xF8) != TW_MT_DATA_ACK ) {
    return 1;
  }

  return 0;
}

inline void i2c_writeISR(uint8_t data)
{
  // load data into data register
  TWDR = data;
  // start transmission of data with isr activated
  TWCR = ((1<<TWINT) | (1<<TWEN) | (1 << TWIE));
  // Send data and enable IRQ
}


uint8_t i2c_read_ack(void)
{

  // start TWI module and acknowledge data after reception
  TWCR = (1<<TWINT) | (1<<TWEN) | (1<<TWEA);
  // wait for end of transmission
  wait();
  // return received data from TWDR
  return TWDR;
}

uint8_t i2c_read_nack(void)
{

  // start receiving without acknowledging reception
  TWCR = (1<<TWINT) | (1<<TWEN);
  // wait for end of transmission
  wait();
  // return received data from TWDR
  return TWDR;
}

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
  if (i2c_start(devaddr | 0x00)) return 1;

  i2c_write(regaddr);

  for (uint16_t i = 0; i < length; i++) {
    if (i2c_write(data[i])) return 1;
  }

  i2c_stop();

  return 0;
}

uint8_t i2c_readReg(uint8_t devaddr, uint8_t regaddr, uint8_t* data, uint16_t length)
{
  if (i2c_start(devaddr)) return 1;

  i2c_write(regaddr);

  if (i2c_start(devaddr | 0x01)) return 1;

  for (uint16_t i = 0; i < (length-1); i++) {
    data[i] = i2c_read_ack();
  }
  data[(length-1)] = i2c_read_nack();

  i2c_stop();

  return 0;
}

void i2c_stop(void)
{
  // transmit STOP condition
  TWCR = (1<<TWINT) | (1<<TWEN) | (1<<TWSTO);
}

uint8_t iic_read (
  uint8_t dev,    /* Device address */
  uint8_t adr,    /* Read start address */
  uint16_t cnt,   /* Read uint8_t count */
  uint8_t *buff   /* Read data buffer */
)
{
  uint8_t ret;
  ret = i2c_readReg(dev, adr, buff, cnt);
  return (!ret);
}

uint8_t iic_write (
  uint8_t dev,      /* Device address */
  uint8_t adr,      /* Write start address */
  uint16_t cnt,     /* Write uint8_t count */
  const uint8_t *buff /* Data to be written */
)
{
  uint8_t ret;
  ret = i2c_writeReg(dev, adr, (uint8_t *)buff, cnt);
  return (!ret);
}

