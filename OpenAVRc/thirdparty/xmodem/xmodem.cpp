
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
//                                  _                                       //
//     __  __ _ __ ___    ___    __| |  ___  _ __ ___      ___  ___  ___    //
//     \ \/ /| '_ ` _ \  / _ \  / _` | / _ \| '_ ` _ \    / __|| _ \| _ \   //
//      >  < | | | | | || (_) || (_| ||  __/| | | | | | _| (__ |  _/|  _/   //
//     /_/\_\|_| |_| |_| \___/  \__,_| \___||_| |_| |_|(_)\___||_|  |_|     //
//                                                                          //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////
//                                                                          //
//          Copyright (c) 2012 by S.F.T. Inc. - All rights reserved         //
//  Use, copying, and distribution of this software are licensed according  //
//    to the GPLv2, LGPLv2, or BSD license, as appropriate (see COPYING)    //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////

// This version of XMODEM is adapted and optimized for arduino and OpenAVRc by RC-Navy
// -> usage of Stream rather than HardwareSerial and optimization
// ReceiveXmodem() and SendXmodem() use only bytes in the stack (RAM released when transfer finished)
//
// This version of XMODEM by RC-Navy has been moded by Ingwie : (Arduino compatibility is lost)
// Only CRC is used (more secure than Checksum) : This reduce code size and stack usage.
// Reduce timout to offer good performances (we don't use 70' RTC lines ;-)
// Reduce var size (E.G Arduino use 32 bits for time (millis(), OpenAVRc use 16 byte)
// Without this reduction, my free stack was 71 bytes :-( and all is not implemented ...


#ifndef DESKTOP
 #ifdef SIMU
  #include "xmodem.h"
  #include "xmodem_cfg.cpp" /* For a non-arduino environment (such as OpenAVRc), please fill all the macros defined in this file */
 #else
  #include "thirdparty/xmodem/xmodem.h"
  #include "thirdparty/xmodem/xmodem_cfg.cpp" /* For a non-arduino environment (such as OpenAVRc), please fill all the macros defined in this file */
  #include "uCli.h"
 #endif
#else
 #include "../../../OpenAVRc_Desktop/OpenAVRc_DesktopApp.h"
 #include "../../../OpenAVRc_Desktop/OpenAVRc_DesktopMain.h"
 #include "../../../OpenAVRc_Desktop/BluetoothFrame.h"
 #include "../../uCli.h" // for UCLI_CMD_LINE_MAX_SIZE def
 wxGauge* GaugeCpy;
 #ifndef PACK
  #define PACK( __Declaration__ ) __Declaration__ __attribute__((__packed__))
 #endif
#endif

// common definitions (Placed here for FW, SIMU and Desktop)
#define XMODEM_PACKET_SIZE 128

#define SILENCE_TIMEOUT_MS                 300UL /* 1 second */
#define CNX_TIMEOUT_MS                     3000UL
#define CNX_TRY_COUNT_MAX                  (CNX_TIMEOUT_MS / SILENCE_TIMEOUT_MS)
#define FLUSH_TIME_MS                      10UL /* 100 mS */
#define TOTAL_ERROR_COUNT                  5
#define ACK_ERROR_COUNT                    3


#define _SOH_ 1 /* start of packet - note XMODEM-1K uses '2' */
#define _EOT_ 4
#define _ENQ_ 5
#define _ACK_ 6
#define _NAK_ 21 /* NAK character */
#define _CAN_ 24 /* CAN character CTRL+X */

/** \file xmodem.c
  * \brief main source file for S.F.T. XMODEM library
  *
  * S.F.T. XMODEM library
**/

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
  SERIAL_TYPE ser;     // identifies the serial connection, data type is OS-dependent
  FILE_TYPE file;      // identifies the file handle, data type is OS-dependent
  XmodemCBufSt_t cbuf; // XMODEM CRC buffer
} XModemSt_t;

\endcode
  *
