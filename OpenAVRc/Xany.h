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
#ifndef X_ANY_H
#define X_ANY_H

#include "OpenAVRc.h"

/* Possible operation for Xany_operation() */
#define XANY_OP_BUILD_MSG    (1 << 0)
#define XANY_OP_READ_INFO    (1 << 1)

typedef struct{
    uint8_t  MsgNibbleLen;
    uint8_t  SwNb;
    uint16_t SwValue;
    uint16_t Angle;
    uint8_t  RotPotValue;
    uint16_t TxPeriodMs;
}XanyInfoSt_t;

void     Xany_init(void);
uint8_t  Xany_readInputsAndLoadMsg(uint8_t XanyIdx);
void     Xany_scheduleTx(uint8_t XanyIdx);
uint8_t  Xany_operation(uint8_t XanyIdx, uint8_t XanyOp, XanyInfoSt_t *XanyInfo);

#endif // X_ANY_H
