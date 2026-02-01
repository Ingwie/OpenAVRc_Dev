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

#ifndef __LCD_H
#define __LCD_H

#include <stdint.h>
#include <avr/io.h>
#include <util/delay.h>
#include <avr/pgmspace.h>
#include <string.h>

// ST7565P, ST7565R, ERC12864FSF, ST7920, KS108, SSD1306, SH1106, NT7538SPI LT13264B
// Passed to makefile as LCD=KS108 then converted to a define via -D LCD_KS108.


#if defined(LCD_LT13264B)
 #define CONTRAST_MIN 10
 #define LCD_SIZE_132X64
#else
 #define CONTRAST_MIN 15
#endif

#define DIM(a) ((sizeof a) / (sizeof *a))

typedef uint8_t display_t;
typedef uint8_t coord_t;
typedef uint8_t LcdFlags;

#define LCD_W  128
#define LCD_H  64
#define DISPLAY_BUF_SIZE       (LCD_W*((LCD_H+7)/8))
#define DISPLAY_BUFER_SIZE     (sizeof(display_t)*DISPLAY_BUF_SIZE)
#define DISPLAY_END            (displayBuf + DISPLAY_BUF_SIZE)
#define ASSERT(x)
#define ASSERT_IN_DISPLAY(p)   ASSERT((p) >= displayBuf && (p) < DISPLAY_END)

void lcdPortInit(void);
void lcdInit(void);
void lcdRefresh(void);

#ifdef __AVR_XMEGA__

#else
// Mega2560 LCD Port
#define PORTA_LCD_DAT            PORTA
#define PORTC_LCD_CTRL           PORTC
//#if defined(LCD_KS108)              // (For KS108 LCD only) MEGA R/W pin always at 0 state in OpenAVRc then
  #define OUT_C_LCD_CS2          6  // use this pin to control second KS108 (CS2)
//#else                               // and connect KS108 R/W pin to ground via a 1k resistor
  #define OUT_C_LCD_RnW          6
//#endif
#define OUT_C_LCD_E              7
#define OUT_C_LCD_A0             5
#define OUT_C_LCD_RES            4
#define OUT_C_LCD_CS1            3
#define OUT_C_LIGHT              2
#define backlightEnable()        PORTC |= _BV(OUT_C_LIGHT)
#define backlightDisable()       PORTC &= ~_BV(OUT_C_LIGHT)

#endif

uint8_t displayBuf[DISPLAY_BUFER_SIZE];

#endif /* __LCD_H */

