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

#include "OpenAVRc.h"

struct partition_struct* SD_partition;
struct fat_fs_struct*    SD_filesystem;
struct fat_dir_entry_struct SD_dir_entry;
struct fat_dir_struct* SD_dir;
struct fat_file_struct* SD_file;

uint8_t MountSD()
{
  uint8_t ret = 0;
  /* setup sd card slot */
  if (sd_raw_init())
    {
      /* open first partition */
      SD_partition = partition_open(sd_raw_read,
                                    sd_raw_read_interval,
                                    sd_raw_write,
                                    sd_raw_write_interval,
                                    0);

      if (SD_partition)
        {
          /* open file system */
          SD_filesystem = fat_open(SD_partition);

          if (SD_filesystem)
            {
              /* open root directory */
              fat_get_dir_entry_of_path(SD_filesystem, ROOT_PATH, &SD_dir_entry);
              SD_dir = fat_open_dir(SD_filesystem, &SD_dir_entry);

              if (SD_dir)
                {
                  ret = 1;
                }
            }
        }
    }
  return ret;
}

void UmountSD()
{
  while (!sd_raw_sync()) {};
  /* close file */
  fat_close_file(SD_file);
  /* close directory */
  fat_close_dir(SD_dir);
  /* close file system */
  fat_close(SD_filesystem);
  /* close partition */
  partition_close(SD_partition);
}

uint8_t sdChangeCurDir(const char* path)
{
  fat_close_dir(SD_dir);
  if ((path[0] == '.') && (path[1] == '.') && (path[2] == '\0') )
    {
      fat_get_dir_entry_of_path(SD_filesystem, ROOT_PATH, &SD_dir_entry);
    }
  else
    {
      fat_get_dir_entry_of_path(SD_filesystem, path, &SD_dir_entry);
    }
  SD_dir = fat_open_dir(SD_filesystem, &SD_dir_entry);
  uint8_t ret = (!SD_dir)? 0:1;
  return ret;
}

void sdCreateSystemDir()
{
  fat_create_dir(SD_dir, MODELS_PATH, &SD_dir_entry);
  fat_create_dir(SD_dir, LOGS_PATH, &SD_dir_entry);
}

uint8_t sdOpenDir(const char* path)
{
  return sdChangeCurDir(path);
}

uint8_t sdOpenModelsDir()
{
  return sdOpenDir(ROOT_PATH MODELS_PATH);
}

uint8_t sdOpenLogsDir()
{
  return sdOpenDir(ROOT_PATH LOGS_PATH);
}

uint8_t sdFindFileStruct(const char* name)
{
  while(fat_read_dir(SD_dir, &SD_dir_entry))
    {
      if(strcmp(SD_dir_entry.long_name, name) == 0)
        {
          if (fat_reset_dir(SD_dir))
            return 1;
        }
    }
  return 0;
}

uint8_t sdDeleteFile(const char* name)
{
  return (sdFindFileStruct(name) && fat_delete_file(SD_filesystem, &SD_dir_entry));
}

