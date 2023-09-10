/*
 * lcd.h
 *
 *  Created on: 9 Apr 2020
 *      Author: sloped-soarer
 */

#ifndef LCD_H_
#define LCD_H_

#define BITBANGSPI
#define LCD_W  128
#define LCD_H  64


#define LcdFlags  uint8_t

typedef uint8_t display_t;
typedef uint8_t coord_t;

#define DISPLAY_BUF_SIZE     (LCD_W*((LCD_H+7)/8))

extern display_t displayBuf[DISPLAY_BUF_SIZE];

#define DISPLAY_BUFER_SIZE     (sizeof(display_t)*DISPLAY_BUF_SIZE)
#define DISPLAY_END            (displayBuf + DISPLAY_BUF_SIZE)
#define ASSERT(x)
#define ASSERT_IN_DISPLAY(p)   ASSERT((p) >= displayBuf && (p) < DISPLAY_END)

#define SHOWDURATIONLCD1
#define SHOWDURATIONLCD2


void lcdInit(void);
void lcdRefreshFast(void);
void lcdRefresh(void);
void lcd_imgfar(coord_t x, coord_t y,  uint_farptr_t img, uint8_t idx, LcdFlags att);

#endif /* LCD_H_ */

