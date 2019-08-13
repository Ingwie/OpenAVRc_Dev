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

#ifdef ARDUINO
#include "TinyDbg.h"
#else
#include "debug/TinyDbg.h"
#endif
#include <ctype.h>

#ifdef TDBG_ACTIVE

#define BACK_SPACE               0x08
#define TDBG_CMD_LEN             16
#define TDBG_BUFF_SIZE           60

#define TDBG_USER_HALT_BP_ID     100

#define TDBG_FIRST_VAL_COL_POS   60
#define TDBG_SECOND_VAL_COL_POS  (TDBG_FIRST_VAL_COL_POS  + 12)
#define TDBG_THIRD_VAL_COL_POS   (TDBG_SECOND_VAL_COL_POS + 10)
#define TDBG_FOURTH_VAL_COL_POS  (TDBG_THIRD_VAL_COL_POS  + 14)

#define CONCAT_VER_REV(a, b)     #a"."#b
#define VER_REV_STR(v, r)        CONCAT_VER_REV(v, r)

#define htons(x)                 __builtin_bswap16((uint16_t) (x))
#define htonl(x)                 __builtin_bswap32((uint32_t) (x))

#define VAR_MODULO(Var,Modulo)   !((Var)%(Modulo))

#define STR(Str)                 const char Str_##Str     [] PROGMEM = #Str
#define STR2_(StrName, Str)      const char Str_##StrName [] PROGMEM =  Str
#define STR_TBL(StrTbl)          const char * const StrTbl[] PROGMEM

STR2_(voidptr, "void*");
STR(char);
STR(uint8_t);
STR(int8_t);
STR(uint16_t);
STR(int16_t);
STR(uint32_t);
STR(int32_t);
STR(float);
STR2_(charptr, "char*");

STR_TBL(Var_Type) =
{Str_voidptr, Str_char, Str_uint8_t, Str_int8_t, Str_uint16_t, Str_int16_t, Str_uint32_t, Str_int32_t, Str_float, Str_charptr};

enum {RAM_LOC = 0, EEPROM_LOC, FLASH_LOC, MEM_TYPE_NB};

STR(RAM);
STR(EEPROM);
STR(FLASH);

STR_TBL(Mem_Type) = {Str_RAM, Str_EEPROM, Str_FLASH};

enum {ST_FROM_ANY_OTHER = 0, ST_FROM_BREAK, ST_FROM_DB};

static void    displayWatchVariable(char *tmpbuf);
static void    displayStatus(uint8_t FromBreak = 0);
static void    displaySpace(uint8_t SpaceNb);
static void    watchRaw(char *Cmd, uint8_t MemLocIdx);
static void    printByteBin(Stream *stream, uint8_t Byte, uint8_t RemainingNibble = 0);
static void    printBin(Stream *stream, uint8_t *Buf, uint8_t BufSize, uint8_t RemainingNibble = 0);
static char   *getLbl(const char * const *LblTbl, uint8_t LblIdx, char *Lbl, uint8_t LblMaxLen);

static char     Command[TDBG_CMD_LEN];
static uint8_t  index = 0;
#ifdef TDBG_PERIODIC_DISPLAY
static uint8_t  DisplayPeriodMsDiv128 = 0;
#endif
#ifdef TDBG_WITH_MIN_FEATURES

const char TDBG_PROMPT[] PROGMEM = "\nTDBG> ";

const char NL[]          PROGMEM = "\r\n";
const char CMD_NOT_RECOGNIZED[] PROGMEM = "?";

const char RUNNING[]     PROGMEM = "Running...";
const char STOPPED[]     PROGMEM = "Stopped.";

#define STILL_STOPPED    STOPPED
const char BP_NONE[]     PROGMEM = "0";

const char ACTIVE_BREAKPOINTS[]  PROGMEM = "[Act bps: ";
const char BREAKPOINTS_CLEARED[] PROGMEM = "BP Cleared";
const char WATCH_VARS_CLEARED[]  PROGMEM = "";

#else
const char helpText1[]   PROGMEM = "\ndm [addr (l len)]    -> ";
const char helpText2[]   PROGMEM =   "display mem content (m=r->";
const char helpText3[]   PROGMEM =   "RAM,m=e->eEPROM,m=f->fLASH)\n";
const char helpText4[]   PROGMEM =   "bp [breakpoint ID]   -> ";
const char helpText5[]   PROGMEM =   "activate BP(s) with ID\n";

const char helpText6[]   PROGMEM =   "db [breakpoint ID]   -> ";
const char helpText7[]   PROGMEM =   "deactivate BP(s) with ID\n";

const char helpText8[]   PROGMEM =   "cb                   -> ";
const char helpText9[]   PROGMEM =   "clear all BP(s)\n";