**/
typedef struct
{
 SERIAL_TYPE ser;     ///< identifies the serial connection, data type is OS-dependent
 FILE_DESC   fd;      ///< identifies the file handle, data type is OS-dependent
 XModemCBufSt_t buf; ///< XMODEM CRC buffer
} XModemSt_t;

//char iBinaryTransfer = 0, iDisableRXOVER = 0;

/** \ingroup xmodem_internal
  * \brief Calculate checksum for XMODEM packet
  *
  * \param sVal An uint16_t integer to be made 'high endian' by flipping bytes (as needed)
  * \return A (possibly) byte-flipped high-endian uint16_t integer
  *
  * This function assumes low-endian for Arduino, and performs a universal operation
  * for 'indeterminate' architectures.
**/
static uint16_t my_htons(uint16_t sVal)
{
 union
 {
  uint8_t aVal[2];
  uint16_t sVal;
 } a, b;

// tweeked for size and speed.  enjoy.

 b.sVal = sVal;

 a.aVal[0] = b.aVal[1]; // no math involved, pre-optimized code
 a.aVal[1] = b.aVal[0];

 return a.sVal;
}

/** \ingroup xmodem_internal
  * \brief Calculate 16-bit CRC for XMODEM packet
  *
  * \param lpBuf A pointer to the XMODEM data buffer
  * \param cbBuf The length of the XMODEM data buffer (typically 128)
  * \return A high-endian 16-bit (uint16_t) value to be assigned to the 'CRC' element in the XMODEM packet
  *
  * This method uses the 'long way' which is SMALLER CODE for microcontrollers, but eats up a bit more CPU.
  * Otherwise, you'd have to pre-build the 256 byte table and use "the table lookup" method.
**/
uint16_t CalcCRC(const char *lpBuf, uint16_t cbBuf)
{
 uint16_t wCRC;
 uint16_t  i1, i2, iAX;
 char     cAL;

// ** this function returns 2-byte string containing
// ** the CRC calculation result, as high endian
 wCRC = 0;
 for(i1 = 0; i1 < cbBuf; i1++)
  {
   cAL = lpBuf[i1];
   iAX = (uint16_t)cAL << 8;
   wCRC = iAX ^ wCRC;
   for(i2 = 0; i2 < 8; i2++)
    {
     iAX = wCRC;
     if(iAX & 0x8000)
      {
       wCRC <<= 1;
       wCRC ^= 0x1021;
      }
     else
      {
       wCRC <<= 1;
      }
    }
  }

 return my_htons(wCRC);
}

/** \ingroup xmodem_internal
  * \brief Generate a sequence number pair, place into XModemCBufSt_t (the CRC version)
  *
  * \param pBuf A pointer to an XModemCBufSt_t structure
  * \param bSeq An uint8_t, typically cast from an unsigned long 'block number'
  *
  * This function generates the sequence pair for the XMODEM packet.  The 'block number'
  * is initially assigned a value of '1', and increases by 1 for each successful packet.
  * That value is 'truncated' to a single byte and assigned as a sequence number for the
  * packet itself.
**/
void GenerateSEQC(XModemCBufSt_t *pBuf, uint8_t bSeq)
{
 pBuf->aSEQ = bSeq;
 pBuf->aNotSEQ = (255 - bSeq);//~bSeq; these should be the same but for now I do this...
}

