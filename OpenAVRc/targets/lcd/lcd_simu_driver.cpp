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

// Hwserial
void SendBtSerTxBuffer(uint8_t c)
{
  wxGetApp().SimuFrame->SendBtSerTxBufferToCliFrame(c);
}

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
  //wxYieldIfNeeded();
  wxGetApp().Yield();
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
  wxGetApp().SimuFrame->DrawWxSimuLcd();
}

void lcdRefresh()
{
  for (uint8_t i=0; i < NUMITERATIONFULLREFRESH; i++)
    {
      lcdRefreshFast();
    }
}

void simu_EditModelName()
{
  wxGetApp().SimuFrame->EditModelName();
}

//SD FUNCTIONS

uint8_t sd_raw_read,sd_raw_read_interval,sd_raw_write,sd_raw_write_interval;

static wxString sdpath, sdroot;
static wxDir sddir(sdroot);
static wxFile sdfile;
static bool cont = 0;

wxString switchRootChar(wxString val)
{
  val.Replace("/","\\");
  return val;
}

void simulateLcdBufferUsedBySd()
{
  for (uint16_t i=0; i<512 ; ++i)
  {
    displayBuf[i] = (rand() & 0xFF);
  }
}

void sd_raw_get_info(struct sd_raw_info* tmp)
{
  simulateLcdBufferUsedBySd();
  tmp->manufacturing_year = 18;
  tmp->capacity = 1978 * 1024 * 1024;
}

uint8_t sd_raw_init()
{
  wxString root = AppPath+"\\SD\\";

  if (!sddir.Exists(root))
    {
      sddir.Make(root);
    }
  sdroot = root;
  simulateLcdBufferUsedBySd();
  return 1;
}

uint8_t sd_raw_sync()
{
  simulateLcdBufferUsedBySd();
  return 1;
}

struct fat_fs_struct* fat_open(struct partition_struct* partition)
{
  struct fat_fs_struct* tmp = 0;
  simulateLcdBufferUsedBySd();
  return tmp;
}

partition_struct* partition_open(uint8_t a,uint8_t b,uint8_t c,uint8_t d,uint8_t e)
{
  partition_struct* tmp = 0;
  simulateLcdBufferUsedBySd();
  return tmp;
}

uint8_t fat_seek_file(struct fat_file_struct* fd, int32_t* offset, uint8_t whence)
{
  uint8_t ret = 0;

  if (sdfile.Seek(*offset,(wxSeekMode)whence) != wxInvalidOffset)
    {
      ret = 1;
    }
  simulateLcdBufferUsedBySd();
  return ret;
}

uint8_t fat_delete_file(struct fat_fs_struct* fs, struct fat_dir_entry_struct* dir_entry)
{
  simulateLcdBufferUsedBySd();
  return wxRemoveFile(sdroot + sdpath + switchRootChar(wxString::FromUTF8(dir_entry->long_name)));
}

uint8_t fat_create_file(struct fat_dir_struct* parent, const char* file, struct fat_dir_entry_struct* dir_entry)
{
  uint8_t ret = 0;
  wxString temp = sdroot + sdpath + "\\" + switchRootChar(wxString::FromUTF8(file));

  simulateLcdBufferUsedBySd();
  if (wxFile::Exists(temp))
    {
      ret = 2;
    }
  else if (sdfile.Create(temp, true, wxS_DEFAULT))
    {
      ret = 1;
    }
  if (ret)
    {
      strncpy(dir_entry->long_name, file, 32);
    }
  return ret;
}

struct fat_file_struct* fat_open_file(struct fat_fs_struct* fs, struct fat_dir_entry_struct* dir_entry)
{
  struct fat_file_struct* tmp = 0;
  wxString temp = sdroot + sdpath + "\\" + switchRootChar(wxString::FromUTF8(dir_entry->long_name));

  simulateLcdBufferUsedBySd();

  if (wxFile::Exists(temp))
    {
      if (sdfile.Open(temp, wxFile::read_write, wxS_DEFAULT))
        {
          dir_entry->file_size = sdfile.Length();
          return (struct fat_file_struct*)&sdfile;
        }
    }
  return tmp;
}

intptr_t fat_write_file(struct fat_file_struct* fd, const uint8_t* buffer, uintptr_t buffer_len)
{
  size_t ret = 0;

  simulateLcdBufferUsedBySd();

  ret = sdfile.Write(buffer, buffer_len);
  if (sdfile.Flush())
    {
      return ret;
    }
  return 0;
}