const char helpText10[]  PROGMEM =  "ru                   -> ";
const char helpText11[]  PROGMEM =  "run program execution\n";

const char helpText12[]  PROGMEM =  "ha                   -> ";
const char helpText13[]  PROGMEM =  "halt program execution\n";

const char helpText14[]  PROGMEM =  "dv [(p period in ms)]-> ";
const char helpText15[]  PROGMEM =  "display variables\n";

const char helloText1[]  PROGMEM =  "-- Tiny Debugger V" VER_REV_STR(TINY_DBG_VERSION,TINY_DBG_REVISION)"";
const char helloText2[]  PROGMEM =  " started.  h for help. --\n";
const char TDBG_PROMPT[] PROGMEM = "\nTDB> ";

const char NL[] PROGMEM = "\n";
const char CMD_NOT_RECOGNIZED[] PROGMEM = "Command Not Recognized.";

const char RUNNING[]       PROGMEM = "Running...";
const char STOPPED[]       PROGMEM = "Stopped. (by user halt cmd)";
const char STILL_STOPPED[] PROGMEM = "Still stopped... ";
const char BP_NONE[]       PROGMEM = "None";

const char ACTIVE_BREAKPOINTS[]  PROGMEM = " [Active BP(s): ";
const char BREAKPOINTS_CLEARED[] PROGMEM = "All BP Cleared: ";
const char WATCH_VARS_CLEARED[]  PROGMEM = "All Watch Variables Cleared: ";

#endif
typedef struct{
  Stream         *stream;
  const char     *FlashPrePrompt;
  char           *FunctName;
  uint16_t        ArmedBreakPointMap;
  TdbWatchVarSt_t Vars[TDBG_MAX_WATCHES];
  uint16_t        Line;
  uint8_t
                  Stopped        :1,
                  BreakPointAtId :7;
}TdbgSt_t;

TdbgSt_t        Tdbg;

#ifdef TDBG_PERIODIC_DISPLAY
static uint32_t DisplayPeriodStartTick = 0L;
#endif

/**
 * \fn void TinyDbg_init(Stream *TdbgStream, const char *FlashPrePrompt)
 * \brief Tiny Debugger initialization.
 *
 * \param  TdbgStream:     pointer on a stream (eg: Serial).
 * \param  FlashPrePrompt: pointer on a Prompt string (if TinyDbg is used in an existing CLI).
 * \return Void.
 */
void TinyDbg_init(Stream *TdbgStream, const char *FlashPrePrompt/*=NULL*/)
{
  memset((void*)&Tdbg, 0, sizeof(Tdbg));
  Tdbg.stream         = TdbgStream;
  Tdbg.FlashPrePrompt = FlashPrePrompt;
  if(TdbgStream)
  {
#ifndef TDBG_WITH_MIN_FEATURES
    if(!Tdbg.FlashPrePrompt)
    {
      TinyDbg_Printf(helloText1);
      TinyDbg_Printf(helloText2);
    }
#endif
    TinyDbg_Printf(TDBG_PROMPT);
  }
}

/**
 * \fn void TinyDbg_attachToStream(Stream *TdbgStream)
 * \brief Attach or re-attach the Tiny Debugger to a stream.
 *
 * \param  TdbgStream: pointer on a stream (eg: Serial).
 * \return Void.
 */
void TinyDbg_attachToStream(Stream *TdbgStream)
{
  Tdbg.stream = TdbgStream;
}

/**
 * \fn void TinyDbg_Printf(const char *fmt, ...)
 * \brief Printf-like debug function.
 *
 * \param fmt: same parameters as printf().
 * \param ...: variable argument list.
 * \return The length printed.
 */
uint16_t TinyDbg_Printf(const char *fmt, ...)
{
  va_list  args;
  char     DbgBuf[TDBG_BUFF_SIZE];
  uint16_t Len = 0;

  if(Tdbg.stream)
  {
    va_start(args, fmt);
    vsnprintf_P(DbgBuf, TDBG_BUFF_SIZE, fmt, args);
    va_end(args);
    Len = Tdbg.stream->print(DbgBuf);
  }
  return(Len);
}

/**
 * \fn uint8_t TinyDbg_armBreakpoint(uint8_t ID)
 * \brief Arm a breakpoint
 *
 * \param  ID: the breakpoint ID.
 * \return 0: Error, 1: Breakpoint armed
 */
uint8_t TinyDbg_armBreakpoint(uint8_t ID)
{
  uint8_t Ret = 0;
  if(ID && (ID <= TDBG_MAX_BREAKPOINTS))
  {
    bitSet(Tdbg.ArmedBreakPointMap, ID - 1);
    Ret = 1;
  }
  return(Ret);
}

