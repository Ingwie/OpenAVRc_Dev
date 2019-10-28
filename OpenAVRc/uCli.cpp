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
#include "uCli.h"

#ifdef SIMU
#include "OpenAVRc.h"
#endif

enum {FILE_MEDIA_UNKNOWN = 0, FILE_MEDIA_SD, FILE_MEDIA_XMODEM, FILE_MEDIA_EEPROM};
enum {CONTEXT_UCLI = 0, CONTEXT_XMODEM, CONTEXT_BT};

typedef struct{
  uint8_t
              Src   :2,
              Dst   :2,
              Unused:4;
}FileMediaSt_t;

typedef int8_t (*uCliCmdFunct) (const char ** argv, uint8_t argc);

uCliSt_t uCli;

typedef struct{
  const char  *Name;
  const char  *Help;
  uCliCmdFunct Funct;
}uCliCmdSt_t;

#define UCLI_CMD_MAX_ARG_NB_MAX            4

DECL_FLASH_STR2(UCLI_PROMPT, "uCLI>");
DECL_FLASH_STR2(SD_MEDIA,    "SD/");
DECL_FLASH_STR2(xmdm_MEDIA,  "xmdm"); // XMODEM
DECL_FLASH_STR2(eep_MEDIA,   "eep");  // eep/fram -> rename it NVM for Non-Volatile Memory?

#define UCLI_DEF(Cmd,CmdHelp)              const char uCli_##Cmd        [] PROGMEM = #Cmd;\
                                           const char uCli_##Cmd##_Help [] PROGMEM = #CmdHelp;\
                                           static int8_t uCli_Cmd_##Cmd(const char ** argv, uint8_t argc)
UCLI_DEF(help,  [cmd]);
UCLI_DEF(ls,    [directory]);
UCLI_DEF(cp,    srcfile dstfile);
UCLI_DEF(rmdir, directory);
UCLI_DEF(rm,    file);
UCLI_DEF(mv,    srcfile dstfile);
UCLI_DEF(bt,    on|off|master|slave|state|pin|name);
UCLI_DEF(tf,    ch1-ch8:Chks); //Trainer Frame
UCLI_DEF(ram,    );
UCLI_DEF(reboot, );

#define UCLI_CMD(Cmd)                      {uCli_##Cmd, uCli_##Cmd##_Help, uCli_Cmd_##Cmd}
#define UCLI_CMD_TBL(uCliCmdTbl)           DECL_FLASH_TBL(uCliCmdTbl, uCliCmdSt_t)

UCLI_CMD_TBL(uCliCmd) = { UCLI_CMD(help),
                          UCLI_CMD(ls),
                          UCLI_CMD(cp),
                          UCLI_CMD(rmdir),
                          UCLI_CMD(rm),
                          UCLI_CMD(mv),
                          UCLI_CMD(bt),
                          UCLI_CMD(tf),//tfsHHHsHHH...sHHHsHHH<Chks> (2 + 4 x 16 + 2) -> L = 36 bytes (3ms@115200)
                          UCLI_CMD(ram),
                          UCLI_CMD(reboot),
                        };

static void   uCliPrompt(void);
static int8_t getCliCmdIdx(const char *Cmd);
static int8_t execCmdLine(char * CmdLine);

void uCli_init(Stream *stream)
{
  uCli.stream = stream;
  memset(&uCli.CmdLine, 0, sizeof(uCli.CmdLine));
  uCli.Context = CONTEXT_UCLI;
  uCliPrompt();
}

void uCli_process(void)
{
  char RxChar;

  if(uCli.Context == CONTEXT_UCLI)
  {
    if(uCli.stream->available() >  0)
    {
      RxChar = uCli.stream->read();
      switch(RxChar)
      {
        case 0x0D:
        case 0x0A:
        uCli.CmdLine.Msg[uCli.CmdLine.Idx] = 0;
        uCli.CmdLine.Idx = 0;
        uCliPrompt();
        if(execCmdLine(uCli.CmdLine.Msg) == -1)
        {
#if defined(TINY_DBG_UART_BT)
          TinyDbg_interpretAndExecute(uCli.CmdLine.Msg);
#else
          uCli.stream->println(F("err: unknown cmd"));
#endif
        }
        uCliPrompt();
        break;

        case BACK_SPACE:
        if(uCli.CmdLine.Idx) uCli.CmdLine.Idx--;
        break;

        default:
        if(uCli.CmdLine.Idx < CMD_LINE_MAX_SIZE)
        {
          uCli.CmdLine.Msg[uCli.CmdLine.Idx++] = RxChar;
        }
        else uCli.CmdLine.Idx = 0; //msg too long!
        break;
      }
    }
  }
}

static void uCliPrompt(void)
{
  char Buf[10];
  strcpy_P(Buf, (char *)&UCLI_PROMPT);
  uCli.stream->print(Buf);
}

