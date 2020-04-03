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

#ifndef UCLI_H
#define UCLI_H

#if defined(SDCARD)
#include "sdcard.h"
#endif
#if defined(TINY_DBG_UART_BT)
#include "debug/TinyDbg.h"
#endif
#if defined(XMODEM)
#include "xmodem/xmodem.h"
#endif

#define BACK_SPACE         0x08
#define CMD_LINE_MAX_SIZE  50  // Will be optimized later

enum {CONTEXT_UCLI = 0, CONTEXT_XMODEM, CONTEXT_BT, CONTEXT_PUPPY};

typedef struct{
  char    Msg[CMD_LINE_MAX_SIZE + 1];
  uint8_t Idx;
}CmdLineSt_t;

typedef struct{
  uint8_t Context; // Will see if really needed, later...
  CmdLineSt_t CmdLine;
}uCliSt_t;

extern uCliSt_t uCli; // uCli structure will be used by bluetooth as well (share buffer)

void uCli_init();
void uCli_process(void);
void uCliFlushRx();

#endif // UCLI_H
