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


#define NUMITERATIONFULLREFRESH  2

#define DISPLAY_SET_COLUMN       0x40
#define DISPLAY_SET_PAGE         0xB8
#define DISPLAY_SET_START        0XC0
#define DISPLAY_ON_CMD           0x3F //or 3E
#define CS1_on                   PORTC_LCD_CTRL |=  _BV(OUT_C_LCD_CS1)
#define CS1_off                  PORTC_LCD_CTRL &= ~_BV(OUT_C_LCD_CS1)
#define CS2_on                   PORTC_LCD_CTRL |=  _BV(OUT_C_LCD_CS2)
#define CS2_off                  PORTC_LCD_CTRL &= ~_BV(OUT_C_LCD_CS2)
#define A0_on                    PORTC_LCD_CTRL |=  _BV(OUT_C_LCD_A0)
#define A0_off                   PORTC_LCD_CTRL &= ~_BV(OUT_C_LCD_A0)
#define E_on                     PORTC_LCD_CTRL |=  _BV(OUT_C_LCD_E)
#define E_off                    PORTC_LCD_CTRL &= ~_BV(OUT_C_LCD_E)

void lcdPulseEnable()
{
  E_on;
  _delay_us(9); //Was 4 on the first tested
  E_off;
}

void lcdSendCtl(uint8_t val)
{
  PORTA_LCD_DAT = val;
  A0_off;
  lcdPulseEnable();
  A0_on;

}

static void LcdInitCommand()
{
  lcdSendCtl(DISPLAY_ON_CMD);
  lcdSendCtl(DISPLAY_SET_START);
}

void lcdInit()
{
  PORTC_LCD_CTRL &= ~_BV(OUT_C_LCD_RES);  //LCD reset
  _delay_us(20);
  PORTC_LCD_CTRL |= _BV(OUT_C_LCD_RES);  //LCD normal operation
  CS1_on;                                //Init KS108 who need hight level on CS pin
  CS2_on;
  LcdInitCommand();                      //Init the two KS in one time
  CS1_off;                               //Init KS108 who need low level on CS pin
  CS2_off;
  LcdInitCommand();
}

void lcdSetRefVolt(uint8_t val)
{
}

void lcdRefreshFast()
{
SHOWDURATIONLCD1
#if defined(SHOWDURATION)
  lcdDrawNumberAttUnit(16*FW, 1, DURATION_MS_PREC2(DurationValue), PREC2);
#endif
  static uint8_t change = 0; // toggle left or right lcd writing
  uint8_t *p;
  if (!change) {
    CS2_off;  // Right
    CS1_on;
    p = displayBuf;
    change = 1;
  } else {
    CS1_off;  // Left
    CS2_on;
    p = displayBuf + 64;
    change = 0;
  }

  for (uint8_t page=0; page < 8; page++) {
    lcdSendCtl(DISPLAY_SET_COLUMN); // Column addr 0
    lcdSendCtl( page | DISPLAY_SET_PAGE); //Page addr
    A0_on;
    for (coord_t x=64; x>0; --x) {
      PORTA_LCD_DAT = *p++;
      lcdPulseEnable();
    }
    p += 64;
  }
  A0_off;
SHOWDURATIONLCD2
}

void lcdRefresh()
{
  for (uint8_t i=0; i < NUMITERATIONFULLREFRESH; i++) {
    lcdRefreshFast();
  }
}