/**
 * \fn void TinyDbg_event(void)
 * \brief Catch command from debug stream (eg: from Serial) and display periodically the variables (if armed)
 *
 * \param  Void
 * \return Void
 */
void TinyDbg_event(void)
{
#ifdef TDBG_PERIODIC_DISPLAY
  char            AutoCommand[3] = {0};
#endif
  char RxChar;

  WDT_RESET();

  if(!Tdbg.stream) return;
  if(Tdbg.Stopped || !Tdbg.FlashPrePrompt) // Catch characters forever if TinyDbg in standalone
  {
    while(Tdbg.stream->available())
    {
      RxChar = (char)Tdbg.stream->read();

      if(index < TDBG_CMD_LEN)
      {
        if((RxChar == BACK_SPACE) && index) index--;
        else
        {
          if (RxChar == '\n' || RxChar == '\r')
          {
            Command[index] = 0; /* Replace CR or LF with end of string */
            index = 0;
  #ifdef TDBG_BASIC_ARDUINO_SERIAL_CONSOLE_SUPPORT
            Tdbg.stream->println(Command); /* Local echo for Arduino Serial terminal */
  #endif
            if(Tdbg.FlashPrePrompt && Tdbg.Stopped) TinyDbg_Printf(Tdbg.FlashPrePrompt);
            TinyDbg_interpretAndExecute(Command);
          }
          else Command[index++] = RxChar; /* Store the character */
        }
      }
      else
      {
        /* Too long! */
        index = 0;
      }
    }
  }
#ifdef TDBG_PERIODIC_DISPLAY
  if(DisplayPeriodMsDiv128)
  {
    if((GET_TICK() - DisplayPeriodStartTick) >= MS_TO_TICK(DisplayPeriodMsDiv128 * 128UL))
    {
      DisplayPeriodStartTick = GET_TICK();
      AutoCommand[0] = 'd';
      AutoCommand[1] = 'v';
      AutoCommand[2] =  0 ;
      TinyDbg_interpretAndExecute(AutoCommand);
    }
  }
#endif

}

/**
 * \fn void TinyDbg_addWatch(char* FuncNameStr, void* var, char* VarNameStr, uint8_t Type)
 * \brief Add a variable watch
 *
 * \param  FuncNameStr: Name of the function where the variable is located
 * \param  var:         Pointer on the variable to watch
 * \param  VarNameStr:  Name of the variable to watch
 * \param  Type:        Type of the variable to watch
 * \return Void
 */
void TinyDbg_addWatch(char* FuncNameStr, void* var, char* VarNameStr, uint8_t Type)
{
  for(uint8_t i = 0; i < TDBG_MAX_WATCHES; i++)
  {
    if(Tdbg.Vars[i].Address == var)
    {
      break; /* Already registered */
    }
    if(Tdbg.Vars[i].Address)
    {
      continue; /* Occupied by another variable */
    }
    /* OK, free -> Register */
    Tdbg.Vars[i].FunctName = FuncNameStr;
    Tdbg.Vars[i].Address   = var;
    Tdbg.Vars[i].Name      = VarNameStr;
    Tdbg.Vars[i].Type      = Type;
    break;
  }
}

/**
 * \fn void TinyDbg_isAtBreakpoint(char *FunctName, uint8_t BpId, uint16_t Line)
 * \brief Software breakpoint
 *
 * \param  FuncName: Name of the function where the software breakpoint is located
 * \param  BpId:     ID of the software breakpoint
 * \param  Line:     Line in the source code where the software breakpoint is located
 * \return Void
 */
void TinyDbg_isAtBreakpoint(char *FunctName, uint8_t BpId, uint16_t Line)
{
  if(bitRead(Tdbg.ArmedBreakPointMap, BpId - 1))
  {
    Tdbg.FunctName      = FunctName;
    Tdbg.Line           = Line;
    Tdbg.BreakPointAtId = BpId;
    Tdbg.Stopped        = 1;
    interrupts();             // enable all interrupts
    displayStatus(ST_FROM_BREAK);
    while(Tdbg.BreakPointAtId)
    {
      TinyDbg_event();
      delay(10);
    }
  }
}

/**
 * \fn void TinyDbg_interpretAndExecute(char *Cmd, const char *FlashPrePrompt)
 * \brief CLI interpretor
 *
 * \param  Cmd: Pointer on a buffer containing the command to interpret and execute
 * \return Void
 */
