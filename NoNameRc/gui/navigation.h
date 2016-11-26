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

#ifndef NAVIGATION_H
#define NAVIGATION_H

vertpos_t menuVerticalOffset;
uint8_t noHighlightCounter;
int8_t s_editMode;
uint8_t calibrationState;
vertpos_t menuVerticalPosition;
horzpos_t menuHorizontalPosition;

#if defined(NAVIGATION_POT1)
int16_t p1valdiff;
#endif

#if defined(NAVIGATION_POT2)
int8_t p2valdiff;
#endif

#endif
