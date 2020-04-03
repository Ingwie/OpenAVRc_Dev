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
#if defined(EXTERNALEEPROM)
#define CHECK_IIC_USED_IRQ_MODE if (TWCR & _BV(TWINT)) return;
#else
#define CHECK_IIC_USED_IRQ_MODE // I2C bus is free !
#endif
#define CASE_PERSISTENT_TIMERS(x) x,


#define NUMITERATIONFULLREFRESH  4

#define SH1106_ADDRESS             (0x3C << 1) // = 0x78
#define SH1106_COMMAND             0x00
#define SH1106_DATA                0x40

#define SH1106_SETCONTRAST         0x81
#define SH1106_DISPLAYALLON_RESUME 0xA4
#define SH1106_DISPLAYALLON        0xA5
#define SH1106_NORMALDISPLAY       0xA6
#define SH1106_INVERTDISPLAY       0xA7
#define SH1106_DISPLAYOFF          0xAE
#define SH1106_DISPLAYON           0xAF
#define SH1106_SETDISPLAYOFFSET    0xD3
#define SH1106_SETCOMPINS          0xDA
#define SH1106_SETVCOMDETECT       0xDB
#define SH1106_SETDISPLAYCLOCKDIV  0xD5
#define SH1106_SETPRECHARGE        0xD9
#define SH1106_SETMULTIPLEX        0xA8
#define SH1106_SETLOWCOLUMN        0x00
#define SH1106_SETHIGHCOLUMN       0x10
#define SH1106_SETPAGENUMBER       0xB0
#define SH1106_SETSTARTLINE        0x40
#define SH1106_MEMORYMODE          0x20
#define SH1106_COMSCANINC          0xC0
#define SH1106_COMSCANDEC          0xC8
#define SH1106_SEGREMAP            0xA0
#define SH1106_CHARGEPUMP          0x8D
#define SH1106_EXTERNALVCC         0x01
#define SH1106_SWITCHCAPVCC        0x02

const static uint8_t lcdInitSequence[] PROGMEM = {
  // Init sequence for 128x64 OLED module (Modded from SSD1306 driver)
  SH1106_DISPLAYOFF,                    // 0xAE
  SH1106_SETDISPLAYCLOCKDIV,            // 0xD5
  0x80,                                  // the suggested ratio 0x80
  SH1106_SETMULTIPLEX,                  // 0xA8
  0x3F,
  SH1106_CHARGEPUMP,                    // 0x8D
  0x14,                                  // using internal VCC
  SH1106_MEMORYMODE,                    // 0x20
  0x00,                                  // 0x00 horizontal addressing
  SH1106_SEGREMAP | 0x1,                // rotate screen 180
  SH1106_COMSCANDEC,                    // rotate screen 180
  SH1106_SETCOMPINS,                    // 0xDA
  0x12,
  SH1106_SETCONTRAST,                   // 0x81
  0xCF,
  SH1106_SETPRECHARGE,                  // 0xd9
  0xF1,
  SH1106_SETVCOMDETECT,                 // 0xDB
  0x40,
  SH1106_DISPLAYALLON_RESUME,           // 0xA4
  SH1106_NORMALDISPLAY,                 // 0xA6
  SH1106_DISPLAYON                      //switch on OLED
};

void lcdSendCommand(uint8_t data)
{
  CHECK_IIC_USED_IRQ_MODE;
  i2c_start(SH1106_ADDRESS);
  i2c_write(SH1106_COMMAND);
  i2c_write(data);
  i2c_stop();
}

void lcdInit()
{
  for (uint8_t i=0; i<DIM(lcdInitSequence); i++) {
    lcdSendCommand(pgm_read_byte_near(&lcdInitSequence[i]));
  }
}

void lcdSetRefVolt(uint8_t val)
{
  lcdSendCommand(0x81);
  lcdSendCommand((val-10)*7);
}

void lcdRefreshFast() //4.25mS
{
SHOWDURATIONLCD1
#if defined(SHOWDURATION)
  lcdDrawNumberNAtt(16*FW, 1, DURATION_MS_PREC2(DurationValue), PREC2);
#endif
  static uint8_t state = 0;
  uint8_t *p;

  CHECK_IIC_USED_IRQ_MODE;
  // Set Column
  lcdSendCommand(SH1106_SETLOWCOLUMN);
  lcdSendCommand(SH1106_SETHIGHCOLUMN);
  lcdSendCommand(SH1106_SETPAGENUMBER | state);

  p = displayBuf;
  i2c_writeReg(SH1106_ADDRESS, SH1106_DATA, (p-2 + (LCD_W*state)), (LCD_W + 4)); // Send datas ( 888 Khz I2C bus ;-)
  lcdSendCommand(SH1106_SETPAGENUMBER | ++state);
  i2c_writeReg(SH1106_ADDRESS, SH1106_DATA, (p-2 + (LCD_W*state)), (LCD_W + 4)); // Send datas ( 888 Khz I2C bus ;-)
  ++state;
  if (state == (NUMITERATIONFULLREFRESH*2)) {
    state = 0;
  }
SHOWDURATIONLCD2
}

void lcdRefresh()
{
  for (uint8_t i=0; i < NUMITERATIONFULLREFRESH; i++) {
    lcdRefreshFast();
  }
}
