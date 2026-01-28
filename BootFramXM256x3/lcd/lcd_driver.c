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
#include "lcd/lcd.h"

static void lcd_imgfar(coord_t x, coord_t y,  uint_farptr_t img, uint8_t idx, LcdFlags att);

static const uint8_t desktop_icon[] PROGMEM = {
#include "desktop.lbm"
};

static void lcd_imgfar(coord_t x, coord_t y,  uint_farptr_t img, uint8_t idx, LcdFlags att) // progmem "far"
{
  uint_farptr_t q = img;
  uint8_t w    = pgm_read_byte_far(q++);
  uint8_t hb   = (pgm_read_byte_far(q++)+7)/8;

  q += idx*w*hb;
  for (uint8_t yb = 0; yb < hb; yb++) {
    uint8_t *p = &displayBuf[ (y / 8 + yb) * LCD_W + x ];
    for (coord_t i=0; i<w; i++) {
      uint8_t b = pgm_read_byte_far(q);
      q++;
      ASSERT_IN_DISPLAY(p);
      *p++ = b;
    }
  }
}


void lcdPortInit()
{
  memset(displayBuf, 0x00, DISPLAY_BUFER_SIZE);
  lcd_imgfar(32, 0, (pgm_get_far_address(desktop_icon)), 0, 0);

 // Setup LCD port pin states.
#ifdef __AVR_XMEGA__

#else // ATMEGA
  DDRA = 0b11111111;  PORTA = 0b00000000; // LCD data
  DDRC = 0b11111100;  PORTC = 0b00000011; // 7-3:LCD, 2:BackLight, 1:ID2_SW, 0:ID1_SW
#endif // __AVR_XMEGA__
}


#if 0 // Makefile can compile all drivers and select the required driver at Link time.
#include "../../OpenAVRc.h"


#if defined(SIMU)
#include "lcd_simu_driver.cpp"
#else
#if defined(LCD_KS108)
#include "targets/lcd/lcd_ks108_driver.cpp"
#elif defined(LCD_ST7920)
#include "targets/lcd/lcd_st7920_driver.cpp"
#elif defined(LCD_SSD1306)
#include "targets/lcd/lcd_ssd1306_driver.cpp"
#elif defined(LCD_SH1106)
#include "targets/lcd/lcd_sh1106_driver.cpp"
#else
#include "targets/lcd/lcd_default_driver.cpp"
#endif
#endif // defined
#endif