/** \ingroup xmodem_internal
  * \brief Get an XMODEM block from the serial device
  *
  * \param ser A 'SERIAL_TYPE' identifier for the serial connection
  * \param pBuf A pointer to the buffer that receives the data
  * \param cbSize The number of bytes/chars to read
  * \return The number of bytes/chars read, 0 if timed out (no data), < 0 on error
  *
  * Call this function to read data from the serial port, specifying the number of
  * bytes to read.  This function times out after no data transferred (silence) for
  * a period of 'SILENCE_TIMEOUT_MS' milliseconds.  This allows spurious data transfers
  * to continue as long as there is LESS THAN 'SILENCE_TIMEOUT_MS' between bytes, and
  * also allows VERY SLOW BAUD RATES (as needed).  However, if the transfer takes longer
  * than '10 times SILENCE_TIMEOUT_MS', the function will return the total number of bytes
  * that were received within that time.
  * The default value of 3 seconds, extended to 10 seconds, allows a worst-case baud
  * rate of about 20. This should not pose a problem. If it does, edit the code.
**/
int16_t GetXmodemBlock(SERIAL_TYPE ser, char *pBuf, int16_t cbSize)
{
 uint16_t ulCur, ulStart;
 int16_t  cb1;
 uint8_t  i1, rx;
 char    *p1;

// ** This function obtains a buffer of 'wSize%' bytes,      **
// ** waiting a maximum of 5 seconds (of silence) to get it. **
// ** It returns this block as a string of 'wSize%' bytes,   **
// ** or a zero length string on error.                      **

//   iDisableRXOVER% = 1; // bug workaround

 p1 = pBuf;
 cb1 = 0;
 ulCur = GET_TICK();

 for(i1 = 0; i1 < cbSize; i1++)
  {
   ulStart = GET_TICK();
   rx = 0;
   do
    {
     YIELD_TO_PRIO_TASK(); // Optionnal: calls non blocking tasks during the silence timeout (see xmodem[_cfg].h)
     if(ser->available())
      {
       *p1 = (char)ser->read();
       rx = 1;
       break; // OK, byte received within 3 seconds
      }
    }
   while((GET_TICK() - ulStart) < (uint16_t)SILENCE_TIMEOUT_MS);
   if(!rx) break; // 5 seconds of "silence" is what fails this
   cb1++;
   p1++;
   if((GET_TICK() - ulCur) > (uint16_t)(10UL * SILENCE_TIMEOUT_MS))  // 10 times SILENCE TIMEOUT_MS for TOTAL TIMEOUT
    {
     break; // took too long, I'm going now
    }
  }

 return cb1; // what I actually read
}

/** \ingroup xmodem_internal
  * \brief Write a single character to the serial device
  *
  * \param ser A 'SERIAL_TYPE' identifier for the serial connection
  * \param bVal The byte to send
  * \return The number of bytes/chars written, or < 0 on error
  *
  * Call this function to write one byte of data to the serial port.  Typically
  * this is used to send things like an ACK or NAK byte.
**/
uint8_t WriteXmodemChar(SERIAL_TYPE ser, uint8_t bVal)
{
 uint8_t iRval;

 iRval = ser->write(bVal);

 return iRval;
}

/** \ingroup xmodem_internal
  * \brief Send an XMODEM block via the serial device
  *
  * \param ser A 'SERIAL_TYPE' identifier for the serial connection
  * \param pBuf A pointer to the buffer that receives the data
  * \param cbSize The number of bytes/chars to write
  * \return The number of bytes/chars written, < 0 on error
  *
  * Call this function to write data via the serial port, specifying the number of
  * bytes to write.
**/
uint8_t WriteXmodemBlock(SERIAL_TYPE ser, const void *pBuf, uint8_t cbSize)
{
 uint8_t iRval;

 iRval = ser->write((const uint8_t *)pBuf, cbSize);

 return iRval;
}

/** \ingroup xmodem_internal
  * \brief Read all input from the serial port until there is 1 second of 'silence'
  *
  * \param ser A 'SERIAL_TYPE' identifier for the serial connection
  *
  * Call this function to read ALL data from the serial port, until there is a period
  * with no data (i.e. 'silence') for 1 second.  At that point the function will return.
  * Some operations require that any bad data be flushed out of the input to prevent
  * synchronization problems.  By using '1 second of silence' it forces re-synchronization
  * to occur in one shot, with the possible exception of VERY noisy lines.  The down side
  * is that it may slow down transfers with a high data rate.
**/
void XModemFlushInput(SERIAL_TYPE ser)
{
 uint16_t ulStart;

 ulStart = GET_TICK();
 do
  {
   YIELD_TO_PRIO_TASK();
   if(ser->available())
    {
     ser->read(); // don't care about the data
     ulStart = GET_TICK(); // reset time
    }
   else
    {
     DELAY_MS(1);
    }
  }
 while((GET_TICK() - ulStart) < (uint16_t)FLUSH_TIME_MS);
}

