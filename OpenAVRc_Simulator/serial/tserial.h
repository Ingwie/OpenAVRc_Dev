


/* ------------------------------------------------------------------------ --
--                                                                          --
--                        PC serial port connection object                  --
--                           for non-event-driven programs                  --
--                                                                          --
--                                                                          --
--                                                                          --
--  Copyright @ 2001          Thierry Schneider                             --
--                            thierry@tetraedre.com                         --
--                                                                          --
--                                                                          --
--                                                                          --
-- ------------------------------------------------------------------------ --
--                                                                          --
--  Filename : sertest2.cpp                                                 --
--  Author   : Thierry Schneider                                            --
--  Created  : April 4th 2000                                               --
--  Modified : April 8th 2001                                               --
--  Plateform: Windows 95, 98, NT, 2000 (Win32)                             --
-- ------------------------------------------------------------------------ --
--                                                                          --
--  This software is given without any warranty. It can be distributed      --
--  free of charge as long as this header remains, unchanged.               --
--                                                                          --
-- ------------------------------------------------------------------------ */


// Driver modified by Ingwie for OpenAVRc (c) to handle COM ports upper to 9 E.G: COM124, COM10
// use "new" definission "\\\\.\\COMXXX"


/* ---------------------------------------------------------------------- */
#ifndef TSERIAL_H
#define TSERIAL_H

#include <stdio.h>
#include <windows.h>
#include <inttypes.h>

enum serial_parity {spNONE, spODD, spEVEN};


/* -------------------------------------------------------------------- */
/* -----------------------------  Tserial  ---------------------------- */
/* -------------------------------------------------------------------- */
class Tserial
{
 // -------------------------------------------------------- //
protected:
 char              port[12];                      // port name "com1",...
 int               rate;                          // baudrate
 serial_parity     parityMode;
 HANDLE            serial_handle;                 // ...

 // ++++++++++++++++++++++++++++++++++++++++++++++
 // .................. EXTERNAL VIEW .............
 // ++++++++++++++++++++++++++++++++++++++++++++++
public:
 Tserial();
 ~Tserial();
 int           connect          (char *port_arg, int rate_arg,
                                 serial_parity parity_arg);
 void          sendChar         (char c);
 void          sendArray        (char *buffer, int len);
 char          getChar          (void);
 int           getArray         (char *buffer, int len);
 int           getNbrOfBytes    (void);
 void          disconnect       (void);


 //////////// ADDS FOR XMODEM SUPPORT //////////////// Ingwie
 uint8_t available();
 char read();
 uint16_t write(char c);
 uint16_t write(const uint8_t * buffer, uint16_t len);
};
/* -------------------------------------------------------------------- */

#endif


