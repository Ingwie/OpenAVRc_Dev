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
static void lcdRefreshFast(void);


#define NUMITERATIONFULLREFRESH  1


static void lcdSendCmd(uint8_t val)
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

static const uint8_t lcdInitSequence[] PROGMEM = {
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
#elif defined(LCD_LT13264B) // SPLC501C 132x64 Inverse.
  0xE2, // CMD_RESET
  0xAE, // CMD_DISPLAY_OFF
  0xA1, // CMD_REVERSE_SEG_DIRECTION
  0xA6, // ST756n_CMD_DISPLAY_NORMAL
  0xA4, // CMD_DISPLAY_ALL_POINTS_OFF
  0xA2, // CMD_BIAS_SELECT_9TH
  0xC0, // CMD_NORMAL_COM_DIRECTION
  0x2F, // CMD_POWER_CTRL
  0x24, // *** Use default 0x24 *** CMD_VOLTAGE_RESISTOR_RATIO ***
  0x81, // CMD_CONTRAST_SET
  CONTRAST_MIN,  // Contrast value.
  0xAF  // CMD_DISPLAY_ON
#else   //ST7565P (default 9x LCD)
  0xE2, //ST756n_CMD_RESET
  0xAE, //ST756n_CMD_DISPLAY_OFF
  0xA1, //ST756n_CMD_REVERSE_SEG_DIRECTION (SEG132->SEG1)
  0xA6, //ST756n_CMD_DISPLAY_NORMAL
  0xA4, //ST756n_CMD_DISPLAY_ALL_POINTS_OFF
  0xA2, //ST756n_CMD_BIAS_SELECT_9TH
  0xC0, //ST756n_CMD_NORMAL_COM_DIRECTION (COM1->COM64)
  0x2F, //Control power circuit operation VC=VR=VF=1
  0x25, //ST756n_CMD_VOLTAGE_RESISTOR_RATIO_5dec5
  0x81, //ST756n_CMD_CONTRAST_SET
  0x22, //// Contrast value.
  0xAF  //ST756n_CMD_DISPLAY_ON
#endif
};

void lcdInit()
{
  PORTC_LCD_CTRL &= ~_BV(OUT_C_LCD_RES);  //LCD reset
  _delay_us(2);
  PORTC_LCD_CTRL |= _BV(OUT_C_LCD_RES);  //LCD normal operation
  _delay_us(1500);

  uint_farptr_t initseq = pgm_get_far_address(lcdInitSequence);
  for (uint8_t i=0; i<DIM(lcdInitSequence); i++) {
    lcdSendCmd(pgm_read_byte_far(initseq++));
  }

#if defined(LCD_ERC12864FSF)
//  g_eeGeneral.contrast = 0x2D;
#else
//  g_eeGeneral.contrast = 0x22;
#endif
}

static void lcdSetRefVolt(uint8_t val)
{
  lcdSendCmd(0x81);
  lcdSendCmd(val);
}

static void lcdRefreshFast()
{
SHOWDURATIONLCD1
#if defined(SHOWDURATION)
  lcdDrawNumberNAtt(16*FW, 1, DURATION_MS_PREC2(DurationValue), PREC2);
#endif
  uint8_t * p = displayBuf;
  for (uint8_t y=0; y < 8; y++) {
#if defined(LCD_ST7565R)
    lcdSendCmd(0x01);
#elif defined(LCD_SIZE_132X64)
    lcdSendCmd(0x00);
#else
    lcdSendCmd(0x04);
#endif
    lcdSendCmd(0x10); // Column addr 0
    lcdSendCmd( y | 0xB0); //Page addr y
    PORTC_LCD_CTRL &= ~_BV(OUT_C_LCD_CS1);
#if defined(LCD_MULTIPLEX)
    DDRA = 0xFF; // Set LCD_DAT pins to output
#endif
    PORTC_LCD_CTRL |=  _BV(OUT_C_LCD_A0);
    PORTC_LCD_CTRL &= ~_BV(OUT_C_LCD_RnW);
    #if defined (LCD_SIZE_132X64)
    PORTA_LCD_DAT = (0xff); // Pad out the four unused columns.
    PORTC_LCD_CTRL |= _BV(OUT_C_LCD_E);
    PORTC_LCD_CTRL &= ~_BV(OUT_C_LCD_E);
    PORTA_LCD_DAT = (0xff);
    PORTC_LCD_CTRL |= _BV(OUT_C_LCD_E);
    PORTC_LCD_CTRL &= ~_BV(OUT_C_LCD_E);
    #endif
    for (coord_t x=LCD_W; x>0; --x) {
      PORTA_LCD_DAT = *p++;
      PORTC_LCD_CTRL |= _BV(OUT_C_LCD_E);
      PORTC_LCD_CTRL &= ~_BV(OUT_C_LCD_E);
    }
    #if defined (LCD_SIZE_132X64)
    PORTA_LCD_DAT = (0xff);
    PORTC_LCD_CTRL |= _BV(OUT_C_LCD_E);
    PORTC_LCD_CTRL &= ~_BV(OUT_C_LCD_E);
    PORTA_LCD_DAT = (0xff);
    PORTC_LCD_CTRL |= _BV(OUT_C_LCD_E);
    PORTC_LCD_CTRL &= ~_BV(OUT_C_LCD_E);
    #endif
    PORTC_LCD_CTRL |=  _BV(OUT_C_LCD_A0);
    PORTC_LCD_CTRL |=  _BV(OUT_C_LCD_CS1);
  }
SHOWDURATIONLCD2
}

#if 0
static void lcdRefreshFast()
{
SHOWDURATIONLCD1
#if defined(SHOWDURATION)
  lcdDrawNumberNAtt(16*FW, 1, DURATION_MS_PREC2(DurationValue), PREC2);
#endif
  uint8_t * p = displayBuf;
  for (uint8_t y=0; y < 8; y++) {
#if defined(LCD_ST7565R)
    lcdSendCmd(0x01);
#else
    lcdSendCmd(0x04);
#endif
    lcdSendCmd(0x10); // Column addr 0
    lcdSendCmd( y | 0xB0); //Page addr y
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
#endif

void lcdRefresh()
{
  for (uint8_t i=0; i < NUMITERATIONFULLREFRESH; i++) {
    lcdRefreshFast();
  }
}