/** \ingroup xmodem_internal
  * \brief Terminate the XMODEM connection
  *
  * \param pX A pointer to the 'XModemSt_t' object identifying the transfer
  *
  * Call this function prior to ending the XMODEM transfer.  Currently the only
  * thing it does is flush the input.
**/
void XmodemTerminate(XModemSt_t *pX)
{
 XModemFlushInput(pX->ser);
// TODO:  close files? YES
}

/** \ingroup xmodem_internal
  * \brief Validate the sequence number of a received XMODEM block (CRC version)
  *
  * \param pX A pointer to an 'XModemCBufSt_t'
  * \param bSeq The expected sequence number (block & 255)
  * \return A zero value on success, non-zero otherwise
  *
  * Call this function to validate a packet's sequence number against the block number
**/
uint8_t ValidateSEQC(XModemCBufSt_t *pX, uint8_t bSeq)
{
 return pX->aSEQ != 255 - pX->aNotSEQ || // ~(pX->aNotSEQ) ||
        pX->aSEQ != bSeq; // returns TRUE if not valid
}

/** \ingroup xmodem_internal
  * \brief Generic function to receive a file via XMODEM (CRC or Checksum)
  *
  * \param pX A pointer to an 'XModemBufSt_t' with valid bCRC, ser, and file members
  * \return A zero value on success, negative on error, positive on cancel
  *
  * The calling function will need to poll for an SOH from the server using 'C' and 'NAK'
  * characters (as appropriate) until an SOH is received.  That value must be assigned
  * to the 'buf' union (as appropriate), and the bCRC member assigned to non-zero if
  * the server responded to 'C', or zero if it responded to 'NAK'.  With the bCRC,
  * ser, and file members correctly assigned, call THIS function to receive content
  * via XMODEM and write it to 'file'.
  * This function will return zero on success, a negative value on error, and a positive
  * value if the transfer was canceled by the server.
**/
int8_t ReceiveXmodem(XModemSt_t *pX)
{
 uint8_t ecount, ec2;
 int32_t etotal, filesize, block;
 uint8_t cY; // the char to send in response to a packet
 ecount = 0;
 etotal = 0;
 filesize = 0;
 block = 1;

#ifdef DESKTOP
 GaugeCpy->Pulse();
#endif // DESKTOP
// ** already got the first 'SOH' character on entry to this function **

 pX->buf.cSOH = (char)1; // assumed already got this, put into buffer
 do
  {
   if((( GetXmodemBlock(pX->ser, ((char *)&(pX->buf)+ 1) , (sizeof(pX->buf) - 1)))
      != sizeof(pX->buf) - 1) || ( ValidateSEQC(&(pX->buf), block & 255)) ||
      ( CalcCRC(pX->buf.aDataBuf, XMODEM_PACKET_SIZE) != pX->buf.wCRC))

    {
     // did not receive properly
     // TODO:  deal with repeated packet, sequence number for previous packet
     XModemFlushInput(pX->ser);  // necessary to avoid problems
     cY = _NAK_; // TODO do I need this?
     ecount ++; // for this packet
     etotal ++;
    }
   else
    {
     if(FILE_WRITE_CHUNK(pX->fd, &(pX->buf.aDataBuf), XMODEM_PACKET_SIZE) != XMODEM_PACKET_SIZE)
      {
       return -2; // write error on output file
      }
     cY = _ACK_; // send ACK
     block ++;
     filesize += XMODEM_PACKET_SIZE; // TODO:  need method to avoid extra crap at end of file
     ecount = 0; // zero out error count for next packet
    }
   ec2 = 0;   //  ** error count #2 **
   while(ecount < TOTAL_ERROR_COUNT && ec2 < ACK_ERROR_COUNT) // ** loop to get SOH or EOT character **
    {
     WriteXmodemChar(pX->ser, cY); // ** output appropriate command char **
     if(GetXmodemBlock(pX->ser, &(pX->buf.cSOH), 1) == 1)
      {
       if(pX->buf.cSOH == _CAN_) // ** CTRL-X 'CAN' - terminate
        {
         XmodemTerminate(pX);
         return 1; // terminated
        }
       else if(pX->buf.cSOH == _EOT_) // ** EOT - end
        {
         WriteXmodemChar(pX->ser, _ACK_); // ** send an ACK (most XMODEM protocols expect THIS)
//          WriteXmodemChar(pX->ser, _ENQ_); // ** send an ENQ
         return 0; // I am done
        }
       else if(pX->buf.cSOH == _SOH_) // ** SOH - sending next packet
        {
         break; // leave this loop
        }
       else
        {
         // TODO:  deal with repeated packet, i.e. previous sequence number
         XModemFlushInput(pX->ser);  // necessary to avoid problems (since the character was unexpected)
         // if I was asking for the next block, and got an unexpected character, do a NAK; otherwise,
         // just repeat what I did last time
         if(cY == _ACK_) // ACK
          {
           cY = _NAK_; // NACK
          }
         ec2++;
        }
      }
     else
      {
       ecount++; // increase total error count, and try writing the 'ACK' or 'NACK' again
      }
    }
   if(ec2 >= ACK_ERROR_COUNT) // wasn't able to get a packet
    {
     break;
    }
  }
 while(ecount < TOTAL_ERROR_COUNT);
 XmodemTerminate(pX);
#ifdef DESKTOP
 GaugeCpy->SetRange(100);
 GaugeCpy->SetValue(100);
#endif // DESKTOP

 return 1; // terminated
}