intptr_t fat_read_file(struct fat_file_struct* fd, uint8_t* buffer, uintptr_t buffer_len)
{
  return (intptr_t)sdfile.Read(buffer,buffer_len);
}

void fat_close_file(struct fat_file_struct* fd)
{
  simulateLcdBufferUsedBySd();
  sdfile.Close();
  SD_file = 0;
}

uint8_t fat_read_dir(struct fat_dir_struct* dd, struct fat_dir_entry_struct* dir_entry)
{
  simulateLcdBufferUsedBySd();

  if (!sddir.IsOpened())
    {
      return 0;
    }
  wxString filename, filespec;
  if (!cont)
    {
      cont = sddir.GetFirst(&filename, filespec, wxDIR_FILES | wxDIR_DIRS | ((sddir.GetName() == sdroot)? 0:wxDIR_DOTDOT));
    }
  else
    {
      cont = sddir.GetNext(&filename);
    }
  if (cont)
    {
      strncpy(dir_entry->long_name, (const char*)filename.mbc_str(), 32);
      dir_entry->attributes = (wxDir::Exists(sddir.GetName() + "\\" + filename))? FAT_ATTRIB_DIR:0;
    }
  return cont? 1:0;
}

uint8_t fat_get_dir_entry_of_path(struct fat_fs_struct* fs, const char* path, struct fat_dir_entry_struct* dir_entry)
{
  simulateLcdBufferUsedBySd();
  strncpy(dir_entry->long_name, path, 32);
  return 1;
}

struct fat_dir_struct* fat_open_dir(struct fat_fs_struct* fs, const struct fat_dir_entry_struct* dir_entry)
{
  simulateLcdBufferUsedBySd();
  struct fat_dir_struct SD_dir;
  struct fat_dir_struct * P_SD_dir = &SD_dir;;

  if (sddir.Open(sdroot + switchRootChar(wxString::FromUTF8(dir_entry->long_name))))
    {
      sdpath = switchRootChar(wxString::FromUTF8(dir_entry->long_name));
      if (sdpath.length() != 1)
        sdpath.Append("\\"); // Not root
        strncpy(SD_dir.dir_entry.long_name, dir_entry->long_name, 32);
    }
  return P_SD_dir;
}

uint8_t fat_create_dir(struct fat_dir_struct* parent, const char* dir, struct fat_dir_entry_struct* dir_entry)
{
  uint8_t ret = 0;

  if (sddir.Exists(sdroot + switchRootChar(wxString::FromUTF8(dir))))
    {
      ret = 1;
    }
  else if (sddir.Make(sdroot + switchRootChar(wxString::FromUTF8(dir)), wxS_DIR_DEFAULT))
    {
      ret = 1;
    }
  return ret;
}

uint8_t fat_reset_dir(struct fat_dir_struct* dd)
{
  simulateLcdBufferUsedBySd();
  cont = 0; // reset fat_read_dir function
  return 1;
}

void fat_close_dir(struct fat_dir_struct* dd)
{
  simulateLcdBufferUsedBySd();

  if (sddir.IsOpened())
    {
      sddir.Close();
    }
}

uint8_t fat_move_file(struct fat_fs_struct* fs, struct fat_dir_entry_struct* dir_entry, struct fat_dir_struct* parent_new, const char* file_new)
{
 simulateLcdBufferUsedBySd();
 wxString fromFile = wxString::FromUTF8(dir_entry->long_name);
 wxString fromDir = sdpath;
 fromDir.Replace("\\", "");
 wxString destFile = wxString::FromUTF8(file_new);
 wxString destDir = wxString::FromUTF8(parent_new->dir_entry.long_name);

 if (sddir.Exists(sdroot + fromDir))
  {
   if (sddir.Exists(sdroot + switchRootChar(wxString::FromUTF8(destDir))))
    {
     wxString temp = sdroot + fromDir + "\\" + switchRootChar(wxString::FromUTF8(fromFile));
     if (wxFile::Exists(temp))
      {
       if (wxCopyFile(sdroot+fromDir+"\\"+switchRootChar(wxString::FromUTF8(fromFile)), sdroot+destDir+"\\"+switchRootChar(wxString::FromUTF8(destFile)), 0))
        {
         if (wxRemoveFile(sdroot+fromDir+"\\"+switchRootChar(wxString::FromUTF8(fromFile))))
         {
          return 1;
         }
       }
     }
   }
 }
 return 0;
}
//////////////////////////////////

void PlayBeep(uint32_t freq, uint32_t time)
{
  BeepFreq = freq;
  BeepTime = time;
}


