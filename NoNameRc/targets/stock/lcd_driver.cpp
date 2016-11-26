/*
 *************************************************************
 *                      NoNameRc                           *
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

#include "../../NoNameRc.h"


#if defined(SIMU)
#include "lcd_simu_driver.cpp"
#else
#if defined(LCD_KS108)
#include "targets/stock/lcd_ks108_driver.cpp"
#elif defined(LCD_ST7920)
#include "targets/stock/lcd_st7920_driver.cpp"
#elif defined(LCD_SSD1306)
#include "targets/stock/lcd_ssd1306_driver.cpp"
#else
#include "targets/stock/lcd_default_driver.cpp"
#endif
#endif // defined
