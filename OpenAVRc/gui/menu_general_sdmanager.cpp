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
#include "menu_general.h"


void menuGeneralSdManagerInfo(uint8_t event)
{
  struct sd_raw_info disk_info = {0};
  if sdMounted()
    {
      sd_raw_get_info(&disk_info);
      SdBufferClear();
    }

  SIMPLE_SUBMENU(STR_SD_INFO_TITLE, 1);

  lcdDrawTextLeft(2*FH, STR_SD_TYPE);
  lcdDrawText(10*FW, 2*FH, STR_SD_CARD);

  lcdDrawTextLeft(3*FH, STR_SD_SIZE);
  lcdDrawNumberNAtt(10*FW, 3*FH, disk_info.capacity / 1024 / 1024, LEFT);
  lcdDrawChar(lcdLastPos, 3*FH, 'M');

  lcdDrawTextLeft(4*FH, STR_YEAR);
  lcdDrawNumberNAtt(10*FW, 4*FH, disk_info.manufacturing_year + 2000, LEFT);
}

inline uint8_t isFilenameGreater(uint8_t isfile, const char * fn, const char * line)
{
  return (isfile && !line[SD_SCREEN_FILE_LENGTH+1]) || (isfile==(bool)line[SD_SCREEN_FILE_LENGTH+1] && strcasecmp(fn, line) > 0);
}

inline uint8_t isFilenameLower(uint8_t isfile, const char * fn, const char * line)
{
  return (!isfile && line[SD_SCREEN_FILE_LENGTH+1]) || (isfile==(bool)line[SD_SCREEN_FILE_LENGTH+1] && strcasecmp(fn, line) < 0);
}

void onSdManagerMenu(const char *result)
{
  uint8_t index = menuVerticalPosition-1-menuVerticalOffset;
  if (result == STR_SD_INFO)
    {
      pushMenu(menuGeneralSdManagerInfo);
    }
  else if (result == STR_DELETE_FILE)
    {
      if (sdDeleteFile(ReBuff.sdmanager.lines[index]))
        {
          SdBufferClear();
          strncpy(statusLineMsg, ReBuff.sdmanager.lines[index], 13);
          strcpy_P(statusLineMsg+min((uint8_t)strlen(statusLineMsg), (uint8_t)13), STR_REMOVED);
          showStatusLine();
          if ((uint16_t)menuVerticalPosition == ReBuff.sdmanager.count)
            menuVerticalPosition--;
          ReBuff.sdmanager.offset = menuVerticalOffset-1;
        }
    }
}

