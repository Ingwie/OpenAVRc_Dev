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


#define NUMITERATIONFULLREFRESH  5

void lcdSendCtl(uint8_t val)
{
  PORTC_LCD_CTRL &= ~(1<<OUT_C_LCD_CS1);
  PORTC_LCD_CTRL &= ~(1<<OUT_C_LCD_A0);
  PORTC_LCD_CTRL &= ~(1<<OUT_C_LCD_RnW);
  PORTA_LCD_DAT = val;
  PORTC_LCD_CTRL |=  (1<<OUT_C_LCD_E);
  PORTC_LCD_CTRL &= ~(1<<OUT_C_LCD_E);
  PORTC_LCD_CTRL |=  (1<<OUT_C_LCD_A0);
  PORTC_LCD_CTRL |=  (1<<OUT_C_LCD_CS1);
}

const static pm_uchar lcdInitSequence[] PROGMEM = {
  0x30, // Set 8-bit interface
  0x36, // Repeat with graphics bit set to ON
  0x0C, // Display ON, cursor and blink OFF
  0x01, // Clear display, reset address
  0x06  // Display ON, no cursor
};

void lcdInit()
{
  LCD_LOCK();
  PORTC_LCD_CTRL &= ~(1<<OUT_C_LCD_RES); //LCD reset
  _delay_us(2);
  PORTC_LCD_CTRL |= (1<<OUT_C_LCD_RES);  //LCD normal operation
  _delay_ms(40);
  for (uint8_t i=0; i<DIM(lcdInitSequence); i++) {
    lcdSendCtl(pgm_read_byte(&lcdInitSequence[i])) ;
    _delay_us(80);
  }
  g_eeGeneral.contrast = 0x22;
  LCD_UNLOCK();
}

void lcdSetRefVolt(uint8_t val)
{
}

void lcdRefreshFast()
{
  REFRESHDURATION1  //Debug function if defined LCDDURATIONSHOW in OpenAVRc.h

  LCD_LOCK();
  static uint8_t state;
  uint8_t yst;
  uint8_t yend;
  uint8_t y_table[6]= {0,13,26,39,52,64};
  uint8_t y_addr = 0;
  uint8_t col_offset = 0;
  uint8_t result;
  uint8_t *p;

  //Since writing to ST7920 is too slow we need to split it to five bands
  yst=y_table[state];
  yend=y_table[state+1];
  if (state==4) {
    state=0;
  } else {
    state++;
  }


  for (uint8_t y=yst; y<yend; y++) {
    uint8_t x_addr = 0;
    //Convert coordinates to weirdly-arranged 128x64 screen (the ST7920 is mapped for 256x32 displays)
    if (y > 31) {
      y_addr = y - 32;    //Because there are only 31 addressable lines in the ST7920
      x_addr += 8;        //so we overflow x (7 visible bytes per line) to reach the bottom half
    } else {
      y_addr = y;
    }
    lcdSendCtl( 0x80 | y_addr );  //Set Vertical Address
    _delay_us(49);
    lcdSendCtl( 0x80 | x_addr );  //Set Horizontal Address
    _delay_us(49);
    PORTC_LCD_CTRL |= (1<<OUT_C_LCD_A0);    //HIGH RS and LOW RW will put the LCD to
    PORTC_LCD_CTRL &= ~(1<<OUT_C_LCD_RnW);  //Write data register mode
    uint8_t bit_count = y & 0x07; //Count from 0 bis 7 -> 0=0, 1=1..7=7, 8=0, 9=1...
    col_offset = 1 << bit_count; //Build a value for a AND operation with the vorrect bitposition
    uint16_t line_offset = ( y / 8 ) * 128; //On the ST7920 there are 8 lines with each 128 bytes width
    p = displayBuf + line_offset; //Calculate the position of the first byte im array
    for (coord_t x=0; x<16; x++) { //Walk through 16 bytes form left to right (128 Pixel)
      // adressing the bytes sequential and set the bits at the correct position merging them with an OR operation to get all bits in one byte
      // the position of the LSB is the right-most position of the byte to the ST7920
      result = ((*p++ & col_offset)!=0?0x80:0);
      result |= ((*p++ & col_offset)!=0?0x40:0);
      result |= ((*p++  & col_offset)!=0?0x20:0);
      result |= ((*p++  & col_offset)!=0?0x10:0);
      result |= ((*p++  & col_offset)!=0?0x08:0);
      result |= ((*p++  & col_offset)!=0?0x04:0);
      result |= ((*p++  & col_offset) !=0?0x02:0);
      result |= ((*p++  & col_offset)!=0?0x01:0);
      PORTA_LCD_DAT = result;
      PORTC_LCD_CTRL |= (1<<OUT_C_LCD_E); // ST7920 auto increase x counter and roll from 0F to 00
      _delay_us(1);
      PORTC_LCD_CTRL &= ~(1<<OUT_C_LCD_E);
      _delay_us(8);
    }
    _delay_us(41);

  }

  LCD_UNLOCK();

  REFRESHDURATION2  //Debug function if defined LCDDURATIONSHOW in OpenAVRc.h
}

void lcdRefresh()
{
  for (uint8_t i=0; i < NUMITERATIONFULLREFRESH; i++) {
    lcdRefreshFast();
  }
}
