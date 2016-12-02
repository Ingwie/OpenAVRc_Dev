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

#ifndef fonts_h
#define fonts_h

extern const pm_uchar font_5x7[];
extern const pm_uchar font_10x14[];

#if defined(BOLD_FONT) && (defined(EXTSTD) || defined(PCBMEGA2560)) && !defined(BOOT)
 #define BOLD_SPECIFIC_FONT
 extern const pm_uchar font_5x7_B[];
#endif

#endif
