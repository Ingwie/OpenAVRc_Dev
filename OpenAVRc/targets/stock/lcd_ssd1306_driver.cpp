/*
*************************************************************
*                      OpenAVRc                           *
*                                                           *
*             -> Build your DIY MEGA 2560 TX                *
*                                                           *
*      Based on code named                                  *
*      OpenTx - https://github.com/opentx/opentx            *
*                                                           *
*         Only avr code here for lisibility ;-)             *
*                                                           *
*  License GPLv2: http://www.gnu.org/licenses/gpl-2.0.html  *
*                                                           *
*************************************************************
*/

#define NUMITERATIONFULLREFRESH  4

#define SSD1306_ADDRESS             0x78
#define SSD1306_COMMAND             0x00
#define SSD1306_DATA                0x40

#define SSD1306_SETCONTRAST         0x81
#define SSD1306_DISPLAYALLON_RESUME 0xA4
#define SSD1306_DISPLAYALLON        0xA5
#define SSD1306_NORMALDISPLAY       0xA6
#define SSD1306_INVERTDISPLAY       0xA7
#define SSD1306_DISPLAYOFF          0xAE
#define SSD1306_DISPLAYON           0xAF
#define SSD1306_SETDISPLAYOFFSET    0xD3
#define SSD1306_SETCOMPINS          0xDA
#define SSD1306_SETVCOMDETECT       0xDB
#define SSD1306_SETDISPLAYCLOCKDIV  0xD5
#define SSD1306_SETPRECHARGE        0xD9
#define SSD1306_SETMULTIPLEX        0xA8
#define SSD1306_SETLOWCOLUMN        0x00
#define SSD1306_SETHIGHCOLUMN       0x10
#define SSD1306_SETSTARTLINE        0x40
#define SSD1306_MEMORYMODE          0x20
#define SSD1306_COLUMNADDR          0x21
#define SSD1306_PAGEADDR            0x22
#define SSD1306_COMSCANINC          0xC0
#define SSD1306_COMSCANDEC          0xC8
#define SSD1306_SEGREMAP            0xA0
#define SSD1306_CHARGEPUMP          0x8D
#define SSD1306_EXTERNALVCC         0x01
#define SSD1306_SWITCHCAPVCC        0x02

const static uint8_t lcdInitSequence[] PROGMEM =
{  // Init sequence for 128x64 OLED module
  SSD1306_DISPLAYOFF,                    // 0xAE
  SSD1306_SETDISPLAYCLOCKDIV,            // 0xD5
  0x80,                                  // the suggested ratio 0x80
  SSD1306_SETMULTIPLEX,                  // 0xA8
  0x3F,
  SSD1306_SETDISPLAYOFFSET,              // 0xD3
  0x0,                                   // no offset
  SSD1306_SETSTARTLINE,                  // line #0
  SSD1306_CHARGEPUMP,                    // 0x8D
  0x14,                                  // using internal VCC
  SSD1306_MEMORYMODE,                    // 0x20
  0x00,                                  // 0x00 horizontal addressing
  SSD1306_SEGREMAP | 0x1,                // rotate screen 180
  SSD1306_COMSCANDEC,                    // rotate screen 180
  SSD1306_SETCOMPINS,                    // 0xDA
  0x12,
  SSD1306_SETCONTRAST,                   // 0x81
  0xCF,
  SSD1306_SETPRECHARGE,                  // 0xd9
  0xF1,
  SSD1306_SETVCOMDETECT,                 // 0xDB
  0x40,
  SSD1306_DISPLAYALLON_RESUME,           // 0xA4
  SSD1306_NORMALDISPLAY,                 // 0xA6
  SSD1306_DISPLAYON                      //switch on OLED
};

void lcdSendCommand(uint8_t data)
{
  i2c_start(SSD1306_ADDRESS);
  i2c_write(SSD1306_COMMAND);
  i2c_write(data);
  i2c_stop();
}

void lcdInit()
{
  for (uint8_t i=0; i<DIM(lcdInitSequence); i++) {
    lcdSendCommand(pgm_read_byte(&lcdInitSequence[i]));
  }
}

void lcdSetRefVolt(uint8_t val)
{
  lcdSendCommand(0x81);
  lcdSendCommand(255-val*8);  //Todo can be better adjusted
}

void lcdRefreshFast()
{
  REFRESHDURATION1  //Debug function if defined LCDDURATIONSHOW in OpenAVRc.h

  static uint8_t state = 0;
  uint8_t *p;

  // Set Column
  lcdSendCommand(SSD1306_COLUMNADDR); // 0x21 COMMAND
  lcdSendCommand(0); // Column start address
  lcdSendCommand(LCD_W-1); // Column end address
  // Set page
  lcdSendCommand(SSD1306_PAGEADDR); // 0x22 COMMAND
  lcdSendCommand(state); // Start Page address
  lcdSendCommand(state+1);// End Page address

  p = displayBuf;
  i2c_writeReg(SSD1306_ADDRESS, SSD1306_DATA, (p + (LCD_W*state)), (2*LCD_W)); // Send datas ( 800 Khz I2C bus ;-)
  state+=2;
  if (state == (NUMITERATIONFULLREFRESH*2)) { state = 0; }
  
  REFRESHDURATION2  //Debug function if defined LCDDURATIONSHOW in OpenAVRc.h
}

void lcdRefresh()
{
  for (uint8_t i=0; i < NUMITERATIONFULLREFRESH; i++) { lcdRefreshFast(); }
}
