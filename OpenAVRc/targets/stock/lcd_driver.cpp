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


#include "../../OpenAVRc.h"


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
