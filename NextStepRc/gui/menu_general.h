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

extern void menuGeneralSetup(uint8_t event);
extern void menuGeneralSdManager(uint8_t event);
extern void menuGeneralCustomFunctions(uint8_t event);
extern void menuGeneralTrainer(uint8_t event);
extern void menuGeneralVersion(uint8_t event);
extern void menuGeneralDiagKeys(uint8_t event);
extern void menuGeneralDiagAna(uint8_t event);
extern void menuGeneralCalib(uint8_t event);



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