void TinyDbg_interpretAndExecute(char *Cmd)
{
  char    tmpbuf[TDBG_BUFF_SIZE];
  char    Error = TRUE;
  uint8_t BpIdx;

  if(Tdbg.FlashPrePrompt) TinyDbg_Printf(TDBG_PROMPT + 1);
#ifndef TDBG_WITH_MIN_FEATURES
  /////// Display ram memory ///////////
  if(strstr_P(Cmd, PSTR("dr")))
  {
    watchRaw(Cmd, RAM_LOC);
    Error = FALSE;
  }
  /////// Display eeprom memory ///////////
  if(strstr_P(Cmd, PSTR("de")))
  {
    watchRaw(Cmd, EEPROM_LOC);
    Error = FALSE;
  }
  /////// Display flash memory ///////////
  if(strstr_P(Cmd, PSTR("df")))
  {
    watchRaw(Cmd, FLASH_LOC);
    Error = FALSE;
  }
#endif

  /////// arm breakpoint ///////////
  if(strstr_P(Cmd, PSTR("bp")))
  {
    BpIdx = atoi(&Cmd[2]) - 1;
    if(TinyDbg_armBreakpoint(BpIdx + 1))
    {
      TinyDbg_Printf(PSTR("BP %u armed: "), BpIdx + 1);
    }
    displayStatus();
    Error = FALSE;
    if(Tdbg.Stopped)
    {
      if(!Tdbg.FlashPrePrompt) TinyDbg_Printf(TDBG_PROMPT);
      return;
    }
  }
  /////// dearm breakpoint ///////////
  if(strstr_P(Cmd, PSTR("db")))
  {
    BpIdx = atoi(&Cmd[2]) - 1;
    if(BpIdx < TDBG_MAX_BREAKPOINTS)
    {
      bitClear(Tdbg.ArmedBreakPointMap, BpIdx);
      TinyDbg_Printf(PSTR("BP %u disarmed: Still "), BpIdx + 1);
      displayStatus(ST_FROM_DB);
    }
    else displayStatus();
    Error = FALSE;
    if(!Tdbg.FlashPrePrompt) TinyDbg_Printf(TDBG_PROMPT);
    return;
  }

  /////// clear armed breakpoints ///////////
  if(strstr_P(Cmd, PSTR("cb")))
  {
    Tdbg.ArmedBreakPointMap  = 0;
    TinyDbg_Printf(BREAKPOINTS_CLEARED);
    displayStatus();
    Error = FALSE;
  }

  /////// display variables ///////////
  if(strstr_P(Cmd, PSTR("dv")))
  {
#ifdef TDBG_PERIODIC_DISPLAY
    char *displayPeriodMsPtr = strstr_P(Cmd, PSTR("p"));
    if(displayPeriodMsPtr)
    {
      DisplayPeriodMsDiv128 = atoi(displayPeriodMsPtr + 1) / 128;
    }
#endif
    displayWatchVariable(tmpbuf);
    if(Tdbg.FlashPrePrompt)
    {
      if(Tdbg.Stopped)
      {
        TinyDbg_Printf(NL);
        TinyDbg_Printf(Tdbg.FlashPrePrompt);
        return;
      }
    }
    Error = FALSE;
  }

#ifndef TDBG_WITH_MIN_FEATURES
  /////// halt ///////////
  if(strstr_P(Cmd, PSTR("ha")))
  {
    if(!Tdbg.Stopped)
    {
      Tdbg.Stopped = 1;
      Tdbg.BreakPointAtId = TDBG_USER_HALT_BP_ID;
#ifdef TDBG_PERIODIC_DISPLAY
      DisplayPeriodMsDiv128 = 0; // disable periodic display if armed
#endif
    }
    displayStatus();
    if(!Tdbg.FlashPrePrompt) TinyDbg_Printf(TDBG_PROMPT);
    while(Tdbg.BreakPointAtId)
    {
      TinyDbg_event();
      delay(10);
    }
    Error = FALSE;
    return;
  }
#endif

  /////// run ///////////
  if(strstr_P(Cmd, PSTR("ru")))
  {
    Tdbg.Stopped = 0;
    Tdbg.BreakPointAtId = 0;
    displayStatus();
    Error = FALSE;
  }

    /////// status ///////////
  if(strstr_P(Cmd, PSTR("st")))
  {
    displayStatus();
    Error = FALSE;
    if(Tdbg.Stopped)
    {
      if(Tdbg.FlashPrePrompt) return;
    }
  }

#ifndef TDBG_WITH_MIN_FEATURES
  /////// help ///////////
  if(strstr_P(Cmd, PSTR("h")))
  {
    TinyDbg_Printf(helpText1); TinyDbg_Printf(helpText2);
    TinyDbg_Printf(helpText3); TinyDbg_Printf(helpText4);
    TinyDbg_Printf(helpText5); TinyDbg_Printf(helpText6);
    TinyDbg_Printf(helpText7); TinyDbg_Printf(helpText8);
    TinyDbg_Printf(helpText9); TinyDbg_Printf(helpText10);
    TinyDbg_Printf(helpText11);TinyDbg_Printf(helpText12);
    TinyDbg_Printf(helpText13);TinyDbg_Printf(helpText14);
    TinyDbg_Printf(helpText15);
    Error = FALSE;
  }
#endif

  if(Error)
  {
    TinyDbg_Printf(CMD_NOT_RECOGNIZED);
  }

  if(!Tdbg.FlashPrePrompt) TinyDbg_Printf(TDBG_PROMPT);
  else
  {
    TinyDbg_Printf(NL);
  }
}

