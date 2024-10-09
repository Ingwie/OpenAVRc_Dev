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

void master_spi_disable(void);
void enable_spi_master_mode(void);
uint8_t master_spi_xfer(uint8_t value);

// SPI general.
#define MSPI_250K(spix)  { spix.CTRL &= ~(SPI_CLK2X_bm | SPI_PRESCALER_gm); spix.CTRL |= 0b11 << SPI_PRESCALER_gp ;}
#define MSPI_500K(spix)  { spix.CTRL &= ~(SPI_CLK2X_bm | SPI_PRESCALER_gm); spix.CTRL |= 0b10 << SPI_PRESCALER_gp ;}
#define MSPI_1M(spix)    { spix.CTRL &= ~(SPI_CLK2X_bm | SPI_PRESCALER_gm); spix.CTRL |= 0b10 << SPI_PRESCALER_gp | SPI_CLK2X_bm ;}
#define MSPI_2M(spix)    { spix.CTRL &= ~(SPI_CLK2X_bm | SPI_PRESCALER_gm); spix.CTRL |= 0b01 << SPI_PRESCALER_gp ;}
#define MSPI_4M(spix)    { spix.CTRL &= ~(SPI_CLK2X_bm | SPI_PRESCALER_gm); spix.CTRL |= 0b01 << SPI_PRESCALER_gp | SPI_CLK2X_bm ;}
#define MSPI_8M(spix)    { spix.CTRL &= ~(SPI_CLK2X_bm | SPI_PRESCALER_gm) }
#define MSPI_16M(spix)   { spix.CTRL &= ~(SPI_CLK2X_bm | SPI_PRESCALER_gm); spix.CTRL |=  SPI_CLK2X_bm ;}

#endif // _SPI_H_
