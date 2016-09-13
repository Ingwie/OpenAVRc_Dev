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

#include "../nextsteprc.h"
#include "menu_general.h"


void displayKeyState(uint8_t x, uint8_t y, EnumKeys key)
{
  uint8_t t = switchState(key);
  lcdDrawCharAtt(x, y, t+'0', t ? INVERS : 0);
}

void menuGeneralDiagKeys(uint8_t event)
{
  SIMPLE_MENU(STR_MENUDIAG, menuTabGeneral, e_Keys, 1);

  lcdDrawText(14*FW, MENU_HEADER_HEIGHT+2*FH, STR_VTRIM);

  for(uint8_t i=0; i<9; i++) {
    coord_t y;

    if (i<8) {
      y = MENU_HEADER_HEIGHT + FH*3 + FH*(i/2);
      if (i&1) lcd_img(14*FW, y, sticks, i/2, 0);
      displayKeyState(i&1? 20*FW : 18*FW, y, (EnumKeys)(TRM_BASE+i));
    }

    if (i<6) {
      y = i*FH+MENU_HEADER_HEIGHT+FH;
      lcdDrawTextAtIndex(0, y, STR_VKEYS, (5-i), 0);
      displayKeyState(5*FW+2, y, (EnumKeys)(KEY_MENU+(5-i)));
    }

    if (i != SW_ID0-SW_BASE) {
      y = MENU_HEADER_HEIGHT+i*FH-2*FH;
      lcdPutsSwitches(8*FW, y, i+1, 0); //ohne off,on
      displayKeyState(11*FW+2, y, (EnumKeys)(SW_BASE+i));
    }
  }

#if defined(ROTARY_ENCODERS) || defined(ROTARY_ENCODER_NAVIGATION)
  for(uint8_t i=0; i<DIM(g_rotenc); i++) {
    coord_t y = MENU_HEADER_HEIGHT /* ??? + 1 ??? */ + i*FH;
    lcdDrawTextAtIndex(14*FW, y, STR_VRENCODERS, i, 0);
    int16_t rex = getRotaryEncoder(i);
    rex /= 8;
    lcdDrawNumberNAtt(18*FW, y, rex, LEFT|(switchState((EnumKeys)(BTN_REa+i)) ? INVERS : 0));
  }
#endif

}
