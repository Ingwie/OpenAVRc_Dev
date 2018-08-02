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


//

#ifndef _COMMON_H_
#define _COMMON_H_

#include "../OpenAVRc.h"

// For Multiplex M-Link & PPM
#define CHAN_MAX_VALUE (1520L)
#define CHAN_MIN_VALUE (-CHAN_MAX_VALUE)
#define DELTA_PPM_IN 550 // 500 for Most systems or 550 for Multiplex. e.g. 1500 +/- 550 us
//#define CHAN_MAX_VALUE (+1520 * 7)
//#define CHAN_MIN_VALUE (-1520 * 7) // Range of -1521 to +1520.



#define COUNTS_PER_MILLI_SEC (F_CPU/128000L)-1 // value should == 1ms based on F_CPU.


#endif // _COMMON_H_
