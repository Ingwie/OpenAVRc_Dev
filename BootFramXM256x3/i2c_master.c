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


// From https://github.com/g4lvanix/I2C-master-lib

#include "xboot.h"
#include "i2c_master.h"

#include <util/twi.h>



#if !defined (__AVR_XMEGA__)

void InitPin()
{
  DDRD &= 0b11111100;  PORTD |= 0b00000011; // 1:I2C_SDA, 0:I2C_SCL pull_up on.
}


void wait()
{
  while(! (TWCR & (1<<TWINT)) );
}


void i2c_init(void)
{
  InitPin();
  TWBR = (uint8_t)TWBR_val;
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


void i2c_stop(void)
{
  // transmit STOP condition
  TWCR = (1<<TWINT) | (1<<TWEN) | (1<<TWSTO);
}
#endif


#if defined (__AVR_XMEGA__)

void InitPin()
{
  // Using external Pullups.
}

void i2c_init(void)
{
  InitPin();
  /*
  This function needs to be called only once to set up the correct SCL frequency
  for the bus.
  */
  FRAM_RTC_TWI.CTRL = 0;
  FRAM_RTC_TWI.MASTER.BAUD = (uint8_t) BAUD_val;
  FRAM_RTC_TWI.MASTER.CTRLA = TWI_MASTER_ENABLE_bm;

  FRAM_RTC_TWI.MASTER.STATUS = TWI_MASTER_BUSSTATE_IDLE_gc;
}


uint8_t i2c_start(uint8_t address)
{
/*
This function needs to be called any time a connection to a new slave device should
be established. The function returns 1 if an error has occurred, otherwise it returns 0.
*/
  FRAM_RTC_TWI.MASTER.ADDR = address;

  while(! (FRAM_RTC_TWI.MASTER.STATUS & TWI_MASTER_CLKHOLD_bm)); // CLKHOLD flag is (RIF | WIF).

  // Check if the device has acknowledged the READ / WRITE mode
  if(FRAM_RTC_TWI.MASTER.STATUS & TWI_MASTER_RXACK_bm) return 1; //ToDo bus error and fault.
  return 0;
}


uint8_t i2c_write(uint8_t data)
{
/*
This function is used to write data to the currently active device.
The only parameter this function takes is the 8 bit unsigned integer to be sent.
The function returns 1 if an error has occurred, otherwise it returns 0.
*/
  // load data into data register
  FRAM_RTC_TWI.MASTER.DATA = data;

  while(! (FRAM_RTC_TWI.MASTER.STATUS & TWI_MASTER_WIF_bm));

  // Check if the device has acknowledged the READ / WRITE mode
  if(FRAM_RTC_TWI.MASTER.STATUS & TWI_MASTER_RXACK_bm) return 1; //ToDo bus error and fault.
  return 0;
}


uint8_t i2c_read_ack(void)
{
/*
This function is used to read one byte from a device and request another byte of data
after the transmission is complete by sending the acknowledge bit.
This function returns the received byte.
*/

  while(! (FRAM_RTC_TWI.MASTER.STATUS & TWI_MASTER_RIF_bm));

  uint8_t data = FRAM_RTC_TWI.MASTER.DATA;

  FRAM_RTC_TWI.MASTER.CTRLC = (~TWI_MASTER_ACKACT_bm) & TWI_MASTER_CMD_RECVTRANS_gc; // Send ACK - smart mode enabled.

  return data;
}


uint8_t i2c_read_nack(void)
{
/*
This function is used to read one byte from a device an then not requesting another
byte and therefore stopping the current transmission.
This function returns the received byte.
*/

  while(! (FRAM_RTC_TWI.MASTER.STATUS & TWI_MASTER_RIF_bm));

  uint8_t data = FRAM_RTC_TWI.MASTER.DATA;

  FRAM_RTC_TWI.MASTER.CTRLC = TWI_MASTER_ACKACT_bm; // Send NACK and no new command.
  return data;
}


void i2c_stop(void)
{
  // Transmit STOP condition.
  // Send STOP command, preserve ACKACT bit.

  FRAM_RTC_TWI.MASTER.CTRLC |= TWI_MASTER_CMD_STOP_gc;
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

