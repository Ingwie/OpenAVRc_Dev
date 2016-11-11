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

#ifndef menu_general_h
#define menu_general_h

#include "../nextsteprc.h"

void menuGeneralSetup(uint8_t event);
void menuGeneralSdManager(uint8_t event);
void menuGeneralCustomFunctions(uint8_t event);
void menuGeneralTrainer(uint8_t event);
void menuGeneralVersion(uint8_t event);
void menuGeneralDiagKeys(uint8_t event);
void menuGeneralDiagAna(uint8_t event);
void menuGeneralCalib(uint8_t event);


 enum EnumTabDiag {
  e_Setup,
  CASE_SDCARD(e_Sd)
  e_Trainer,
  e_Vers,
  e_Keys,
  e_Ana,
  e_Calib
};

const MenuFuncP_PROGMEM menuTabGeneral[] PROGMEM = {
  menuGeneralSetup,
  CASE_SDCARD(menuGeneralSdManager)
  menuGeneralTrainer,
  menuGeneralVersion,
  menuGeneralDiagKeys,
  menuGeneralDiagAna,
  menuGeneralCalib
};

const pm_uchar sticks[] PROGMEM = {
#include "../bitmaps/sticks.lbm"
};


#endif
