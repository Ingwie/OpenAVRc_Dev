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
#include "spi.h"
//---------------------------
// AVR SPI functions
//---------------------------

#if defined(CPUM2560)

void master_spi_disable()
{
  SPCR &= ~(1<<SPE);
}

//----------------------

void enable_spi_master_mode()
{
// Enable SPI as Master, MSB first.
  SPI_250K(); // Fosc/64
// Note : Make sure Slave Select pin is output or input pullup.
}

//----------------------

uint8_t master_spi_xfer(uint8_t value)
{
  // Full Duplex (4 wire) spi
  SPDR = value;
  /* Wait for transfer to complete */
#if !defined(SIMU)
  while (!(SPSR & (1<<SPIF)));
#endif
  return SPDR;
}

#endif



#if defined(CPUXMEGA)

void master_spi_disable(void)
{
  SDCARD_SPI.CTRL &= ~SPI_ENABLE_bm;
}


void enable_spi_master_mode(void)
{
  // Enable SPI as Master, MSB first, SPI Mode 0.
  SDCARD_SPI.CTRL = SPI_ENABLE_bm | SPI_MASTER_bm | (0b00<<SPI_MODE_gp);
  SDCARD_SPI.CTRL &= ~SPI_DORD_bm;
  // Note : Make sure Slave Select pin is output or input pullup.
  SPI_250K();
  // Set SPI_IF Flag for first time.
  SDCARD_SPI.DATA =0;
}


uint8_t master_spi_xfer(uint8_t value)
{
  // Full Duplex (4 wire) SPI.
  SDCARD_SPI.DATA = value;
  // Wait for transfer to complete.
#if !defined(SIMU)
  while (! (SDCARD_SPI.STATUS & SPI_IF_bm));
#endif
  return SDCARD_SPI.DATA;
}

#endif