/** \ingroup xmodem_internal
  * \brief Generic function to send a file via XMODEM (CRC or Checksum)
  *
  * \param pX A pointer to an 'XModemBufSt_t' with valid ser, and file members, and the polled
  * 'NAK' value assigned to the cSOH member (first byte) within the 'buf' union.
  * \return A zero value on success, negative on error, positive on cancel
  *
  * The calling function will need to poll for a 'C' or NAK from the client (as appropriate)
  * and assign that character to the cSOH member in the 'buf' union (either xbuf or xcbuf since
  * the 'cSOH' will always be the first byte).  Then call this function to send content
  * via XMODEM from 'file'.
  * It is important to record the NAK character before calling this function since the 'C' or
  * 'NAK' value will be used to determine whether to use CRC or CHECKSUM.
  * This function will return zero on success, a negative value on error, and a positive
  * value if the transfer was canceled by the receiver.
**/
int8_t SendXmodem(XModemSt_t *pX)
{
 uint8_t ecount, ec2;
 uint8_t i1;
 int32_t filesize, filepos, block;

 ecount = 0;
 filesize = 0;
 filepos = 0;
 block = 1;
// ** already got first 'NAK' character on entry as pX->buf.cSOH  **
 filesize = FILE_SIZE(pX->fd);
#ifdef DESKTOP
 GaugeCpy->SetRange(filesize);
#endif // DESKTOP
 do
  {
#ifdef DESKTOP
   GaugeCpy->SetValue(filepos);
#endif // DESKTOP
   // ** depending on type of transfer, place the packet
   // ** into pX->buf with all fields appropriately filled.
   if(filepos >= filesize) // end of transfer
    {
     for(i1 = 0; i1 < CNX_TRY_COUNT_MAX; i1++)
      {
       WriteXmodemChar(pX->ser, _EOT_); // ** send an EOT marking end of transfer
       if(GetXmodemBlock(pX->ser, &(pX->buf.cSOH), 1) != 1) // this takes up to 5 seconds
        {
         // nothing returned - try again?
         // break; // for now I loop, uncomment to bail out
        }
       else if(pX->buf.cSOH == _ENQ_    // an 'ENQ' (apparently some expect this)
               || pX->buf.cSOH == _ACK_ // an 'ACK' (most XMODEM implementations expect this)
               || pX->buf.cSOH == _CAN_) // CTRL-X = TERMINATE
        {
         // both normal and 'abnormal' termination.
         break;
        }
      }
     XmodemTerminate(pX);
     return i1 >= CNX_TRY_COUNT_MAX ? 1 : 0; // return 1 if receiver choked on the 'EOT' marker, else 0 for 'success'
    }

   if(pX->buf.cSOH != (char)_NAK_) // NAK
    {
     // increase error count, bail if it's too much
     ec2++;
    }
   FILE_SEEK(pX->fd, filepos); // in case I'm doing a 'retry' and I have to re-read part of the file
   // fortunately, xbuf and xcbuf are the same through the end of 'aDataBuf' so
   // I can read the file NOW using 'xbuf' for both CRC and CHECKSUM versions
   if((filesize - filepos) >= (int32_t)XMODEM_PACKET_SIZE)
    {
     i1 = FILE_READ_CHUNK(pX->fd, pX->buf.aDataBuf, XMODEM_PACKET_SIZE);
     if(i1 != XMODEM_PACKET_SIZE)
      {
       // TODO:  read error - send a ctrl+x ?
      }
    }
   else
    {
     memset(pX->buf.aDataBuf, '\x1a', XMODEM_PACKET_SIZE); // fill with ctrl+z which is what the spec says
     i1 = FILE_READ_CHUNK(pX->fd, pX->buf.aDataBuf, filesize - filepos);
     if(i1 != (filesize - filepos))
      {
       // TODO:  read error - send a ctrl+x ?
      }
    }
   if (pX->buf.cSOH == _ACK_ || pX->buf.cSOH == _NAK_) /// identifies ACK/NACK with XMODEM CRC
    {
     // calculate the CRC, assign to the packet, and then send it
     pX->buf.cSOH = 1; // must send SOH as 1st char
     pX->buf.wCRC = CalcCRC(pX->buf.aDataBuf, XMODEM_PACKET_SIZE);
     GenerateSEQC(&(pX->buf), block);
     // send it
     i1 = WriteXmodemBlock(pX->ser, &(pX->buf), sizeof(pX->buf));
     if(i1 != sizeof(pX->buf)) // write error
      {
       // TODO:  handle write error (send ctrl+X ?)
      }
    }
   ec2 = 0;
   while(ecount < TOTAL_ERROR_COUNT && ec2 < ACK_ERROR_COUNT) // loop to get ACK or NACK
    {
     if(GetXmodemBlock(pX->ser, &(pX->buf.cSOH), 1) == 1)
      {
       if(pX->buf.cSOH == _CAN_) // ** CTRL-X - terminate
        {
         XmodemTerminate(pX);
         return 1; // terminated
        }
       else if(pX->buf.cSOH == _NAK_) // ** CRC NACK
        {
         break;  // exit inner loop and re-send packet
        }
       else if(pX->buf.cSOH == _ACK_) // ** ACK - sending next packet
        {
         filepos += XMODEM_PACKET_SIZE;
         block++; // increment file position and block count

         break; // leave inner loop, send NEXT packet
        }
       else
        {
         XModemFlushInput(pX->ser);  // for now, do this here too
         ec2++;
        }
      }
     else
      {
       ecount++; // increase total error count, then loop back and re-send packet
       break;
      }
    }
   if(ec2 >= ACK_ERROR_COUNT)
    {
     break;  // that's it, I'm done with this
    }
  }
 while(ecount < TOTAL_ERROR_COUNT * 2);   // twice error count allowed for sending

// ** at this point it is important to indicate the errors
// ** and flush all buffers, and terminate process!
 XmodemTerminate(pX);

 return -2; // exit on error
}