/**
 * \fn void displaySpace(uint8_t SpaceNb)
 * \brief Display space(s)
 *
 * \param  SpaceNb: Number of space to display
 * \return Void
 */
static void displaySpace(uint8_t SpaceNb)
{
  while(SpaceNb--) Tdbg.stream->print(' ');
}

/**
 * \fn void watchRaw(uint8_t MemLocIdx)
 * \brief Memory dump function (works for RAM, EEPROM and FLASH).
 *
 * \param  MemLocIdx: Memory location index (see RAM_LOC, EEPROM_LOC, FLASH_LOC).
 * \return Void.
 */
static void watchRaw(char *Cmd, uint8_t MemLocIdx)
{
  uint32_t Start = 0;
  uint32_t Len =   16 * 2UL;

  char *ptrlen = strstr_P(Cmd, PSTR("l"));
  if(ptrlen)
  {
    Len = atoi(ptrlen + 1);
  }

  TinyDbg_Printf(NL);
  Start = strtol(&Cmd[2], NULL, 16);

  TinyDbg_dumpMem(MemLocIdx, Start, Len);
}

/**
 * \fn void displayStatus(uint8_t From)
 * \brief Display the current status of the Tiny Debugger.
 *
 * \param  From: 0: do not come from breakpoint, 1: comes from breakpoint, 2: comes from db command.
 * \return Void.
 */
static void displayStatus(uint8_t From /*= 0*/)
{
  char     tmpbuf[TDBG_BUFF_SIZE];
  uint8_t  Idx, DisabledBreakPointNb = 0;
  uint16_t BpMsk;

  if(Tdbg.Stopped)
  {
  //  if(Tdbg.FlashPrePrompt && (From != ST_FROM_DB)) TinyDbg_Printf(TDBG_PROMPT + 1);
    if(Tdbg.BreakPointAtId == TDBG_USER_HALT_BP_ID)
    {
      TinyDbg_Printf(PSTR("Stopped by user halt cmd "));
    }
    else
    {
      if(Tdbg.FlashPrePrompt && From == ST_FROM_BREAK) TinyDbg_Printf(TDBG_PROMPT + 1);
      TinyDbg_Printf(PSTR("Stopped at BP"));
      TinyDbg_Printf(PSTR("%d in %s line %u"), (uint16_t)Tdbg.BreakPointAtId, Tdbg.FunctName, Tdbg.Line);
    }
  }
  else
  {
    TinyDbg_Printf(RUNNING);
  }
  /* Display active breakpoint(s) */
  TinyDbg_Printf(ACTIVE_BREAKPOINTS);
  for(Idx = 0; Idx < TDBG_MAX_BREAKPOINTS; Idx++)
  {
    if(bitRead(Tdbg.ArmedBreakPointMap, Idx))
    {
      Tdbg.stream->print(Idx + 1);
      BpMsk = 0xFFFF << (Idx + 1);
      if(Tdbg.ArmedBreakPointMap & BpMsk) Tdbg.stream->print(F(", "));
    }
    else DisabledBreakPointNb++;
  }
  if(DisabledBreakPointNb >= TDBG_MAX_BREAKPOINTS) TinyDbg_Printf(BP_NONE);
  Tdbg.stream->print(F("]"));

  if((From == ST_FROM_BREAK) || (From == ST_FROM_DB) || Tdbg.Stopped)
  {
#ifdef TDBG_PERIODIC_DISPLAY
    DisplayPeriodMsDiv128 = 0; // disable periodic display if armed
#endif
    if(From == ST_FROM_BREAK)
    {
      displayWatchVariable(tmpbuf);
      if(!Tdbg.FlashPrePrompt) TinyDbg_Printf(TDBG_PROMPT);
    }
    if(Tdbg.FlashPrePrompt)
    {
      TinyDbg_Printf(NL);
      if(Tdbg.Stopped) TinyDbg_Printf(Tdbg.FlashPrePrompt);
    }
  }
}

/**
 * \fn void displayWatchVariable(char *tmpbuf)
 * \brief Display the variable(s)
 *
 * \param  tmpbuf: Pointer on a temporary buffer
 * \return Void.
 */
