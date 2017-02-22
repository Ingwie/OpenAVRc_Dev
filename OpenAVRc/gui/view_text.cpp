/*
**************************************************************************
*                                                                        *
*              This file is part of the OpenAVRc project.                *
*                                                                        *
*                         Based on code named                            *
*             OpenTx - https://github.com/opentx/opentx                  *
*                                                                        *
*                Only AVR code here for lisibility ;-)                   *
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

#define VOICE_PROMPT_TEXT_LEN  25

FORCEINLINE void showVoiceTextLine(uint8_t Numline, char * PromptText)
{
  FIL file;
  int result;
  char c[VOICE_PROMPT_TEXT_LEN] = {0};
  unsigned int sz;

  result = f_chdir(VOICETXT_PATH);
  if (result == FR_OK) {
    result = f_open(&file,VOICETXT_FILE, FA_OPEN_EXISTING | FA_READ);
    if (result == FR_OK) {
      result = f_lseek(&file, Numline*(VOICE_PROMPT_TEXT_LEN+2));
      if (result == FR_OK) {
        result = f_read(&file, &c, VOICE_PROMPT_TEXT_LEN, &sz);
        if (result == FR_OK) {
          memcpy(PromptText,c,VOICE_PROMPT_TEXT_LEN);
        }
      }
    }
  }
  f_close(&file);


// test
/*
  PromptText[0] = '1';
  PromptText[1] = '2';
  PromptText[2] = '3';
  PromptText[3] = '4';
  PromptText[4] = '5';
  PromptText[5] = '6';
  PromptText[6] = '7';
  PromptText[7] = '8';
  PromptText[8] = '9';
  PromptText[9] = 'A';
  PromptText[10] = 'B';
  PromptText[11] = 'C';
  PromptText[12] = 'D';
  PromptText[13] = 'E';
  PromptText[14] = 'F';
  PromptText[15] = 'G';
  PromptText[16] = 'H';
  PromptText[17] = 'I';
  PromptText[18] = 'J';
  PromptText[19] = 'K';
  PromptText[20] = 'L';
  PromptText[21] = 'M';
  PromptText[22] = 'N';
  PromptText[23] = 'O';
  PromptText[24] = 'K';
*/
}


// Keep unused code ;-)

/*#define TEXT_FILE_MAXSIZE     2048

char s_text_file[TEXT_FILENAME_MAXLEN];
char s_text_screen[LCD_LINES-1][LCD_COLS+1];

void readTextFile(int & lines_count)
{
  FIL file;
  int result;
  char c;
  unsigned int sz;
  int line_length = 0;
  int escape = 0;
  char escape_chars[2];
  int current_line = 0;

  memset(s_text_screen, 0, sizeof(s_text_screen));

  result = f_open(&file, s_text_file, FA_OPEN_EXISTING | FA_READ);
  if (result == FR_OK) {
    for (int i=0; i<TEXT_FILE_MAXSIZE && f_read(&file, &c, 1, &sz)==FR_OK && sz==1 && (lines_count==0 || current_line-menuVerticalOffset<LCD_LINES-1); i++) {
      if (c == '\n') {
        ++current_line;
        line_length = 0;
        escape = 0;
      } else if (c!='\r' && current_line>=menuVerticalOffset && current_line-menuVerticalOffset<LCD_LINES-1 && line_length<LCD_COLS) {
        if (c=='\\' && escape==0) {
          escape = 1;
          continue;
        } else if (c!='\\' && escape>0 && escape<3) {
          escape_chars[escape-1] = c;
          if (escape == 2 && !strncmp(escape_chars, "up", 2)) {
            c = '\300';
            escape = 0;
          } else if (escape == 2 && !strncmp(escape_chars, "dn", 2)) {
            c = '\301';
            escape = 0;
          } else {
            escape++;
            continue;
          }
        } else if (c=='~') {
          c = 'z'+1;
        } else if (c=='\t') {
          c = 0x1D; //tab
        }
        escape = 0;
        s_text_screen[current_line-menuVerticalOffset][line_length++] = c;
      }
    }
    if (c != '\n') {
      current_line += 1;
    }
    f_close(&file);
  }

  if (lines_count == 0) {
    lines_count = current_line;
  }
}

void menuTextView(uint8_t event)
{
  static int lines_count;

  switch (event) {
  case EVT_ENTRY:
    menuVerticalOffset = 0;
    lines_count = 0;
    readTextFile(lines_count);
    break;

  case EVT_KEY_FIRST(KEY_UP):
    if (menuVerticalOffset == 0)
      break;
    else
      menuVerticalOffset--;
  // no break;

  case EVT_KEY_FIRST(KEY_DOWN):
    // if (event == EVT_KEY_BREAK(KEY_DOWN)) {
    if (menuVerticalOffset+LCD_LINES-1 >= lines_count)
      break;
    else
      ++menuVerticalOffset;
    // }
    readTextFile(lines_count);
    break;

  case EVT_KEY_BREAK(KEY_EXIT):
    popMenu();
    break;
  }

  for (int i=0; i<LCD_LINES-1; i++) {
    lcdDrawTextAtt(0, i*FH+FH+1, s_text_screen[i], FIXEDWIDTH);
  }

  char *title = s_text_file;
#if defined(SIMU)
  if (!strncmp(title, "./", 2)) title += 2;
#else
  // TODO?
#endif
  lcdDrawText(LCD_W/2-strlen(title)*FW/2, 0, title);
  lcdInvertLine(0);

  if (lines_count > LCD_LINES-1) {
    displayScrollbar(LCD_W-1, FH, LCD_H-FH, menuVerticalOffset, lines_count, LCD_LINES-1);
  }
}*/
