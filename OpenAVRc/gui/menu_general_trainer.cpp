 /*
 **************************************************************************
 *                                                                        *
 *                 ____                ___ _   _____                      *
 *                / __ \___  ___ ___  / _ | | / / _ \____                 *
 *               / /_/ / _ \/ -_) _ \/ __ | |/ / , _/ __/                 *
 *               \____/ .__/\__/_//_/_/ |_|___/_/|_|\__/                  *
 *                   /_/                                                  *
 *                                                                        *
 *              This file is part of the OpenAVRc project.                *
 *                                                                        *
 *                         Based on code(s) named :                       *
 *             OpenTx - https://github.com/opentx/opentx                  *
 *             Deviation - https://www.deviationtx.com/                   *
 *                                                                        *
 *                Only AVR code here for visibility ;-)                   *
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


#include "../OpenAVRc.h"
#include "menu_general.h"


#define TRAINER_CALIB_POS 8

const pm_uchar zz_ppm[] PROGMEM =
{
#if defined (LCDROT180)
#include "../bitmaps/ppm.lbmi"
#else
#include "../bitmaps/ppm.lbm"
#endif
};

void menuGeneralTrainer(uint8_t event)
{
  uint8_t slave = IS_WAIT_PUPIL_STATE();

  MENU(STR_MENUTRAINER, menuTabGeneral, e_Trainer, (slave ? 1 : 7), {0, 2, 2, 2, 2, 0/*, 0*/});

  if (slave) {
    lcdDrawText(7*FW, 4*FH, STR_SLAVE);
  } else {
    uint8_t attr;
    uint8_t blink = ((s_editMode>0) ? BLINK|INVERS : INVERS);

    lcdDrawText(3*FW, MENU_HEADER_HEIGHT+1, STR_MODESRC);

    uint8_t y = MENU_HEADER_HEIGHT + 1 + FH;

    for (uint8_t i=1; i<=NUM_STICKS; i++) {
      uint8_t chan = channel_order(i);
      volatile TrainerMix *td = &g_eeGeneral.trainer.mix[chan-1];

      putsMixerSource(0, y, MIXSRC_Rud-1+chan, (menuVerticalPosition==i && CURSOR_ON_LINE()) ? INVERS : 0);

      for (uint8_t j=0; j<3; j++) {

        attr = ((menuVerticalPosition==i && menuHorizontalPosition==j) ? blink : 0);

        switch(j) {
        case 0:
          lcdDrawTextAtIndex(4*FW, y, STR_TRNMODE, td->mode, attr);
          if (attr&BLINK) CHECK_INCDEC_GENVAR(event, td->mode, 0, 2);
          break;

        case 1:
          lcdDrawNumberNAtt(11*FW, y, td->studWeight, attr);
          if (attr&BLINK) CHECK_INCDEC_GENVAR(event, td->studWeight, -125, 125);
          break;

        case 2:
          lcdDrawTextAtIndex(12*FW, y, STR_TRNCHN, td->srcChn, attr);
          if (attr&BLINK) CHECK_INCDEC_GENVAR(event, td->srcChn, 0, 3);
          break;
        }
      }
      y += FH;
    }

    attr = (menuVerticalPosition==5) ? blink : 0;
    lcdDrawTextLeft(MENU_HEADER_HEIGHT+1+5*FH, STR_MULTIPLIER);
    lcdDrawNumberNAtt(LEN_MULTIPLIER*FW+3*FW, MENU_HEADER_HEIGHT+1+5*FH, g_eeGeneral.PPM_Multiplier+10, attr|PREC1);
    if (attr) CHECK_INCDEC_GENVAR(event, g_eeGeneral.PPM_Multiplier, -10, 40);

    attr = (menuVerticalPosition==6) ? INVERS : 0;
    if (attr) s_editMode = 0;
    lcdDrawTextAtt(0*FW, MENU_HEADER_HEIGHT+1+6*FH, STR_CAL, attr);
    for (uint8_t i=0; i<4; i++) {
      uint8_t x = (i*TRAINER_CALIB_POS+16)*FW/2;
#if defined (PPM_UNIT_PERCENT_PREC1)
      lcdDrawNumberNAtt(x, MENU_HEADER_HEIGHT+1+6*FH, (ppmInput[i]-g_eeGeneral.trainer.calib[i])*2, PREC1);
#else
      lcdDrawNumberNAtt(x, MENU_HEADER_HEIGHT+1+6*FH, (ppmInput[i]-g_eeGeneral.trainer.calib[i])/5, 0);
#endif
    }

    if (attr) {
      if (event==EVT_KEY_LONG(KEY_ENTER)) {
        memcpy(g_eeGeneral.trainer.calib, ppmInput, sizeof(g_eeGeneral.trainer.calib));
        eeDirty(EE_GENERAL);
        AUDIO_WARNING1();
      }
    }
  if IS_TRAINER_INPUT_VALID()
   {
     if (systemBolls.puppyPpmSignalOk)
     {
      lcd_imgfar(19*FW, 3*FH, (pgm_get_far_address(zz_ppm)), 0, BLINK); // This is a PPM valid signal
     }
#if defined(BLUETOOTH)
#include "../bluetooth.h"
     else
     {
      lcd_imgfar(19*FW, 3*FH, (pgm_get_far_address(zz_bt)), 0, BLINK); // This is a BT valid signal
     }
#endif
    lcdDrawRect(112, 22, 13, 17, SOLID, ROUND);
   }
 }
}
