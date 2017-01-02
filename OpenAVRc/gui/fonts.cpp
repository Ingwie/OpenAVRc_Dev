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


#include "../OpenAVRc.h"

#if defined(FONTSTD)

const pm_uchar font_5x7[] PROGMEM = {
#include "../fonts/std/font_05x07_avr.lbm"
#if defined(TRANSLATIONS_DE)
#include "../fonts/std/font_de_05x07.lbm"
#elif defined(TRANSLATIONS_CZ)
#include "../fonts/std/font_cz_05x07.lbm"
#elif defined(TRANSLATIONS_ES)
#include "../fonts/std/font_es_05x07.lbm"
#elif defined(TRANSLATIONS_FI)
#include "../fonts/std/font_fi_05x07.lbm"
#elif defined(TRANSLATIONS_FR)
#include "../fonts/std/font_fr_05x07.lbm"
#elif defined(TRANSLATIONS_IT)
#include "../fonts/std/font_it_05x07.lbm"
#elif defined(TRANSLATIONS_PL)
#include "../fonts/std/font_pl_05x07.lbm"
#elif defined(TRANSLATIONS_PT)
#include "../fonts/std/font_pt_05x07.lbm"
#elif defined(TRANSLATIONS_SE)
#include "../fonts/std/font_se_05x07.lbm"
#endif
};

#if defined(BOLD_SPECIFIC_FONT)
const pm_uchar font_5x7_B[] PROGMEM = {
#include "../fonts/std/font_05x07_B_compressed.lbm"
};
#endif

const pm_uchar font_10x14[] PROGMEM = {
#include "../fonts/std/font_10x14_compressed.lbm"
};

#else //SQT5

const pm_uchar font_5x7[] PROGMEM = {
#include "../fonts/sqt5/font_05x07_avr.lbm"
#if defined(TRANSLATIONS_DE)
#include "../fonts/sqt5/font_de_05x07.lbm"
#elif defined(TRANSLATIONS_CZ)
#include "../fonts/sqt5/font_cz_05x07.lbm"
#elif defined(TRANSLATIONS_ES)
#include "../fonts/sqt5/font_es_05x07.lbm"
#elif defined(TRANSLATIONS_FI)
#include "../fonts/sqt5/font_fi_05x07.lbm"
#elif defined(TRANSLATIONS_FR)
#include "../fonts/sqt5/font_fr_05x07.lbm"
#elif defined(TRANSLATIONS_IT)
#include "../fonts/sqt5/font_it_05x07.lbm"
#elif defined(TRANSLATIONS_PL)
#include "../fonts/sqt5/font_pl_05x07.lbm"
#elif defined(TRANSLATIONS_PT)
#include "../fonts/sqt5/font_pt_05x07.lbm"
#elif defined(TRANSLATIONS_SE)
#include "../fonts/sqt5/font_se_05x07.lbm"
#endif
};

#if defined(BOLD_SPECIFIC_FONT)
const pm_uchar font_5x7_B[] PROGMEM = {
#include "../fonts/sqt5/font_05x07_B_compressed.lbm"
};
#endif

const pm_uchar font_10x14[] PROGMEM = {
#include "../fonts/sqt5/font_10x14_compressed.lbm"
};


#endif