void menuGeneralSdManager(uint8_t _event)
{
  uint8_t event = ((READ_ONLY() && EVT_KEY_MASK(_event) == KEY_ENTER) ? 0 : _event);
  SIMPLE_MENU(STR_SD_CARD, menuTabGeneral, e_Sd, 1+ReBuff.sdmanager.count);

  if (s_editMode > 0)
    s_editMode = 0;

  switch(_event)
    {
    case EVT_ENTRY:
      sdChangeCurDir(ROOT_PATH);
      ReBuff.sdmanager.offset = 65535;
      SdBufferClear();
      break;

    case EVT_ROTARY_BREAK:
    case EVT_KEY_FIRST(KEY_RIGHT):
    case EVT_KEY_FIRST(KEY_ENTER):
    {
      if (menuVerticalPosition > 0)
        {
          vertpos_t index = menuVerticalPosition-1-menuVerticalOffset;
          if (!ReBuff.sdmanager.lines[index][SD_SCREEN_FILE_LENGTH+1])
            {
              sdChangeCurDir(ReBuff.sdmanager.lines[index]);
              menuVerticalOffset = 0;
              menuVerticalPosition = 1;
              ReBuff.sdmanager.offset = 65535;
              killEvents(_event);
              SdBufferClear();
              break;
            }
        }
      if (!IS_ROTARY_BREAK(_event) || menuVerticalPosition==0)
        break;
    }

    case EVT_KEY_LONG(KEY_ENTER):
      killEvents(_event);
      if (menuVerticalPosition == 0)
        {
          POPUP_MENU_ADD_ITEM(STR_SD_INFO);
        }
      else
        {
          if (!READ_ONLY())
            {
              POPUP_MENU_ADD_ITEM(STR_DELETE_FILE);
            }
        }
      popupMenuHandler = onSdManagerMenu;
      break;
    }

  if (ReBuff.sdmanager.offset != menuVerticalOffset)
    {
      if (menuVerticalOffset == 0)
        {
          ReBuff.sdmanager.offset = 0;
          memset(ReBuff.sdmanager.lines, 0, sizeof(ReBuff.sdmanager.lines));
        }
      else if (menuVerticalOffset == ReBuff.sdmanager.count-7)
        {
          ReBuff.sdmanager.offset = menuVerticalOffset;
          memset(ReBuff.sdmanager.lines, 0, sizeof(ReBuff.sdmanager.lines));
        }
      else if (menuVerticalOffset > ReBuff.sdmanager.offset)
        {
          memmove(ReBuff.sdmanager.lines[0], ReBuff.sdmanager.lines[1], 6*sizeof(ReBuff.sdmanager.lines[0]));
          memset(ReBuff.sdmanager.lines[6], 0xff, SD_SCREEN_FILE_LENGTH);
          ReBuff.sdmanager.lines[6][SD_SCREEN_FILE_LENGTH+1] = 1;
        }
      else
        {
          memmove(ReBuff.sdmanager.lines[1], ReBuff.sdmanager.lines[0], 6*sizeof(ReBuff.sdmanager.lines[0]));
          memset(ReBuff.sdmanager.lines[0], 0, sizeof(ReBuff.sdmanager.lines[0]));
        }

      ReBuff.sdmanager.count = 0;

      while(fat_read_dir(SD_dir, &SD_dir_entry))
        {

          if (SD_dir_entry.long_name[0] == '.' && SD_dir_entry.long_name[1] == '\0')
            continue; /* Ignore dot entry */

          if (strlen(SD_dir_entry.long_name) > SD_SCREEN_FILE_LENGTH)
            continue;

          ReBuff.sdmanager.count++;

          uint8_t isfile = !(SD_dir_entry.attributes & FAT_ATTRIB_DIR);

          if (menuVerticalOffset == 0)
            {
              for (uint8_t i=0; i<LCD_LINES-1; i++)
                {
                  char *line = ReBuff.sdmanager.lines[i];
                  if (line[0] == '\0' || isFilenameLower(isfile, SD_dir_entry.long_name, line))
                    {
                      if (i < 6)
                        memmove(ReBuff.sdmanager.lines[i+1], line, sizeof(ReBuff.sdmanager.lines[i]) * (6-i));
                      memset(line, 0, sizeof(ReBuff.sdmanager.lines[i]));
                      strcpy(line, SD_dir_entry.long_name);
                      line[SD_SCREEN_FILE_LENGTH+1] = isfile;
                      break;
                    }
                }
            }
          else if (ReBuff.sdmanager.offset == menuVerticalOffset)
            {
              for (int8_t i=6; i>=0; i--)
                {
                  char *line = ReBuff.sdmanager.lines[i];
                  if (line[0] == '\0' || isFilenameGreater(isfile, SD_dir_entry.long_name, line))
                    {
                      if (i > 0)
                        memmove(ReBuff.sdmanager.lines[0], ReBuff.sdmanager.lines[1], sizeof(ReBuff.sdmanager.lines[0]) * i);
                      memset(line, 0, sizeof(ReBuff.sdmanager.lines[i]));
                      strcpy(line, SD_dir_entry.long_name);
                      line[SD_SCREEN_FILE_LENGTH+1] = isfile;
                      break;
                    }
                }
            }
          else if (menuVerticalOffset > ReBuff.sdmanager.offset)
            {
              if (isFilenameGreater(isfile, SD_dir_entry.long_name, ReBuff.sdmanager.lines[5]) && isFilenameLower(isfile, SD_dir_entry.long_name, ReBuff.sdmanager.lines[6]))
                {
                  memset(ReBuff.sdmanager.lines[6], 0, sizeof(ReBuff.sdmanager.lines[0]));
                  strcpy(ReBuff.sdmanager.lines[6], SD_dir_entry.long_name);
                  ReBuff.sdmanager.lines[6][SD_SCREEN_FILE_LENGTH+1] = isfile;
                }
            }
          else
            {
              if (isFilenameLower(isfile, SD_dir_entry.long_name, ReBuff.sdmanager.lines[1]) && isFilenameGreater(isfile, SD_dir_entry.long_name, ReBuff.sdmanager.lines[0]))
                {
                  memset(ReBuff.sdmanager.lines[0], 0, sizeof(ReBuff.sdmanager.lines[0]));
                  strcpy(ReBuff.sdmanager.lines[0], SD_dir_entry.long_name);
                  ReBuff.sdmanager.lines[0][SD_SCREEN_FILE_LENGTH+1] = isfile;
                }
            }
        }
        SdBufferClear();
    }

  ReBuff.sdmanager.offset = menuVerticalOffset;

  for (uint8_t i=0; i<LCD_LINES-1; i++)
    {
      coord_t y = MENU_HEADER_HEIGHT + 1 + i*FH;
      lcdNextPos = 0;
      uint8_t attr = (menuVerticalPosition-1-menuVerticalOffset == i ? BSS|INVERS : BSS);
      if (ReBuff.sdmanager.lines[i][0])
        {
          if (!ReBuff.sdmanager.lines[i][SD_SCREEN_FILE_LENGTH+1])
            {
              lcdDrawCharAtt(0, y, '[', attr);
            }
          lcdDrawTextAtt(lcdNextPos, y, ReBuff.sdmanager.lines[i], attr);
          if (!ReBuff.sdmanager.lines[i][SD_SCREEN_FILE_LENGTH+1])
            {
              lcdDrawCharAtt(lcdNextPos, y, ']', attr);
            }
        }
    }
}
