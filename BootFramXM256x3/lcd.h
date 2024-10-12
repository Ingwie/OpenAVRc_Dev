/*
 * lcd.h
 *
 *  Created on: 9 Apr 2020
 *      Author: sloped-soarer
 */

#ifndef LCD_H_
#define LCD_H_


//#define LCD_SSD1309
#define LCD_EVO
//#define LCD_ST7567

#if defined(LCD_EVO)
 #define CONTRAST_MIN 10
 #define LCD_SIZE_132X64
 #define POSITIVE_CHIP_SELECT
#endif
#if defined(LCD_ST7567)
 #define CONTRAST_MIN 35
 #define LCD_SIZE_132X64
 #define POSITIVE_CHIP_SELECT
#endif
#if defined(LCD_SSD1309)
 #define CONTRAST_MIN 30
#endif



#define LCD_W  128
#define LCD_H  64

#define DIM(a) ((sizeof a) / (sizeof *a))

#define LcdFlags  uint8_t

typedef uint8_t display_t;
typedef uint8_t coord_t;

#define DISPLAY_BUF_SIZE     (LCD_W*((LCD_H+7)/8))

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

