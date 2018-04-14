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

#endif // MISCLIB_H