static void displayWatchVariable(char *tmpbuf)
{
  uint8_t  DispLen, Byte;
  uint16_t Word, Len;
  uint32_t Dword;
  float    Float;

  for(int i = 0; i < TDBG_MAX_WATCHES; i++)
  {
    DispLen = 0;
    if(Tdbg.Vars[i].Address)
    {
      void *Address;
      if(Tdbg.Vars[i].Type == TDBG_VAR_RAW)  Address = (void*)(*(uint16_t*)(uint16_t)(Tdbg.Vars[i].Address));
      else                              Address = Tdbg.Vars[i].Address;
      DispLen += TinyDbg_Printf(PSTR("\nV%02u:(@0x%04x)"), i + 1, (uint16_t)Address);
      DispLen += TinyDbg_Printf(PSTR("%s()."), Tdbg.Vars[i].FunctName);
      DispLen += TinyDbg_Printf(PSTR("(%s)"),  getLbl(Var_Type, Tdbg.Vars[i].Type, tmpbuf, 10));
      DispLen += TinyDbg_Printf(PSTR("%s: "),  Tdbg.Vars[i].Name);
      if(DispLen < TDBG_FIRST_VAL_COL_POS) displaySpace(TDBG_FIRST_VAL_COL_POS - DispLen);
      switch(Tdbg.Vars[i].Type)
      {
        case TDBG_VAR_INT8:
        Byte = *(uint8_t*)Address;
        DispLen = TDBG_FIRST_VAL_COL_POS + TinyDbg_Printf(PSTR("%d "), (int16_t)Byte);
        if(DispLen < TDBG_SECOND_VAL_COL_POS) displaySpace(TDBG_SECOND_VAL_COL_POS - DispLen);
        DispLen = TDBG_SECOND_VAL_COL_POS + TinyDbg_Printf(PSTR("0x%02x"), (uint16_t)((*(int8_t*)Address)& 0x00FF));
        if(DispLen < TDBG_THIRD_VAL_COL_POS) displaySpace(TDBG_THIRD_VAL_COL_POS - DispLen);
        DispLen = TDBG_THIRD_VAL_COL_POS + TinyDbg_Printf(PSTR("'%c'"), Byte < 127? Byte: '.');
        if(DispLen < TDBG_FOURTH_VAL_COL_POS) displaySpace(TDBG_FOURTH_VAL_COL_POS - DispLen);
        Tdbg.stream->print(F("0b"));
        printByteBin(Tdbg.stream, (uint8_t)Byte);
        continue;

        case TDBG_VAR_UINT8:
        case TDBG_VAR_CHAR:
        Byte = *(uint8_t*)Address;
        DispLen = TDBG_FIRST_VAL_COL_POS + TinyDbg_Printf(PSTR("%u "), (uint16_t)Byte);
        if(DispLen < TDBG_SECOND_VAL_COL_POS) displaySpace(TDBG_SECOND_VAL_COL_POS - DispLen);
        DispLen = TDBG_SECOND_VAL_COL_POS + TinyDbg_Printf(PSTR("0x%02x"), (uint16_t)((*(int8_t*)Address)& 0x00FF));
        if(DispLen < TDBG_THIRD_VAL_COL_POS) displaySpace(TDBG_THIRD_VAL_COL_POS - DispLen);
        DispLen = TDBG_THIRD_VAL_COL_POS + TinyDbg_Printf(PSTR("'%c'"), Byte < 127? Byte: '.');
        if(DispLen < TDBG_FOURTH_VAL_COL_POS) displaySpace(TDBG_FOURTH_VAL_COL_POS - DispLen);
        Tdbg.stream->print(F("0b"));
        printByteBin(Tdbg.stream, Byte);
        continue;

        case TDBG_VAR_INT16:
        Word = *(uint16_t*)Address;
        DispLen = TDBG_FIRST_VAL_COL_POS + TinyDbg_Printf(PSTR("%d "), (int16_t)Word);
        if(DispLen < TDBG_SECOND_VAL_COL_POS) displaySpace(TDBG_SECOND_VAL_COL_POS - DispLen);
        DispLen = TDBG_THIRD_VAL_COL_POS + TinyDbg_Printf(PSTR("0x%04x"), Word);
        if(DispLen < TDBG_FOURTH_VAL_COL_POS) displaySpace(TDBG_FOURTH_VAL_COL_POS - DispLen);
        Word = htons(Word);
        Tdbg.stream->print(F("0b"));
        printBin(Tdbg.stream, (uint8_t*)&Word, 2);
        continue;

        case TDBG_VAR_UINT16:
        Word = *(uint16_t*)Address;
        DispLen = TDBG_FIRST_VAL_COL_POS + TinyDbg_Printf(PSTR("%u "), Word);
        if(DispLen < TDBG_SECOND_VAL_COL_POS) displaySpace(TDBG_SECOND_VAL_COL_POS - DispLen);
        DispLen = TDBG_THIRD_VAL_COL_POS + TinyDbg_Printf(PSTR("0x%04x"), Word);
        if(DispLen < TDBG_FOURTH_VAL_COL_POS) displaySpace(TDBG_FOURTH_VAL_COL_POS - DispLen);
        Word = htons(Word);
        Tdbg.stream->print(F("0b"));
        printBin(Tdbg.stream, (uint8_t*)&Word, 2);
        continue;

        case TDBG_VAR_INT32:
        Dword = *(uint32_t*)Address;
        DispLen = TDBG_FIRST_VAL_COL_POS + TinyDbg_Printf(PSTR("%ld "), (int32_t)Dword);
        if(DispLen < TDBG_SECOND_VAL_COL_POS) displaySpace(TDBG_SECOND_VAL_COL_POS - DispLen);
        DispLen = TDBG_THIRD_VAL_COL_POS + TinyDbg_Printf(PSTR("0x%08lx"), Dword);
        if(DispLen < TDBG_FOURTH_VAL_COL_POS) displaySpace(TDBG_FOURTH_VAL_COL_POS - DispLen);
        Dword = htonl(Dword);
        Tdbg.stream->print(F("0b"));
        printBin(Tdbg.stream, (uint8_t*)&Dword, 4);
        continue;

        case TDBG_VAR_UINT32:
        Dword = *(uint32_t*)Address;
        DispLen = TDBG_FIRST_VAL_COL_POS + TinyDbg_Printf(PSTR("%lu "), Dword);
        if(DispLen < TDBG_SECOND_VAL_COL_POS) displaySpace(TDBG_SECOND_VAL_COL_POS - DispLen);
        DispLen = TDBG_THIRD_VAL_COL_POS + TinyDbg_Printf(PSTR("0x%08lx"), Dword);
        if(DispLen < TDBG_FOURTH_VAL_COL_POS) displaySpace(TDBG_FOURTH_VAL_COL_POS - DispLen);
        Dword = htonl(Dword);
        Tdbg.stream->print(F("0b"));
        printBin(Tdbg.stream, (uint8_t*)&Dword, 4);
        continue;

        case TDBG_VAR_FLOAT:
        Float= *(float*)Address;
        if((Float > -1.0) && (Float < 1.0)) dtostrf(Float, -12, 10, tmpbuf); /* - to be left justified */
        else                                dtostrf(Float, -12,  8, tmpbuf); /* - to be left justified */
        DispLen = TDBG_FIRST_VAL_COL_POS + Tdbg.stream->print(tmpbuf);
        continue;

        case TDBG_VAR_STRING:
        Len = strlen((char*)Address);
        Tdbg.stream->print(F("'"));
        Tdbg.stream->write((char*)Address, Len); /* String may be longer than buffer size -> use write(buf, len) */
        TinyDbg_Printf(PSTR("' (Len = %u)"), Len);
        continue;

        case TDBG_VAR_RAW:
        sprintf_P(Command, PSTR("dr 0x%x"), (uint16_t)Address);
        watchRaw(Command, RAM_LOC);
      }
    }
  }
}

