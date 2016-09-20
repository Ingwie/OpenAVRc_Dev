/*
*************************************************************
*                      NEXTSTEPRC                           *
*                                                           *
*             -> Build your DIY MEGA 2560 TX                *
*                                                           *
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

#ifndef popup_h
#define popup_h

#include "menus.h"
#if defined(SIMU)
//#include "../targets/simu/simpgmspace.h"
#endif

const pm_char * warningText = NULL;
const pm_char * warningInfoText;
uint8_t         warningInfoLength;
uint8_t         warningType;
uint8_t         warningResult = 0;

const pm_uchar asterisk_lbm[] PROGMEM = {
  #include "../bitmaps/asterisk.lbm"
};

#if defined(NAVIGATION_MENUS)
const char *popupMenuItems[POPUP_MENU_MAX_LINES];
uint8_t s_menu_item = 0;
uint16_t popupMenuNoItems = 0;
uint8_t popupMenuFlags = 0;
uint16_t popupMenuOffset = 0;
#endif // defined


#endif
