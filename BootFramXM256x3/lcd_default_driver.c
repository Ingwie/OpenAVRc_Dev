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

#include <stdint.h>
#include <avr/io.h>
#include <util/delay.h>
#include <avr/pgmspace.h>
#include <string.h>
#include "lcd.h"

#define NUMITERATIONFULLREFRESH  1

// LCD driver
#define PORTA_LCD_DAT            PORTA
#define PORTC_LCD_CTRL           PORTC
#if defined(LCD_KS108)              // (For KS108 LCD only) MEGA R/W pin always at 0 state in OpenAVRc then
  #define OUT_C_LCD_CS2          6  // use this pin to control second KS108 (CS2)
#else                               // and connect KS108 R/W pin to ground via a 1k resistor
  #define OUT_C_LCD_RnW          6
#endif
#define OUT_C_LCD_E              7
#define OUT_C_LCD_A0             5
#define OUT_C_LCD_RES            4
#define OUT_C_LCD_CS1            3
#define OUT_C_LIGHT              2

void lcdSendCtl(uint8_t val)
{
  PORTC_LCD_CTRL &= ~_BV(OUT_C_LCD_CS1);
#if defined(LCD_MULTIPLEX)
  DDRA = 0xFF; //Set LCD_DAT pins to output
#endif
  PORTC_LCD_CTRL &= ~_BV(OUT_C_LCD_A0);
  PORTC_LCD_CTRL &= ~_BV(OUT_C_LCD_RnW);
  PORTA_LCD_DAT = val;
  PORTC_LCD_CTRL |=  _BV(OUT_C_LCD_E);
  PORTC_LCD_CTRL &= ~_BV(OUT_C_LCD_E);
  PORTC_LCD_CTRL |=  _BV(OUT_C_LCD_A0);
#if defined(LCD_MULTIPLEX)
  DDRA = 0x00; //Set LCD_DAT pins to input
#endif
  PORTC_LCD_CTRL |=  _BV(OUT_C_LCD_CS1);
}

const static uint8_t lcdInitSequence[] PROGMEM = {
  //ST7565 eq. : KS0713, SED1565, S6B1713, SPLC501C, NT7532 /34 /38, TL03245
#if defined(LCD_ST7565R)
  0xE2, //Initialize the internal functions
  0xAE, //DON = 0: display OFF
  0xA0, //ADC = 0: normal direction (SEG132->SEG1)
  0xA6, //REV = 0: non-reverse display
  0xA4, //EON = 0: normal display. non-entire
  0xA2, //Select LCD bias
  0xC8, //SHL = 1: reverse direction (COM64->COM1)
  0x2F, //Control power circuit operation VC=VR=VF=1
  0x25, //Select int resistance ratio R2 R1 R0 =5
  0x81, //Set reference voltage Mode
  0x22, //24 SV5 SV4 SV3 SV2 SV1 SV0 = 0x18
  0xAF, //DON = 1: display ON
  0x60  //Set the display start line to zero
#elif defined(LCD_ERC12864FSF)
  0xE2, //Initialize the internal functions
  0xAE, //DON = 0: display OFF
  0xA1, //ADC = 1: reverse direction (SEG132->SEG1)
  0xA6, //REV = 0: non-reverse display
  0xA4, //EON = 0: normal display. non-entire
  0xA3, //Select LCD bias
  0xC0, //SHL = 0: normal direction (COM1->COM64)
  0x2F, //Control power circuit operation VC=VR=VF=1
  0x27, //Select int resistance ratio R2 R1 R0
  0x81, //Set reference voltage Mode
  0x2D, //24 SV5 SV4 SV3 SV2 SV1 SV0
  0xAF  //DON = 1: display ON
#else    //ST7565P (default 9x LCD)
  0xE2, //Initialize the internal functions
  0xAE, //DON = 0: display OFF
  0xA1, //ADC = 1: reverse direction(SEG132->SEG1)
  0xA6, //REV = 0: non-reverse display
  0xA4, //EON = 0: normal display. non-entire
  0xA2, //Select LCD bias=0
  0xC0, //SHL = 0: normal direction (COM1->COM64)
  0x2F, //Control power circuit operation VC=VR=VF=1
  0x25, //Select int resistance ratio R2 R1 R0 =5
  0x81, //Set reference voltage Mode
  0x22, //24 SV5 SV4 SV3 SV2 SV1 SV0 = 0x18
  0xAF  //DON = 1: display ON
#endif
};

