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

void spi_disable(void)
{
  SPCR &= ~(1<<SPE);
}

//----------------------

void spi_enable_master_mode(void)
{
// Enable SPI as Master, MSB first.
SPCR = (1<<SPE) | (1<<MSTR) | (0<<DORD);
// Set clock rate Fosc/2
//SPSR |= (1<<SPI2X); // 8 Mhz on M2560
SPSR |= (0<<SPI2X); // 4 Mhz on M2560
// Note : Make sure Slave Select pin is output or input pullup.
}

//----------------------

uint8_t spi_xfer(uint8_t value)
{
  // Full Duplex (4 wire) spi
  SPDR = value;
  /* Wait for transfer to complete */
#if !defined(SIMU)
  while (!(SPSR & (1<<SPIF)));
#endif
  return SPDR;
}

//----------------------