bool listSdFiles(const char *path, const char *extension, const uint8_t maxlen, const char *selection, uint8_t flags=0)
{

  static uint16_t lastpopupMenuOffset = 0;

  // we must close the logs as we reuse the same SDfile structure
  closeLogIfActived();

  if (popupMenuOffset == 0)
    {
      lastpopupMenuOffset = 0;
      memset(reusableBuffer.modelsel.menu_bss, 0, sizeof(reusableBuffer.modelsel.menu_bss));
    }
  else if (popupMenuOffset == popupMenuNoItems - MENU_MAX_DISPLAY_LINES)
    {
      lastpopupMenuOffset = 0xffff;
      memset(reusableBuffer.modelsel.menu_bss, 0, sizeof(reusableBuffer.modelsel.menu_bss));
    }
  else if (popupMenuOffset == lastpopupMenuOffset)
    {
      // should not happen, only there because of Murphy's law
      return true;
    }
  else if (popupMenuOffset > lastpopupMenuOffset)
    {
      memmove(reusableBuffer.modelsel.menu_bss[0], reusableBuffer.modelsel.menu_bss[1], (MENU_MAX_DISPLAY_LINES-1)*MENU_LINE_LENGTH);
      memset(reusableBuffer.modelsel.menu_bss[MENU_MAX_DISPLAY_LINES-1], 0xff, MENU_LINE_LENGTH);
    }
  else
    {
      memmove(reusableBuffer.modelsel.menu_bss[1], reusableBuffer.modelsel.menu_bss[0], (MENU_MAX_DISPLAY_LINES-1)*MENU_LINE_LENGTH);
      memset(reusableBuffer.modelsel.menu_bss[0], 0, MENU_LINE_LENGTH);
    }

  popupMenuNoItems = 0;
  POPUP_MENU_ITEMS_FROM_BSS();

  if (sdChangeCurDir(path)) // Open  directory
    {
      if (flags)
        {
          ++popupMenuNoItems;
          if (selection)
            {
              lastpopupMenuOffset++;
            }
          else if (popupMenuOffset==0 || popupMenuOffset < lastpopupMenuOffset)
            {
              char *line = reusableBuffer.modelsel.menu_bss[0];
              memset(line, 0, MENU_LINE_LENGTH);
              strcpy(line, "---");
              popupMenuItems[0] = line;
            }
        }

      while (fat_read_dir(SD_dir, &SD_dir_entry))
        {

          uint8_t len = strlen(SD_dir_entry.long_name);
          if (len < 5 || len > maxlen+4 ||
              strcasecmp(SD_dir_entry.long_name+len-4, extension) ||
              (SD_dir_entry.attributes & FAT_ATTRIB_DIR))
            continue;

          ++popupMenuNoItems;

          SD_dir_entry.long_name[len-4] = '\0';

          if (popupMenuOffset == 0)
            {
              if (selection && strncasecmp(SD_dir_entry.long_name, selection, maxlen) < 0)
                {
                  lastpopupMenuOffset++;
                }
              else
                {
                  for (uint8_t i=0; i<MENU_MAX_DISPLAY_LINES; i++)
                    {
                      char *line = reusableBuffer.modelsel.menu_bss[i];
                      if (line[0] == '\0' || strcasecmp(SD_dir_entry.long_name, line) < 0)
                        {
                          if (i < MENU_MAX_DISPLAY_LINES-1)
                            memmove(reusableBuffer.modelsel.menu_bss[i+1], line, sizeof(reusableBuffer.modelsel.menu_bss[i]) * (MENU_MAX_DISPLAY_LINES-1-i));
                          memset(line, 0, MENU_LINE_LENGTH);
                          strcpy(line, SD_dir_entry.long_name);
                          break;
                        }
                    }
                }
              for (uint8_t i=0; i<min(popupMenuNoItems, (uint16_t)MENU_MAX_DISPLAY_LINES); i++)
                {
                  popupMenuItems[i] = reusableBuffer.modelsel.menu_bss[i];
                }

            }
          else if (lastpopupMenuOffset == 0xffff)
            {
              for (int i=MENU_MAX_DISPLAY_LINES-1; i>=0; i--)
                {
                  char *line = reusableBuffer.modelsel.menu_bss[i];
                  if (line[0] == '\0' || strcasecmp(SD_dir_entry.long_name, line) > 0)
                    {
                      if (i > 0)
                        memmove(reusableBuffer.modelsel.menu_bss[0], reusableBuffer.modelsel.menu_bss[1], sizeof(reusableBuffer.modelsel.menu_bss[i]) * i);
                      memset(line, 0, MENU_LINE_LENGTH);
                      strcpy(line, SD_dir_entry.long_name);
                      break;
                    }
                }
              for (uint8_t i=0; i<min(popupMenuNoItems, (uint16_t)MENU_MAX_DISPLAY_LINES); i++)
                {
                  popupMenuItems[i] = reusableBuffer.modelsel.menu_bss[i];
                }
            }
          else if (popupMenuOffset > lastpopupMenuOffset)
            {
              if (strcasecmp(SD_dir_entry.long_name, reusableBuffer.modelsel.menu_bss[MENU_MAX_DISPLAY_LINES-2]) > 0 && strcasecmp(SD_dir_entry.long_name, reusableBuffer.modelsel.menu_bss[MENU_MAX_DISPLAY_LINES-1]) < 0)
                {
                  memset(reusableBuffer.modelsel.menu_bss[MENU_MAX_DISPLAY_LINES-1], 0, MENU_LINE_LENGTH);
                  strcpy(reusableBuffer.modelsel.menu_bss[MENU_MAX_DISPLAY_LINES-1], SD_dir_entry.long_name);
                }
            }
          else
            {
              if (strcasecmp(SD_dir_entry.long_name, reusableBuffer.modelsel.menu_bss[1]) < 0 && strcasecmp(SD_dir_entry.long_name, reusableBuffer.modelsel.menu_bss[0]) > 0)
                {
                  memset(reusableBuffer.modelsel.menu_bss[0], 0, MENU_LINE_LENGTH);
                  strcpy(reusableBuffer.modelsel.menu_bss[0], SD_dir_entry.long_name);
                }
            }
        }
    }

  if (popupMenuOffset > 0)
    lastpopupMenuOffset = popupMenuOffset;
  else
    popupMenuOffset = lastpopupMenuOffset;

  return popupMenuNoItems;
}

uint8_t setSdModelName(char *filename, uint8_t nummodel)
{
  int8_t i = sizeof(g_model.name)-1;
  uint8_t len = 0;
  while (i>-1)
    {
      if (!len && filename[i])
        len = i+1;
      if (len)
        {
          if (filename[i])
            filename[i] = idx2char(filename[i]);
          else
            filename[i] = '_';
        }
      --i;
    }

  if (len == 0)
    {
      uint8_t num = nummodel + 1;
      strcpy_P(filename, STR_MODEL);
      filename[PSIZE(TR_MODEL)] = (char)((num / 10) + '0');
      filename[PSIZE(TR_MODEL) + 1] = (char)((num % 10) + '0');
      len = PSIZE(TR_MODEL) + 2;
    }
    return len;
}