/** \ingroup xmodem_internal
  * \brief Calling function for ReceiveXmodem
  *
  * \param pX A pointer to an 'XModemBufSt_t' with valid ser, and file members
  * \return A zero value on success, negative on error, positive on cancel
  *
  * This is a generic 'calling function' for ReceiveXmodem that checks for
  * a response to 'NAK' character, and sets up the XMODEM transfer
  * for CRC mode.
  * This function will return zero on success, a negative value on error, and a positive
  * value if the transfer was canceled by the receiver.
**/
int8_t XReceiveSub(XModemSt_t *pX)
{
 uint8_t i1;

 for(i1 = 0; i1 < CNX_TRY_COUNT_MAX; i1++)
  {
   WriteXmodemChar(pX->ser, _NAK_); // switch to NAK for XMODEM Checksum
   if(GetXmodemBlock(pX->ser, &(pX->buf.cSOH), 1) == 1)
    {
     if(pX->buf.cSOH == _SOH_) // SOH - packet is on its way
      {
       return ReceiveXmodem(pX);
      }
     else if(pX->buf.cSOH == _EOT_) // an EOT [blank file?  allow this?]
      {
       return 1; // canceled
      }
     else if(pX->buf.cSOH == _CAN_) // cancel
      {
       return 1; // canceled
      }
    }
  }
 XmodemTerminate(pX);

 return -3; // fail
}