static int8_t execCmdLine(char *CmdLine)
{
  uint8_t      len  = strlen(CmdLine);
  uint8_t      argc = 1;
  int8_t       CmdIdx;
  const char  *argv[UCLI_CMD_MAX_ARG_NB_MAX];
  uCliCmdFunct CmdFunct;

  if(*CmdLine)
  {
    /* Parse cli argument(s) */
    memset(argv, 0, sizeof(argv));
    argv[0] = CmdLine;
    for(uint8_t i = 0; i < len; i++)
    {
      if(CmdLine[i] == ' ')
      {
        CmdLine[i] = 0;
        if(argc < UCLI_CMD_MAX_ARG_NB_MAX)
        {
          argv[argc++] = &CmdLine[i + 1];
        }
      }
    }
    /* Search in cmd table and execute it if found */
    CmdIdx = getCliCmdIdx(argv[0]);
    if(CmdIdx >= 0)
    {
        CmdFunct = (uCliCmdFunct)pgm_read_word(&uCliCmd[CmdIdx].Funct);
        return(CmdFunct(argv, argc));
    }
  }

  return -1;
}

static int8_t getCliCmdIdx(const char *Cmd)
{
  for(uint8_t Idx = 0; Idx < TBL_ITEM_NB(uCliCmd); Idx++)
  {
    if(!strcmp_P(Cmd, (char*)pgm_read_word(&uCliCmd[Idx].Name)))
    {
      return(Idx);
    }
  }

  return(-1);
}

static int8_t uCli_Cmd_help(const char ** argv, uint8_t argc)
{
  char   Buf[50];
  int8_t CmdIdx;
  int8_t Ret = 0;
  argv = argv;
  argc = argc;

  if(argc == 1)
  {
    uCli.stream->println(F("help: this help"));
    for(uint8_t Idx = 1; Idx < TBL_ITEM_NB(uCliCmd); Idx++)
    {
      strcpy_P(Buf, (char *)pgm_read_word(&uCliCmd[Idx].Name));
      uCli.stream->print(Buf);uCli.stream->print(F(" "));
      strcpy_P(Buf, (char *)pgm_read_word(&uCliCmd[Idx].Help));
      uCli.stream->print(Buf);uCli.stream->println();
    }
  }
  else if(argc == 2)
  {
    CmdIdx = getCliCmdIdx(argv[1]);
    if(CmdIdx >= 0)
    {
      strcpy_P(Buf, (char *)pgm_read_word(&uCliCmd[CmdIdx].Name));
      uCli.stream->print(Buf);uCli.stream->print(F(" "));
      strcpy_P(Buf, (char *)pgm_read_word(&uCliCmd[CmdIdx].Help));
      uCli.stream->print(Buf);uCli.stream->println();
    }
  }
  else
  {
    Ret = -1;
  }

  return(Ret);
}

static int8_t uCli_Cmd_ls(const char ** argv, uint8_t argc)
{
    // we must close the logs as we reuse the same SDfile structure
  closeLogIfActived();
  uCli.stream->println(F("ls:"));
  if (argc > 1)
    {
      if (sdChangeCurDir(argv[1]))
        {
          while(fat_read_dir(SD_dir, &SD_dir_entry))
            {
              if (SD_dir_entry.long_name[0] != '.') // Ignore "." && ".."
                {
                  if (SD_dir_entry.attributes & FAT_ATTRIB_DIR)
                    {
                      // this is a DIR
                      uCli.stream->print('[');
                      uCli.stream->print(SD_dir_entry.long_name);
                      uCli.stream->println(']');
                    }
                  else
                    {
                      // this is a file
                      uCli.stream->println(SD_dir_entry.long_name);
                    }
                }
            }
        }
    }

  return(0);
}

/*
cp src dst
cp SD/toto.txt xmdm        -> X-Modem in Send    mode
cp xmdm SD/titi.cfg        -> X-Modem in Receive mode
cp SD/toto.txt SD/titi.cfg -> after copy both files are available in the SD
cp xmdm xmdm               -> not allowed!
*/
static int8_t uCli_Cmd_cp(const char ** argv, uint8_t argc)
{
  FileMediaSt_t FileMedia;
  const char   *FileName = NULL;

  if(argc != 3) return(-1);
  FileMedia.Src = FILE_MEDIA_UNKNOWN;
  FileMedia.Dst = FILE_MEDIA_UNKNOWN;
  /* Check for source file */
  if(!memcmp_P(argv[1], SD_MEDIA, 3))
  {
    FileMedia.Src = FILE_MEDIA_SD;
    FileName = (const char*)&argv[1][2]; // Just skip SD prefix (shall points on '/')
  }
#if defined(XMODEM)
  else if(!strcmp_P(argv[1], xmdm_MEDIA))
  {
    FileMedia.Src = FILE_MEDIA_XMODEM;
  }
#endif
  /* Check for destination file */
  if(!memcmp_P(argv[2], SD_MEDIA, 3))
  {
    FileMedia.Dst = FILE_MEDIA_SD;
    FileName = (const char*)&argv[2][2]; // Just skip SD prefix (shall points on '/')
  }
#if defined(XMODEM)
  else if(!strcmp_P(argv[2], xmdm_MEDIA))
  {
    FileMedia.Dst = FILE_MEDIA_XMODEM;
  }
#endif
  if(!FileMedia.Src || !FileMedia.Dst || ((FileMedia.Src == FILE_MEDIA_XMODEM) && (FileMedia.Src == FileMedia.Dst)) || !FileName || !*FileName) return(-1);
  /* Command is correct */
  if(FileMedia.Src == FileMedia.Dst)
  {
    /* SD to SD copy */
    uCli.stream->println(F("SD to SD copy"));
  }
#if defined(XMODEM)
  else
  {
    /* X-Modem is Src or Dst */
    if(FileMedia.Src == FILE_MEDIA_XMODEM)
    {
      /* OpenAVRc X-Modem in Receive mode (Source is outside, Destination is SD)*/
      uCli.stream->println(XReceive(uCli.stream, FileName)); // display return value in the console
    }
    else
    {
      /* OpenAVRc X-Modem in Send mode (Source is SD, Destination is outside) */
      uCli.stream->println(XSend(uCli.stream, FileName)); // display return value in the console
    }
  }
#endif
  return(0);
}

