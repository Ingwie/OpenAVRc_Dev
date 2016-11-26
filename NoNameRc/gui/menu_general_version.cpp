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

#include "../NoNameRc.h"
#include "menu_general.h"


void menuGeneralVersion(uint8_t event)
{
  SIMPLE_MENU(STR_MENUVERSION, menuTabGeneral, e_Vers, 1);

  lcdDrawTextLeft(MENU_HEADER_HEIGHT+FH, vers_stamp);

#if defined(COPROCESSOR)
  if (Coproc_valid == 1) {
     lcdDrawTextLeft(6*FH, PSTR("CoPr:"));
     lcd_outdez8(10*FW, 6*FH, Coproc_read);
  }
  else {
     lcdDrawTextLeft(6*FH, PSTR("CoPr: ---"));
  }
#endif
}
