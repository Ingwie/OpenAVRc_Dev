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


/*  This file could be moded */
/*   From the original one   */
/*  Thanks to Roland Riegel  */
/*   Original header above   */

/*
 * Copyright (c) 2006-2012 by Roland Riegel <feedback@roland-riegel.de>
 *
 * This file is free software; you can redistribute it and/or modify
 * it under the terms of either the GNU General Public License version 2
 * or the GNU Lesser General Public License version 2.1, both as
 * published by the Free Software Foundation.
 */

#ifndef FAT_H
#define FAT_H

#include <stdint.h>
#include "fat_config.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * \addtogroup fat
 *
 * @{
 */
/**
 * \file
 * FAT header (license: GPLv2 or LGPLv2.1)
 *
 * \author Roland Riegel
 */

/**
 * \addtogroup fat_file
 * @{
 */

/** The file is read-only. */
#define FAT_ATTRIB_READONLY (1 << 0)
/** The file is hidden. */
#define FAT_ATTRIB_HIDDEN (1 << 1)
/** The file is a system file. */
#define FAT_ATTRIB_SYSTEM (1 << 2)
/** The file is empty and has the volume label as its name. */
#define FAT_ATTRIB_VOLUME (1 << 3)
/** The file is a directory. */
#define FAT_ATTRIB_DIR (1 << 4)
/** The file has to be archived. */
#define FAT_ATTRIB_ARCHIVE (1 << 5)

/** The given offset is relative to the beginning of the file. */
#define FAT_SEEK_SET 0
/** The given offset is relative to the current read/write position. */
#define FAT_SEEK_CUR 1
/** The given offset is relative to the end of the file. */
#define FAT_SEEK_END 2

/**
 * @}
 */

struct partition_struct;

/**
 * \ingroup fat_file
 * Describes a directory entry.
 */
struct fat_dir_entry_struct
{
    /** The file's name, truncated to 31 characters. */
    char long_name[32];
    /** The file's attributes. Mask of the FAT_ATTRIB_* constants. */
    uint8_t attributes;
#if FAT_DATETIME_SUPPORT
    /** Compressed representation of modification time. */
    uint16_t modification_time;
    /** Compressed representation of modification date. */
    uint16_t modification_date;
#endif
    /** The cluster in which the file's first byte resides. */
    cluster_t cluster;
    /** The file's size. */
    uint32_t file_size;
    /** The total disk offset of this directory entry. */
    offset_t entry_offset;
};

struct fat_fs_struct* fat_open(struct partition_struct* partition);
void fat_close(struct fat_fs_struct* fs);

struct fat_file_struct* fat_open_file(struct fat_fs_struct* fs, const struct fat_dir_entry_struct* dir_entry);
void fat_close_file(struct fat_file_struct* fd);
intptr_t fat_read_file(struct fat_file_struct* fd, uint8_t* buffer, uintptr_t buffer_len);
intptr_t fat_write_file(struct fat_file_struct* fd, const uint8_t* buffer, uintptr_t buffer_len);
uint8_t fat_seek_file(struct fat_file_struct* fd, int32_t* offset, uint8_t whence);
uint8_t fat_resize_file(struct fat_file_struct* fd, uint32_t size);

struct fat_dir_struct* fat_open_dir(struct fat_fs_struct* fs, const struct fat_dir_entry_struct* dir_entry);
void fat_close_dir(struct fat_dir_struct* dd);
uint8_t fat_read_dir(struct fat_dir_struct* dd, struct fat_dir_entry_struct* dir_entry);
uint8_t fat_reset_dir(struct fat_dir_struct* dd);

uint8_t fat_create_file(struct fat_dir_struct* parent, const char* file, struct fat_dir_entry_struct* dir_entry);
uint8_t fat_delete_file(struct fat_fs_struct* fs, struct fat_dir_entry_struct* dir_entry);
uint8_t fat_move_file(struct fat_fs_struct* fs, struct fat_dir_entry_struct* dir_entry, struct fat_dir_struct* parent_new, const char* file_new);
uint8_t fat_create_dir(struct fat_dir_struct* parent, const char* dir, struct fat_dir_entry_struct* dir_entry);
#define fat_delete_dir fat_delete_file
#define fat_move_dir fat_move_file

void fat_get_file_modification_date(const struct fat_dir_entry_struct* dir_entry, uint16_t* year, uint8_t* month, uint8_t* day);
void fat_get_file_modification_time(const struct fat_dir_entry_struct* dir_entry, uint8_t* hour, uint8_t* min, uint8_t* sec);

uint8_t fat_get_dir_entry_of_path(struct fat_fs_struct* fs, const char* path, struct fat_dir_entry_struct* dir_entry);

offset_t fat_get_fs_size(const struct fat_fs_struct* fs);
offset_t fat_get_fs_free(const struct fat_fs_struct* fs);

struct fat_header_struct
{
    offset_t size;

    offset_t fat_offset;
    uint32_t fat_size;

    uint16_t sector_size;
    uint16_t cluster_size;

    offset_t cluster_zero_offset;

    offset_t root_dir_offset;
#if FAT_FAT32_SUPPORT
    cluster_t root_dir_cluster;
#endif
};

struct fat_fs_struct
{
    struct partition_struct* partition;
    struct fat_header_struct header;
    cluster_t cluster_free;
};

struct fat_file_struct
{
    struct fat_fs_struct* fs;
    struct fat_dir_entry_struct dir_entry;
    offset_t pos;
    cluster_t pos_cluster;
};

struct fat_dir_struct
{
    struct fat_fs_struct* fs;
    struct fat_dir_entry_struct dir_entry;
    cluster_t entry_cluster;
    uint16_t entry_offset;
};

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif

