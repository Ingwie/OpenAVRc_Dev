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


#include "../../../OpenAVRc_Simulator/OpenAVRc_SimulatorApp.h"

#include <wx/filename.h>
#include <wx/dir.h>
#include <wx/msgdlg.h>
#include <wx/choicdlg.h>


#define NUMITERATIONFULLREFRESH  1
#define SIMU_VOICE_PROMPT_TEXT_LEN 25

wxFileName Myfile;

void simuTrace(const char * format, ...)
{
  va_list arglist;
  va_start(arglist, format);
  char tmp[50];
  vsnprintf(tmp, 50, format, arglist);
  wxLogStatus(tmp);
  va_end(arglist);
}

/*bool wxYieldIfNeeded() //undocumented function save a copy here ...
{
  return wxTheApp && wxTheApp->Yield(true);
}*/

void SimuSleepMs(uint16_t x)
{
  wxYieldIfNeeded();
  wxMilliSleep(x);
}

void lcdInit()
{
  BACKLIGHT_ON(); //LCD back light on
}

void lcdSetRefVolt(uint8_t val)
{
val=val;
}

void lcdRefreshFast()
{
  SHOWDURATIONLCD1
#if defined(SHOWDURATION)
  lcdDrawNumberAttUnit(16*FW, 1, DURATION_MS_PREC2(DurationValue), PREC2);
#endif
  wxGetApp().SimuFrame->DrawWxSimuLcd();
  SHOWDURATIONLCD2
}

void lcdRefresh()
{
  for (uint8_t i=0; i < NUMITERATIONFULLREFRESH; i++) {
    lcdRefreshFast();
  }
}

void simu_EditModelName()
{
  wxGetApp().SimuFrame->EditModelName();
}

//SD FUNCTIONS

void Simu_showVoiceTextLine(uint8_t Numline, char * PromptText)
{
  wxTextFile MyVoiceListfile(AppPath+"\\list.txt");
  wxString voiceText;

  if (MyVoiceListfile.Exists()) {
    MyVoiceListfile.Open(AppPath+"\\list.txt");
    voiceText = MyVoiceListfile.GetLine(Numline);
    voiceText.Truncate(SIMU_VOICE_PROMPT_TEXT_LEN);
    memcpy(PromptText, voiceText.c_str(),SIMU_VOICE_PROMPT_TEXT_LEN);
    MyVoiceListfile.Close();
  }
}

void PlayBeep(uint32_t freq, uint32_t time)
{
  BeepFreq = freq;
  BeepTime = time;
}

FRESULT f_close (FIL * fil)
{
  TRACE("f_close (FIL:%p)",  fil);
  //if (fil->fs) {
  //  fclose((FILE*)fil->fs);
  return FR_OK;
}

FRESULT f_opendir (DIR * rep, const TCHAR * name)
{
  char temp[20];
  //sprintf(temp, "%s", name);
  wxString dir(temp,wxConvUTF8);
  dir.Replace("/","\\",true);
  if (dir == ".") dir = "";
  //wxMessageBox("f_opendir  " +AppPath+dir);
  if (Myfile.DirExists(AppPath+dir)) {
    rep->fs = (FATFS *)true;
    simu_dir = AppPath+dir;
    TRACE("f_opendir(%s) = OK", name);
    return FR_OK;
  }
  TRACE("f_opendir(%s) = error", name);
  return FR_NO_PATH;
}

FRESULT f_readdir (DIR * rep, FILINFO * fil)
{
  static wxDir dir(simu_dir);
  static wxString filename;
  static bool cont = false;

  if (!cont) {
    cont = dir.GetFirst(&filename, wxEmptyString, wxDIR_DEFAULT );
    //wxMessageBox(wxString::Format(wxT("%s\n"), filename.c_str()));

    //fil->fattrib = AM_DIR; //todo check

    /* memset(fil->fname, 0, 13);
     memset(fil->lfname, 0, SD_SCREEN_FILE_LENGTH);
    TCHAR fn[13];
    const wxChar* myStringChars = filename.c_str();
    for (int i = 0; i < 13; i++) {
     fn[i] = myStringChars[i];
    }*/
    //strncpy(fil->fname, fn, 13-1);
    //strcpy(fil->lfname, fn);

    //fil->fname = wxString::Format(wxT("%13s\n"), filename.c_str());
    if (!cont) return FR_NO_FILE;
    else return FR_OK;
  }

//fil->fattrib = AM_DIR;

  cont = dir.GetNext(&filename);

  // wxMessageBox(wxString::Format(wxT("%s\n"), filename.c_str()));

  //if (!rep->fs) return FR_NO_FILE;

  if (!cont) return FR_NO_FILE;
  else return FR_OK;

  //fil->fattrib = (ent->d_type == DT_DIR ? AM_DIR : 0);

  //return FR_OK;
}

FRESULT f_mkdir (const TCHAR*)
{
  return FR_OK;
}

FRESULT f_open (FIL * fil, const TCHAR *name, BYTE flag)
{
  return FR_INVALID_NAME;
  return FR_OK;
}

FRESULT f_write (FIL* fil, const void* data, UINT size, UINT* written)
{
  return FR_OK;
}

FRESULT f_read (FIL* fil, void* data, UINT size, UINT* read)
{
  return FR_OK;
}

FRESULT f_getcwd (TCHAR *path, UINT sz_path)
{
  return FR_NO_PATH;
  return FR_OK;
}

FRESULT f_unlink (const TCHAR* name)
{
  return FR_INVALID_NAME;
  return FR_OK;
}

FRESULT f_mkfs (const TCHAR *path, BYTE, UINT)
{
  TRACE("Format SD...");
  return FR_OK;
}

FRESULT f_chdir (const TCHAR *name)
{
  //chdir(convertSimuPath(name));
  return FR_OK;
}

FRESULT f_lseek (FIL* fil, DWORD offset)
{
  return FR_OK;
}

int f_putc (TCHAR c, FIL * fil)
{
  //if (fil && fil->fs) fwrite(&c, 1, 1, (FILE*)fil->fs);
  return FR_OK;
}

int f_puts (const TCHAR * str, FIL * fil)
{
  int n;
  for (n = 0; *str; str++, n++) {
    if (f_putc(*str, fil) == EOF) return EOF;
  }
  return n;
}

int f_printf (FIL *fil, const TCHAR * format, ...)
{
  va_list arglist;
  va_start(arglist, format);
  //if (fil && fil->fs) vfprintf((FILE*)fil->fs, format, arglist);
  va_end(arglist);
  return 0;
}

FRESULT f_stat (const TCHAR * name, FILINFO *)
{
  return FR_INVALID_NAME;
  return FR_OK;
}

FRESULT f_mount (FATFS*,const TCHAR*, BYTE opt)
{
  return FR_OK;
}
