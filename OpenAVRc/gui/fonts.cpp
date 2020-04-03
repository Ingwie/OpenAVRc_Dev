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

#if defined(FONTSTD)

const uint8_t font_5x7_in_sram[] = {
#if defined (LCDROT180)
#include "../fonts/std/font_05x07_avr.lbmi"
#if defined(TRANSLATIONS_DE)
#include "../fonts/std/font_de_05x07.lbmi"
#elif defined(TRANSLATIONS_CZ)
#include "../fonts/std/font_cz_05x07.lbmi"
#elif defined(TRANSLATIONS_ES)
#include "../fonts/std/font_es_05x07.lbmi"
#elif defined(TRANSLATIONS_FI)
#include "../fonts/std/font_fi_05x07.lbmi"
#elif defined(TRANSLATIONS_FR)
#include "../fonts/std/font_fr_05x07.lbmi"
#elif defined(TRANSLATIONS_IT)
#include "../fonts/std/font_it_05x07.lbmi"
#elif defined(TRANSLATIONS_PL)
#include "../fonts/std/font_pl_05x07.lbmi"
#elif defined(TRANSLATIONS_PT)
#include "../fonts/std/font_pt_05x07.lbmi"
#elif defined(TRANSLATIONS_SE)
#include "../fonts/std/font_se_05x07.lbmi"
#endif
#else
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
#endif
};

#if defined(BOLD_SPECIFIC_FONT)
const pm_uchar font_5x7_B_in_flash[] PROGMEM = {
#if defined (LCDROT180)
#include "../fonts/std/font_05x07_B_compressed.lbmi"
#else
#include "../fonts/std/font_05x07_B_compressed.lbm"
#endif
};
#endif

const uint8_t font_10x14_in_sram[] = {
#if defined (LCDROT180)
#include "../fonts/std/font_10x14_compressed.lbmi"
#else
#include "../fonts/std/font_10x14_compressed.lbm"
#endif
};

#else //SQT5

const uint8_t font_5x7_in_sram[] = {
#if defined (LCDROT180)
#include "../fonts/sqt5/font_05x07_avr.lbmi"
#if defined(TRANSLATIONS_DE)
#include "../fonts/sqt5/font_de_05x07.lbmi"
#elif defined(TRANSLATIONS_CZ)
#include "../fonts/sqt5/font_cz_05x07.lbmi"
#elif defined(TRANSLATIONS_ES)
#include "../fonts/sqt5/font_es_05x07.lbmi"
#elif defined(TRANSLATIONS_FI)
#include "../fonts/sqt5/font_fi_05x07.lbmi"
#elif defined(TRANSLATIONS_FR)
#include "../fonts/sqt5/font_fr_05x07.lbmi"
#elif defined(TRANSLATIONS_IT)
#include "../fonts/sqt5/font_it_05x07.lbmi"
#elif defined(TRANSLATIONS_PL)
#include "../fonts/sqt5/font_pl_05x07.lbmi"
#elif defined(TRANSLATIONS_PT)
#include "../fonts/sqt5/font_pt_05x07.lbmi"
#elif defined(TRANSLATIONS_SE)
#include "../fonts/sqt5/font_se_05x07.lbmi"
#endif
#else
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
#endif
};

#if defined(BOLD_SPECIFIC_FONT)
const pm_uchar font_5x7_B_in_flash[] PROGMEM = {
#if defined (LCDROT180)
#include "../fonts/sqt5/font_05x07_B_compressed.lbmi"
#else
#include "../fonts/sqt5/font_05x07_B_compressed.lbm"
#endif
};
#endif

const uint8_t font_10x14_in_sram[] = {
#if defined (LCDROT180)
#include "../fonts/sqt5/font_10x14_compressed.lbmi"
#else
#include "../fonts/sqt5/font_10x14_compressed.lbm"
#endif
};

#endif
