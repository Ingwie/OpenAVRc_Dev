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

/*
  "TinyDbg (Tiny DeBuGger)" by RC-Navy, a Tiny Debugger for arduino with breakpoints support
  and variable inspection, based on "debugger" project of Stephen Payne
  Copyright (c) 2019, RC-Navy

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#ifndef TINY_DBG_H_
#define TINY_DBG_H_

#define TINY_DBG_VERSION      0
#define TINY_DBG_REVISION     3

/* vvv Start of User Debugger configuration vvv */

#define TDBG_ACTIVE             // Comment this line to deactivate all debugging stuff
#define TDBG_PERIODIC_DISPLAY   // Comment this line to deactivate periodic display
#define TDBG_MAX_BREAKPOINTS  8 //Define here the max number of breakpoint(s) you need (Max is 16)
#define TDBG_MAX_WATCHES      8 //Define here the max number of watch(es) you need (Max is limited by memory)
//#define TDBG_WITH_MIN_FEATURES  //Comment this line to have full debug capabilities

/* ^^^ End of User Debugger configuration ^^^ */


#ifndef TDBG_ACTIVE
#warning TinyDbg present, but deactivated by commented TDBG_ACTIVE compilation directive in TinyDbg.h!
#endif

#ifdef ARDUINO
#include <Arduino.h>
#define GET_TICK()         millis()
#define MS_TO_TICK(ms)     (ms)
#else
/* OpenAVRc specific */
#include <Stream.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include <util/delay.h>
#include "OpenAVRc.h"

#define bitSet(var, bit)   (var) |=  (1 << (bit))
#define bitClear(var, bit) (var) &= ~(1 << (bit))
#define bitRead(var, bit)  (!!((var) & (1 << (bit))))
#define interrupts()       sei()
#define delay(d)           _delay_ms(d)
#define GET_TICK()         (uint32_t)getTmr10ms()
#define MS_TO_TICK(ms)     MS_TO_10MS_TICK(ms)
#endif

#if defined(ARDUINO_ARCH_AVR)
  // AVR-specific code
#elif defined(ARDUINO_ARCH_SAM)
  #error "not tested with SAM-specific boards"
#else
  // generic, non-platform specific code
#endif

enum {TDBG_VAR_RAW = 0,
      TDBG_VAR_CHAR,
      TDBG_VAR_UINT8,  TDBG_VAR_INT8,
      TDBG_VAR_UINT16, TDBG_VAR_INT16,
      TDBG_VAR_UINT32, TDBG_VAR_INT32,
      TDBG_VAR_FLOAT,
      TDBG_VAR_STRING
     };

#if (TDBG_MAX_BREAKPOINTS > 16)
#error TDBG_MAX_BREAKPOINTS is limited to 16!
#endif

#define FALSE 0
#define TRUE  1

typedef struct {
    char* FunctName;
    void* Address;
    char* Name;
    uint8_t
         Type :4,
         ID   :4;
}TdbWatchVarSt_t;

#ifdef TDBG_ACTIVE
void     TinyDbg_init(Stream *TdbgStream);
void     TinyDbg_attachToStream(Stream *TdbgStream);
void     TinyDbg_event(void);

void     TinyDbg_addWatch(char*  FuncNameStr, void*  var, char*  VarNameStr, uint8_t Type);
uint8_t  TinyDbg_armBreakpoint(uint8_t ID);
void     TinyDbg_isAtBreakpoint(char *FunctName, uint8_t BpId, uint16_t Line);
void     TinyDbg_dumpMem(uint8_t MemLocIdx, uint32_t Start, uint32_t Len);
uint16_t TinyDbg_Printf(const char *fmt, ...);
#define BREAK(BpId) do{TinyDbg_isAtBreakpoint((char *)__PRETTY_FUNCTION__, BpId, __LINE__);}while(0)

#define gWARG(a)    (char*)__FUNCTION__, (void*)&a, (char*)#a /* Generic Watch ARGument */
#define sWARG(a)    (char*)__FUNCTION__, (void*)a,  (char*)#a /* String  Watch ARGument */

#define WATCH(a)     TinyDbg_addWatch(gWARG(a), TDBG_VAR_INT16)

#define i8WATCH(a)   TinyDbg_addWatch(gWARG(a), TDBG_VAR_INT8)
#define u8WATCH(a)   TinyDbg_addWatch(gWARG(a), TDBG_VAR_UINT8)

#define chrWATCH(a)  TinyDbg_addWatch(gWARG(a), TDBG_VAR_CHAR)

#define i16WATCH(a)  TinyDbg_addWatch(gWARG(a), TDBG_VAR_INT16)
#define u16WATCH(a)  TinyDbg_addWatch(gWARG(a), TDBG_VAR_UINT16)

#define i32WATCH(a)  TinyDbg_addWatch(gWARG(a), TDBG_VAR_INT32)
#define u32WATCH(a)  TinyDbg_addWatch(gWARG(a), TDBG_VAR_UINT32)

#define f32WATCH(a)  TinyDbg_addWatch(gWARG(a), TDBG_VAR_FLOAT)

#define strWATCH(a)  TinyDbg_addWatch(sWARG(a), TDBG_VAR_STRING)

#define rawWATCH(a)  TinyDbg_addWatch(gWARG(a), TDBG_VAR_RAW)

#else
/* Tiny Debugger deactived by compilation directive */
#define TinyDbg_init(DbgStream)   do{}while(0)
#define TinyDbg_armBreakpoint(ID) do{}while(0)
#define TinyDbg_event()           do{}while(0)
#define BREAK(BpId)               do{}while(0)

#define WATCH(a)                  do{}while(0)

#define i8WATCH(a)                do{}while(0)
#define u8WATCH(a)                do{}while(0)

#define chrWATCH(a)               do{}while(0)

#define i16WATCH(a)               do{}while(0)
#define u16WATCH(a)               do{}while(0)

#define i32WATCH(a)               do{}while(0)
#define u32WATCH(a)               do{}while(0)

#define f32WATCH(a)               do{}while(0)

#define strWATCH(a)               do{}while(0)

#define rawWATCH(a)               do{}while(0)
#endif

#endif