/**
 * \fn void printByteBin(Stream *stream, uint8_t Byte, uint8_t RemainingNibble)
 * \brief Display a byte in binary.
 *
 * \param  stream: Pointer on a stream (eg: Serial)
 * \param  Byte:   The byte
 * \param  RemainingNibble: 0: no remaining nibble, 1: remaining nibble
 * \return Void.
 */
static void printByteBin(Stream *stream, uint8_t Byte, uint8_t RemainingNibble/* = 0*/)
{
  for(uint8_t Idx = 0; Idx < 8; Idx++)
  {
    stream->print(bitRead(Byte, 7 - Idx));
    if(Idx == 3)
    {
      if(RemainingNibble) break;
      stream->print(F("."));
    }
  }
}

/**
 * \fn void printBin(Stream *stream, uint8_t *Buf, uint8_t BufSize, uint8_t RemainingNibble)
 * \brief Display a buffer in binary.
 *
 * \param  stream:  Pointer on a stream (eg: Serial)
 * \param  Buf:     Pointer on the buffer
 * \param  BufSize: Size of the buffer
 * \param  RemainingNibble: 0: no remaining nibble, 1: remaining nibble
 * \return Void.
 */
static void printBin(Stream *stream, uint8_t *Buf, uint8_t BufSize, uint8_t RemainingNibble/* = 0*/)
{
  uint8_t ByteIdx;
  for(ByteIdx =0; ByteIdx < BufSize; ByteIdx++)
  {
    if(ByteIdx) stream->print(F("."));
    printByteBin(stream, Buf[ByteIdx]);
  }
  if(RemainingNibble)
  {
    stream->print(F("."));
    printByteBin(stream, Buf[ByteIdx], RemainingNibble);
  }
}

