/*
 *************************************************************
 *                      NEXTSTEPRC                           *
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

#include "lcd.h"
#include "menus.h"

#define NUM_BODY_LINES      (LCD_LINES-1)
#define MENU_HEADER_HEIGHT  FH

void displaySplash();
void displayScreenIndex(uint8_t index, uint8_t count, uint8_t attr);

#if !defined(CPUM64)
  #define DEFAULT_SCROLLBAR_X (LCD_W-1)
  void displayScrollbar(coord_t x, coord_t y, coord_t h, uint16_t offset, uint16_t count, uint8_t visible);
#endif

#define SET_SCROLLBAR_X(x)