static int8_t uCli_Cmd_rmdir(const char ** argv, uint8_t argc)
{
  argv = argv;
  argc = argc;
  uCli.stream->println(F("rmdir"));

  return(0);
}

static int8_t uCli_Cmd_rm(const char ** argv, uint8_t argc)
{
  argv = argv;
  argc = argc;
  uCli.stream->println(F("rm"));
  if(!memcmp_P(argv[2], SD_MEDIA, 3))
  {
    /* SD/FullFileName */
#if defined(SDCARD)
    sdDeleteFile((const char*)&argv[1][2]); // Skip SD prefix -> keep only /FullFileName
#endif
  }
  return(0);
}

static int8_t uCli_Cmd_mv(const char ** argv, uint8_t argc)
{
  int8_t Ret;
  Ret = uCli_Cmd_cp(argv, argc);
  if(!Ret)
  {
    /* Remove Src file */
  }
  uCli.stream->println(F("mv"));

  return(0);
}

static int8_t uCli_Cmd_bt(const char ** argv, uint8_t argc)
{
  argv = argv;
  argc = argc;
  uCli.stream->println(F("bt"));

  return(0);
}

static int8_t uCli_Cmd_tf(const char ** argv, uint8_t argc)
{
  uint8_t Len, Idx, ComputedCheckSum = 0, CheckSumOk;
  int16_t Excursion;
  int16_t Ret = 1;

  //tf command format:  sCh1sCh2sCh3sCh4sCh5sCh6sCh7sCh8:CS
  //                 tf sHHHsHHHsHHHsHHHsHHHsHHHsHHHsHHH:CS<CR>   (CS=Checksum) -> Ex: tf -100+200-300+400-500+600-700+800:00
  if(argc == 2) // TO DO: Check the transmitter is in trainer mode (PROTO = BT and Trainer mode, not Trainee)
  {
    Len = strlen(argv[1]);
    if(Len == ((NUM_TRAINER * 4) + 1 + 2))
    {
      CheckSumOk = ((argv[1][((NUM_TRAINER * 4) + 1 + 0)]) == '0') && (argv[1][((NUM_TRAINER * 4) + 1 + 1)] == '0'); // Check if checksum shall be ignored
      if(!CheckSumOk)
      {
        /* Checksum shall be checked */
        for(Idx = 0; Idx < ((NUM_TRAINER * 4) + 1); Idx++)
        {
          ComputedCheckSum ^= argv[1][Idx];
        }
        if(!ComputedCheckSum) ComputedCheckSum = 0xFF; // Particular case of 00 value (= ignore)
        CheckSumOk = (((BIN_NBL_TO_HEX_DIGIT(((ComputedCheckSum & 0xF0) >> 4)) == argv[1][((NUM_TRAINER * 4) + 1 + 0)])) && ((BIN_NBL_TO_HEX_DIGIT(ComputedCheckSum & 0x0F)) == argv[0][((NUM_TRAINER * 4) + 1 + 1)]));
      }
      if(CheckSumOk)
      {
        for(Idx = 0; Idx < NUM_TRAINER; Idx++)
        {
          Excursion = (int16_t)strtol(&argv[1][(Idx * 4)], NULL, 16);
          ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
          {
            ppmInput[Idx] = Excursion;
          }
        }
        uCli.stream->println(F("tf"));
        Ret = 0;
      }
    }
  }
  return(Ret);
}

static int8_t uCli_Cmd_ram(const char ** argv, uint8_t argc)
{
  argv = argv;
  argc = argc;
  uCli.stream->print(F("ram: "));uCli.stream->print(stackAvailable());uCli.stream->println(F(" bytes"));

  return(0);
}

static int8_t uCli_Cmd_reboot(const char ** argv, uint8_t argc)
{
  argv = argv;
  argc = argc;
  uCli.stream->println(F("reboot"));
  // TO DO Do a reboot to allow a firmware upgrade through the bootloader
  return(0);
}