/**
 * \fn char *getLbl(const char * const *LblTbl, uint8_t LblIdx, char *Lbl, uint8_t LblMaxLen)
 * \brief Get a label from a PROGMEM table.
 *
 * \param  LblTbl:    Pointer on the PROGMEM label table
 * \param  Lbl:       Pointer on a destination buffer
 * \param  LblMaxLen: Maximum size of the destination buffer
 * \return Pointer on the Lbl.
 */
static char *getLbl(const char * const *LblTbl, uint8_t LblIdx, char *Lbl, uint8_t LblMaxLen)
{

  Lbl[0] = 0; /* Empty String */
  if(LblTbl)
  {
    strncpy_P(Lbl, (char*)pgm_read_word(&LblTbl[LblIdx]), LblMaxLen);
  }
  return(Lbl);
}

/**
 * \fn void TinyDbg_dumpMem(uint8_t MemLocIdx, uint32_t Start, uint32_t Len)
 * \brief Memory dump function (works for RAM, EEPROM and FLASH).
 *
 * \param  MemLocIdx: Memory location index (see RAM_LOC, EEPROM_LOC, FLASH_LOC).
 * \param  Start:Start address in the selected memory.
 * \param  Len: Length to dump.
 * \return Void.
 */
void TinyDbg_dumpMem(uint8_t MemLocIdx, uint32_t Start, uint32_t Len)
{
  uint32_t Idx, LineIdx = 0;
  uint8_t  AscIdx;
  uint8_t  Byte;
  char    *BytePtr = NULL;
  char     TypeStr[10];
  char     MemoByteLine[16];

  if(MemLocIdx < MEM_TYPE_NB)
  {
    Start = (Start / 16L) * 16L;
    Len = ((Len + 15L) / 16L) * 16L;
    switch(MemLocIdx)
    {
      case RAM_LOC:
      if(Len > 8192L) Len = 8192L; /* To do: #ifdef with uC type */
      break;

      case EEPROM_LOC:
      if(Len > 4096L) Len = 4096L; /* To do: #ifdef with uC type */
      break;

      case FLASH_LOC:
      if(Len > (256L * 1024L)) Len = (256L * 1024L); /* To do: #ifdef with uC type */
      break;
    }
    if(MemLocIdx == RAM_LOC)
    {
      BytePtr = (char *)Start;
    }
    TinyDbg_Printf(PSTR("%s dump (from 0x%5.5lX "), getLbl(Mem_Type, MemLocIdx, TypeStr, 10), Start);
    TinyDbg_Printf(PSTR("to 0x%5.5lX: Len=%u):\n"), Start + Len - 1, Len);
    TinyDbg_Printf(PSTR("ADDR     0  1  2  3  4  5  6 "));
    TinyDbg_Printf(PSTR(  " 7  8  9  A  B  C  D  E  F"));
    TinyDbg_Printf(PSTR("    0123456789ABCDEF\n0x%4.4X "), Start / 16L);
    for(Idx = 0; Idx < Len; Idx++)
    {
      switch(MemLocIdx)
      {
        case RAM_LOC:
        default:
        Byte = BytePtr[Idx];
        break;
        case EEPROM_LOC:
        Byte = eeprom_read_byte((const uint8_t *)(Start + Idx));
        break;

        case FLASH_LOC:
#ifdef pgm_read_byte_far
        Byte = pgm_read_byte_far((Start + Idx));
#else
        Byte = pgm_read_byte((Start + Idx));
#endif
        break;
      }
      TinyDbg_Printf(PSTR(" %2.2X"), (int)Byte );
      MemoByteLine[Idx % 16L] = Byte;
      if(VAR_MODULO(Idx + 1, 16))
      {
        TinyDbg_Printf(PSTR("    "));
        for(AscIdx = 0; AscIdx < 16; AscIdx++)
        {
          Byte = MemoByteLine[AscIdx];
          if(Byte >= 0x20 && Byte < 128)
            TinyDbg_Printf(PSTR("%c"),(char)Byte);
          else
            TinyDbg_Printf(PSTR("."));
        }
        if((Idx + 1) < Len)
        {
          TinyDbg_Printf(PSTR(" \n0x%4.4X "), (Start / 16L) + (++LineIdx));
        }
      }
    }
    TinyDbg_Printf(NL);
  }
}

#endif //TDBG_ACTIVE
