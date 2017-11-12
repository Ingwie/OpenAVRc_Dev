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
  SIMPLE_SUBMENU(STR_SD_INFO_TITLE, 1);

  lcdDrawTextLeft(2*FH, STR_SD_TYPE);
  lcdDrawText(10*FW, 2*FH, SD_IS_HC() ? STR_SDHC_CARD : STR_SD_CARD);

  lcdDrawTextLeft(3*FH, STR_SD_SIZE);
  lcdDrawNumberAttUnit(10*FW, 3*FH, sdGetSize(), LEFT);
  lcdDrawChar(lcdLastPos, 3*FH, 'M');

  lcdDrawTextLeft(4*FH, STR_SD_SECTORS);
#if defined(SD_GET_FREE_BLOCKNR)
  lcdDrawNumberAttUnit(10*FW, 4*FH,  SD_GET_FREE_BLOCKNR()/1000, LEFT);
  lcdDrawChar(lcdLastPos, 4*FH, '/');
  lcdDrawNumberAttUnit(lcdLastPos+FW, 4*FH, sdGetNoSectors()/1000, LEFT);
#else
  lcdDrawNumberAttUnit(10*FW, 4*FH, sdGetNoSectors()/1000, LEFT);
#endif
  lcdDrawChar(lcdLastPos, 4*FH, 'k');

  lcdDrawTextLeft(5*FH, STR_SD_SPEED);
  lcdDrawNumberAttUnit(10*FW, 5*FH, SD_GET_SPEED()/1000, LEFT);
  lcdDrawText(lcdLastPos, 5*FH, "kb/s");
}

inline bool isFilenameGreater(bool isfile, const char * fn, const char * line)
{
  return (isfile && !line[SD_SCREEN_FILE_LENGTH+1]) || (isfile==(bool)line[SD_SCREEN_FILE_LENGTH+1] && strcasecmp(fn, line) > 0);
}

inline bool isFilenameLower(bool isfile, const char * fn, const char * line)
{
  return (!isfile && line[SD_SCREEN_FILE_LENGTH+1]) || (isfile==(bool)line[SD_SCREEN_FILE_LENGTH+1] && strcasecmp(fn, line) < 0);
}

void onSdManagerMenu(const char *result)
{
  TCHAR lfn[_MAX_LFN+1];

  uint8_t index = menuVerticalPosition-1-menuVerticalOffset;
  if (result == STR_SD_INFO) {
    pushMenu(menuGeneralSdManagerInfo);
  } else if (result == STR_SD_FORMAT) {
    POPUP_CONFIRMATION(STR_CONFIRM_FORMAT);
  } else if (result == STR_DELETE_FILE) {
    f_getcwd(lfn, _MAX_LFN);
    strcat_P(lfn, PSTR("/"));
    strcat(lfn, reusableBuffer.sdmanager.lines[index]);
    f_unlink(lfn);
    strncpy(statusLineMsg, reusableBuffer.sdmanager.lines[index], 13);
    strcpy_P(statusLineMsg+min((uint8_t)strlen(statusLineMsg), (uint8_t)13), STR_REMOVED);
    showStatusLine();
    if ((uint16_t)menuVerticalPosition == reusableBuffer.sdmanager.count) menuVerticalPosition--;
    reusableBuffer.sdmanager.offset = menuVerticalOffset-1;
  }
}

