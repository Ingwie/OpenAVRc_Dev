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
//////////////////////////////////////////////////////////////////////////////
//                                                                          //
//                                     _                      _             //
//        __  __ _ __ ___    ___    __| |  ___  _ __ ___     | |__          //
//        \ \/ /| '_ ` _ \  / _ \  / _` | / _ \| '_ ` _ \    | '_ \         //
//         >  < | | | | | || (_) || (_| ||  __/| | | | | | _ | | | |        //
//        /_/\_\|_| |_| |_| \___/  \__,_| \___||_| |_| |_|(_)|_| |_|        //
//                                                                          //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////
//                                                                          //
//          Copyright (c) 2012 by S.F.T. Inc. - All rights reserved         //
//  Use, copying, and distribution of this software are licensed according  //
//    to the LGPLv2.1, or a BSD-like license, as appropriate (see below)    //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////
#ifndef XMODEM_H
#define XMODEM_H

/** \mainpage S.F.T. XMODEM library (ARDUINO version)
  *
  * Copyright (c) 2012 by S.F.T. Inc. - All rights reserved\n
  *
  * The source files include DOXYGEN SUPPORT to properly document the library
  * Please excuse the additional comments necessary to make this work.
  * Instead, build the doxygen output and view the documentation, as
  * well as the code itself WITHOUT all of the doxygen markup comments.
  * \n
  * \n
  * This library was designed to work with POSIX-compliant operating systems
  * such as Linux, FreeBSD, and OSX, and also on Arduino microcontrollers.
  * The intent was to provide an identical code base for both ends of the
  * XMODEM transfer, compilable as either C or C++ code for maximum flexibility.
  *
  * Normally you will only need to use one of these two functions:\n
  * \n
  * \ref XSend() - send a file via XMODEM\n
  * \ref XReceive() - receive a file via XMODEM\n
  * \n
  * The rest of the documentation was provided to help you debug any problems,
  * or even to write your own library (as appropriate).\n
  *
  * LICENSE
  *
  * This software is licensed under either the LGPLv2 or a BSD-like license.
  * For more information, see\n
  *   http://opensource.org/licenses/BSD-2-Clause\n
  *   http://www.gnu.org/licenses/lgpl-2.1.html\n
  * and the above copyright notice.\n
  * \n
  * In short, you may use this software anyway you like, provided that you
  * do not hold S.F.T. Inc. responsible for consequential or inconsequential
  * damages resulting from use, modification, abuse, or anything else done
  * with this software, and you include the appropriate license (either LGPLv2
  * or a BSD-like license) and comply with the requirements of said license.\n
  * So, if you use a BSD-like license, you can copy the license template at
  * the abovementioned URL and sub in the copyright notice as shown above.
  * Or, you may use an LGPLv2 license, and then provide source files with a
  * re-distributed or derived work (including a complete re-write with this
  * library as a template).  A link back to the original source, of course,
  * would be appreciated but is not required.
**/

/** \file xmodem.h
  * \brief main header file for S.F.T. XMODEM library
  *
  * S.F.T. XMODEM library
**/

/** \defgroup xmodem_api XModem API
  * high-level API functions
*/

/** \defgroup xmodem_internal XModem Internal
  * internal support functions
*/
#ifndef DESKTOP
 #include "../../myeeprom.h"
 #include "../../Serial1.h"
 #include "../../sdcard.h"
#endif

#define FILE_DESC                          struct fat_file_struct*
#define FILE_EXISTS(FullFileName)          sdFileExists((char *)FullFileName)
#define FILE_OPEN_FOR_READ(FullFileName)   sdFileOpenForRead((char *)FullFileName)
#define FILE_OPEN_FOR_WRITE(FullFileName)  sdFileOpenForWrite((char *)FullFileName)
#define FILE_SEEK(fd, Pos)                 fat_seek_file(fd, (int32_t *)&Pos, FAT_SEEK_SET)
#define FILE_READ_CHUNK(fd,  Buf, Size)    fat_read_file(fd, (uint8_t *)Buf, Size)
#define FILE_WRITE_CHUNK(fd, Buf, Size)    fat_write_file(fd, (const uint8_t *)Buf, Size)
#define FILE_SIZE(fd)                      fd->dir_entry.file_size /* /!\ TBC /!\ */
#define FILE_CLOSE(fd)                     fat_close_file(fd);
#define FILE_DELETE(FullFileName)          sdDeleteFile((const char *)FullFileName)

#define DELAY_MS(ms)                       _delay_ms(ms)
#define GET_TICK()                         GET_10MS_TICK()

#define YIELD_TO_PRIO_TASK()               YIELD_TO_TASK(checkMixer(); MYWDT_RESET())
#define XMODEM_PACKET_SIZE 128


/** \ingroup xmodem_internal
  * \brief Structure defining an XMODEM CRC packet
  *
\code
typedef struct{
   char     cSOH;          // ** SOH byte goes here             **
   uint8_t  aSEQ, aNotSEQ; // ** 1st byte = seq#, 2nd is ~seq#  **
   char     aDataBuf[XMODEM_PACKET_SIZE]; // ** the actual data itself!        **
   uint16_t wCRC;          // ** CRC gets 2 bytes, high endian  **
} PACKED XModemCBufSt_t;

\endcode
  *
**/
PACK(typedef struct{
 char     cSOH;          ///< SOH byte goes here
 uint8_t  aSEQ;          ///< 1st byte = seq#
 uint8_t  aNotSEQ;       ///< 2nd is ~seq#
 char     aDataBuf[XMODEM_PACKET_SIZE]; ///< the actual data itself!
 uint16_t wCRC;          ///< CRC gets 2 bytes, high endian
}) XModemCBufSt_t;

/** \ingroup xmodem_internal
  * \brief Structure that identifies the XMODEM communication state
  *
\code
typedef struct{
  FILE_TYPE file;      // identifies the file handle, data type is OS-dependent
  XmodemCBufSt_t cbuf; // XMODEM CRC buffer
} XModemSt_t;

\endcode
  *
**/
typedef struct
{
 FILE_DESC   fd;      ///< identifies the file handle, data type is OS-dependent
 XModemCBufSt_t buf; ///< XMODEM CRC buffer
} XModemSt_t;


/** \ingroup xmodem_api
  * \brief Receive a file using XMODEM protocol (ARDUINO version and other environment if defined in xmodem_cfg.h)
  *
  * \param szFilename A pointer to a (const) 0-byte terminated string containing the file name
  * \return A value of zero on success, negative on failure, positive if canceled
  *
  * Call this function to receive a file, passing the pointer to the 'Stream' object to be used for
  * serial communication, and the name of the file to create from the XMODEM stream.  The function
  * will return a value of zero on success.  On failure or cancelation, the file will be deleted.\n
  * If the specified file exists before calling this function, it will be overwritten.  If you do not
  * want to unconditionally overwrite an existing file, you should test to see if it exists first
  * using the SD library.
  *
**/
int8_t XReceive(const char *szFilename);


/** \ingroup xmodem_api
  * \brief Send a file using XMODEM protocol (ARDUINO version and other environment if defined in xmodem_cfg.h)
  *
  * \param szFilename A pointer to a (const) 0-byte terminated string containing the file name
  * \return A value of zero on success, negative on failure, positive if canceled
  *
  * Call this function to send a file, passing the pointer to the 'Stream' object to be used for
  * serial communication, and the name of the file to send via the XMODEM stream.  The function
  * will return a value of zero on success.  If the file does not exist, the function will return
  * a 'failure' value and cancel the transfer.
  *
**/
int8_t XSend(const char *szFilename);

#endif