const uint8_t desktop_icon[] PROGMEM = {
//#if defined (__AVR_XMEGA__)
#include "desktop.lbm"
//#else
//16,8,0x00,0x24,0x66,0x7E,0x3C,0x18,0x18,0x18,0x18,0x18,0x18,0x3C,0x66,0x66,0x3C,0x00
//#endif
};

uint8_t displayBuf[DISPLAY_BUFER_SIZE];

void lcd_imgfar(coord_t x, coord_t y,  uint_farptr_t img, uint8_t idx, LcdFlags att) // progmem "far"
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

void lcdInit()
{
  memset(displayBuf, 0x00, DISPLAY_BUFER_SIZE);

//#if defined (__AVR_XMEGA__)
  lcd_imgfar(32, 0, (pgm_get_far_address(desktop_icon)), 0, 0);
//#else
//  lcd_imgfar(0, 0, (pgm_get_far_address(desktop_icon)), 0, 0);
//#endif

  PORTC_LCD_CTRL &= ~_BV(OUT_C_LCD_RES);  //LCD reset
  _delay_us(2);
  PORTC_LCD_CTRL |= _BV(OUT_C_LCD_RES);  //LCD normal operation
  _delay_us(1500);
  for (uint8_t i=0; i<DIM(lcdInitSequence); i++) {
    lcdSendCtl(pgm_read_byte_near(&lcdInitSequence[i]));
  }
}

void lcdSetRefVolt(uint8_t val)
{
  lcdSendCtl(0x81);
#if defined(LCD_ERC12864FSF)
  val = 0x2D;
#else
  val = 0x22;
#endif
  lcdSendCtl(val);
}


void lcdRefreshFast()
{
SHOWDURATIONLCD1
#if defined(SHOWDURATION)
  lcdDrawNumberNAtt(16*FW, 1, DURATION_MS_PREC2(DurationValue), PREC2);
#endif
  uint8_t * p = displayBuf;
  for (uint8_t y=0; y < 8; y++) {
#if defined(LCD_ST7565R)
    lcdSendCtl(0x01);
#else
    lcdSendCtl(0x04);
#endif
    lcdSendCtl(0x10); // Column addr 0
    lcdSendCtl( y | 0xB0); //Page addr y
    PORTC_LCD_CTRL &= ~_BV(OUT_C_LCD_CS1);
#if defined(LCD_MULTIPLEX)
    DDRA = 0xFF; // Set LCD_DAT pins to output
#endif
    PORTC_LCD_CTRL |=  _BV(OUT_C_LCD_A0);
    PORTC_LCD_CTRL &= ~_BV(OUT_C_LCD_RnW);
    for (coord_t x=LCD_W; x>0; --x) {
      PORTA_LCD_DAT = *p++;
      PORTC_LCD_CTRL |= _BV(OUT_C_LCD_E);
      PORTC_LCD_CTRL &= ~_BV(OUT_C_LCD_E);
    }
    PORTC_LCD_CTRL |=  _BV(OUT_C_LCD_A0);
    PORTC_LCD_CTRL |=  _BV(OUT_C_LCD_CS1);
  }
SHOWDURATIONLCD2
}

void lcdRefresh()
{
  for (uint8_t i=0; i < NUMITERATIONFULLREFRESH; i++) {
    lcdRefreshFast();
  }
}