void menuGeneralSdManager(uint8_t _event)
{
  FILINFO fno;
  DIR dir;
  TCHAR lfn[_MAX_LFN + 1];
  fno.lfname = lfn;
  fno.lfsize = sizeof(lfn);

#if defined(SDCARD)
  if (warningResult) {
    warningResult = false;
    displayPopup(STR_FORMATTING);
    closeLogs();
    if (f_mkfs(0, 1, 0) == FR_OK) {
      f_chdir("/");
      reusableBuffer.sdmanager.offset = -1;
    } else {
      POPUP_WARNING(STR_SDCARD_ERROR);
    }
  }
#endif

  uint8_t event = ((READ_ONLY() && EVT_KEY_MASK(_event) == KEY_ENTER) ? 0 : _event);
  SIMPLE_MENU(SD_IS_HC() ? STR_SDHC_CARD : STR_SD_CARD, menuTabGeneral, e_Sd, 1+reusableBuffer.sdmanager.count);

  if (s_editMode > 0)
    s_editMode = 0;

  switch(_event) {
  case EVT_ENTRY:
    f_chdir(ROOT_PATH);
    reusableBuffer.sdmanager.offset = 65535;
    break;

    CASE_EVT_ROTARY_BREAK
  case EVT_KEY_FIRST(KEY_RIGHT):
  case EVT_KEY_FIRST(KEY_ENTER): {
    if (menuVerticalPosition > 0) {
      vertpos_t index = menuVerticalPosition-1-menuVerticalOffset;
      if (!reusableBuffer.sdmanager.lines[index][SD_SCREEN_FILE_LENGTH+1]) {
        f_chdir(reusableBuffer.sdmanager.lines[index]);
        menuVerticalOffset = 0;
        menuVerticalPosition = 1;
        reusableBuffer.sdmanager.offset = 65535;
        killEvents(_event);
        break;
      }
    }
    if (!IS_ROTARY_BREAK(_event) || menuVerticalPosition==0)
      break;
    // no break;
  }

  case EVT_KEY_LONG(KEY_ENTER):
    killEvents(_event);
    if (menuVerticalPosition == 0) {
      POPUP_MENU_ADD_ITEM(STR_SD_INFO);
      POPUP_MENU_ADD_ITEM(STR_SD_FORMAT);
    } else {
      if (!READ_ONLY()) {
        POPUP_MENU_ADD_ITEM(STR_DELETE_FILE);
        // POPUP_MENU_ADD_ITEM(STR_RENAME_FILE);  TODO: Implement
        // POPUP_MENU_ADD_ITEM(STR_COPY_FILE);    TODO: Implement
      }
    }
    popupMenuHandler = onSdManagerMenu;
    break;
  }

  if (reusableBuffer.sdmanager.offset != menuVerticalOffset) {
    if (menuVerticalOffset == 0) {
      reusableBuffer.sdmanager.offset = 0;
      memset(reusableBuffer.sdmanager.lines, 0, sizeof(reusableBuffer.sdmanager.lines));
    } else if (menuVerticalOffset == reusableBuffer.sdmanager.count-7) {
      reusableBuffer.sdmanager.offset = menuVerticalOffset;
      memset(reusableBuffer.sdmanager.lines, 0, sizeof(reusableBuffer.sdmanager.lines));
    } else if (menuVerticalOffset > reusableBuffer.sdmanager.offset) {
      memmove(reusableBuffer.sdmanager.lines[0], reusableBuffer.sdmanager.lines[1], 6*sizeof(reusableBuffer.sdmanager.lines[0]));
      memset(reusableBuffer.sdmanager.lines[6], 0xff, SD_SCREEN_FILE_LENGTH);
      reusableBuffer.sdmanager.lines[6][SD_SCREEN_FILE_LENGTH+1] = 1;
    } else {
      memmove(reusableBuffer.sdmanager.lines[1], reusableBuffer.sdmanager.lines[0], 6*sizeof(reusableBuffer.sdmanager.lines[0]));
      memset(reusableBuffer.sdmanager.lines[0], 0, sizeof(reusableBuffer.sdmanager.lines[0]));
    }

    reusableBuffer.sdmanager.count = 0;
    char *fn;   /* This function is assuming non-Unicode cfg. */
    FRESULT res = f_opendir(&dir, ".");        /* Open the directory */
    if (res == FR_OK) {
      for (;;) {
        res = f_readdir(&dir, &fno);                   /* Read a directory item */
        if (res != FR_OK || fno.fname[0] == 0) break;  /* Break on error or end of dir */
        if (fno.fname[0] == '.' && fno.fname[1] == '\0') continue;             /* Ignore dot entry */
#if _USE_LFN
        fn = *fno.lfname ? fno.lfname : fno.fname;
#else
        fn = fno.fname;
#endif
        if (strlen(fn) > SD_SCREEN_FILE_LENGTH) continue;

        reusableBuffer.sdmanager.count++;

        bool isfile = !(fno.fattrib & AM_DIR);

        if (menuVerticalOffset == 0) {
          for (uint8_t i=0; i<LCD_LINES-1; i++) {
            char *line = reusableBuffer.sdmanager.lines[i];
            if (line[0] == '\0' || isFilenameLower(isfile, fn, line)) {
              if (i < 6) memmove(reusableBuffer.sdmanager.lines[i+1], line, sizeof(reusableBuffer.sdmanager.lines[i]) * (6-i));
              memset(line, 0, sizeof(reusableBuffer.sdmanager.lines[i]));
              strcpy(line, fn);
              line[SD_SCREEN_FILE_LENGTH+1] = isfile;
              break;
            }
          }
        } else if (reusableBuffer.sdmanager.offset == menuVerticalOffset) {
          for (int8_t i=6; i>=0; i--) {
            char *line = reusableBuffer.sdmanager.lines[i];
            if (line[0] == '\0' || isFilenameGreater(isfile, fn, line)) {
              if (i > 0) memmove(reusableBuffer.sdmanager.lines[0], reusableBuffer.sdmanager.lines[1], sizeof(reusableBuffer.sdmanager.lines[0]) * i);
              memset(line, 0, sizeof(reusableBuffer.sdmanager.lines[i]));
              strcpy(line, fn);
              line[SD_SCREEN_FILE_LENGTH+1] = isfile;
              break;
            }
          }
        } else if (menuVerticalOffset > reusableBuffer.sdmanager.offset) {
          if (isFilenameGreater(isfile, fn, reusableBuffer.sdmanager.lines[5]) && isFilenameLower(isfile, fn, reusableBuffer.sdmanager.lines[6])) {
            memset(reusableBuffer.sdmanager.lines[6], 0, sizeof(reusableBuffer.sdmanager.lines[0]));
            strcpy(reusableBuffer.sdmanager.lines[6], fn);
            reusableBuffer.sdmanager.lines[6][SD_SCREEN_FILE_LENGTH+1] = isfile;
          }
        } else {
          if (isFilenameLower(isfile, fn, reusableBuffer.sdmanager.lines[1]) && isFilenameGreater(isfile, fn, reusableBuffer.sdmanager.lines[0])) {
            memset(reusableBuffer.sdmanager.lines[0], 0, sizeof(reusableBuffer.sdmanager.lines[0]));
            strcpy(reusableBuffer.sdmanager.lines[0], fn);
            reusableBuffer.sdmanager.lines[0][SD_SCREEN_FILE_LENGTH+1] = isfile;
          }
        }
      }
    }
  }

  reusableBuffer.sdmanager.offset = menuVerticalOffset;

  for (uint8_t i=0; i<LCD_LINES-1; i++) {
    coord_t y = MENU_HEADER_HEIGHT + 1 + i*FH;
    lcdNextPos = 0;
    uint8_t attr = (menuVerticalPosition-1-menuVerticalOffset == i ? BSS|INVERS : BSS);
    if (reusableBuffer.sdmanager.lines[i][0]) {
      if (!reusableBuffer.sdmanager.lines[i][SD_SCREEN_FILE_LENGTH+1]) {
        lcdDrawCharAtt(0, y, '[', attr);
      }
      lcdDrawTextAtt(lcdNextPos, y, reusableBuffer.sdmanager.lines[i], attr);
      if (!reusableBuffer.sdmanager.lines[i][SD_SCREEN_FILE_LENGTH+1]) {
        lcdDrawCharAtt(lcdNextPos, y, ']', attr);
      }
    }
  }
}
