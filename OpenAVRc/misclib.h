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

#ifndef MISCLIB_H
#define MISCLIB_H

#ifdef SIMU
#include "targets/simu/simu_interface.h" // for PROGMEM
#endif


/**
* \file  misclib.h
* \fn    Macro: EVERY_PERIOD_WITH_OFFSET(Var, PowerOf2, Offset)
* \brief Check if a variable reaches a periodic specific value with an offset
* \param Var:      variable to check
* \param PowerOf2: period (SHALL be a power of 2)
* \param Offset:   offset
*/
#define EVERY_PERIOD_WITH_OFFSET(Var, PowerOf2, Offset)     (((Var) & ((PowerOf2) - 1)) == (Offset))

/**
* \file  misclib.h
* \fn    Macro: EVERY_PERIOD(Var, PowerOf2)
* \brief Check if a variable reaches a periodic specific value
* \param Var:      variable to check
* \param PowerOf2: period (SHALL be a power of 2)
*/
#define EVERY_PERIOD(Var, PowerOf2)                         EVERY_PERIOD_WITH_OFFSET((Var), (PowerOf2), 0)

/**
* \file  misclib.h
* \fn    Macro: DECL_STR2(StrName, Str)
* \brief Declare a string in flash memory
* \param StrName:  Name of the string in memory
* \param Str:      The string
*/
#define DECL_FLASH_STR2(StrName, Str)                        const char StrName [] PROGMEM =  Str
DECL_FLASH_STR2(Str_CRLF,        "\r\n"); // used everywhere

/**
* \file  misclib.h
* \fn    Macro: DECL_FLASH_TBL(TblName, TblType)
* \brief Declare a table of specified type in flash memory
* \param TblName:  Name of the table
* \param TblType:  Type of item in the table (uint8_t, char*, struct, etc)
*/
#define DECL_FLASH_TBL(TblName, TblType)                     const TblType TblName [] PROGMEM

/**
* \file  misclib.h
* \fn    Macro: TBL_ITEM_NB(Tbl)
* \brief returns the number of item of a table
* \param Tbl:  Name of the table
*/
#define TBL_ITEM_NB(Tbl)                                     (sizeof(Tbl) / sizeof(Tbl[0]))


/**
* \file  misclib.h
* \fn    Macro: BIN_NBL_TO_HEX_DIGIT(BinNbl)
* \brief returns the ASCII Hexa digit corresponding to a nibble value (eg: BIN_NBL_TO_HEX_DIGIT(15) -> 'F'
* \param BinNbl:  The Nibble value (0 to 15)
*/
#define BIN_NBL_TO_HEX_DIGIT(BinNbl)      ((BinNbl) < 10) ? ((BinNbl) + '0'): ((BinNbl) - 10 + 'A')

/**
* \file  misclib.hs
* \fn    Macro: HEX_DIGIT_TO_BIN_NBL(HexDigit)
* \brief returns the nibble value corresponding to an ASCII Hexa digit (eg: HEX_DIGIT_TO_BIN_NBL('F') -> 15
* \param HexDigit:  The ASCII Hexa digit ('0' to '9' and 'A' to 'F') (Uppercase)
*/
#define HEX_DIGIT_TO_BIN_NBL(HexDigit)    ((HexDigit) < 'A') ? (HexDigit - '0'): ((HexDigit) - 'A' + 10)

#define HTONS(x)                 __builtin_bswap16((uint16_t) (x))
#define HTONL(x)                 __builtin_bswap32((uint32_t) (x))

#endif // MISCLIB_H