/** \ingroup xmodem_internal
  * \brief Calling function for SendXmodem
  *
  * \param pX A pointer to an 'XModemSt_t' with valid ser, and file members
  * \return A zero value on success, negative on error, positive on cancel
  *
  * This is a generic 'calling function' for SendXmodem that checks for polls by the
  * receiver, and places the 'NAK' or 'C' character into the 'buf' member of the XMODEM
  * structure so that SendXmodem can use the correct method, either CRC or CHECKSUM mode.
  * This function will return zero on success, a negative value on error, and a positive
  * value if the transfer was canceled by the receiver.
**/
int8_t XSendSub(XModemSt_t *pX)
{
 uint16_t ulStart;

// waiting up to 10 seconds for transfer to start. This is part of the spec?
 ulStart = GET_TICK();
 do
  {
   if(GetXmodemBlock(pX->ser, &(pX->buf.cSOH), 1) == 1)
    {
     if(pX->buf.cSOH == _NAK_) // NAK - XMODEM CHECKSUM
      {
       return SendXmodem(pX);
      }
     else if(pX->buf.cSOH == _CAN_) // cancel
      {
       return 1; // canceled
      }
    }
  }
 while((GET_TICK() - ulStart) < (uint16_t)CNX_TIMEOUT_MS);   // 10 seconds
 XmodemTerminate(pX);

 return -3; // fail
}

int8_t XReceive(SERIAL_TYPE pSer, const char *szFilename)
{
 int8_t iRval;
 XModemSt_t xx;

 xx.ser = pSer;
 xx.fd = NULL;
 if(FILE_EXISTS(szFilename))
  {
   //FILE_DELETE(szFilename);
   return -8; // Don't overwrite
  }
 xx.fd = FILE_OPEN_FOR_WRITE(szFilename);
 if(!xx.fd)
  {
   return -9; // can't create file
  }
 iRval = XReceiveSub(&xx);
 FILE_CLOSE(xx.fd);
 if(iRval)
  {
   WriteXmodemChar(pSer, _CAN_); // cancel (make sure)
   FILE_DELETE(szFilename); // delete file on error
  }
 return iRval;
}

int8_t XSend(SERIAL_TYPE pSer, const char *szFilename)
{
 int8_t iRval;
 XModemSt_t xx;

 xx.ser = pSer;
 xx.fd = NULL;
 xx.fd = FILE_OPEN_FOR_READ(szFilename);
 if(!xx.fd)
  {
   return -9; // can't open file
  }
 iRval = XSendSub(&xx);
 FILE_CLOSE(xx.fd);
 return iRval;
}

#ifdef DESKTOP
void Set_BluetoothFrame_Gauge_Pointer(wxGauge* g)
{
 GaugeCpy = g;
}
#endif
