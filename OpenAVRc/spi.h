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


#ifndef _SPI_H_
#define _SPI_H_

void spi_disable(void);
void spi_enable_master_mode(void);
uint8_t spi_xfer(uint8_t value);

#if defined(CPUM2560)
// SPI Speeds.
#define SPI_250K() { SPSR &= ~(1<<SPI2X); SPCR = (SPCR | 0b10) & 0b11111110; }
#define SPI_500K() { SPSR |=  (1<<SPI2X); SPCR = (SPCR | 0b10) & 0b11111110; }
#define SPI_4M() { SPSR &= ~(1<<SPI2X); SPCR &= 0b11111100; }
#define SPI_8M() { SPSR |=  (1<<SPI2X); SPCR &= 0b11111100; }
#endif

#endif // _SPI_H_
