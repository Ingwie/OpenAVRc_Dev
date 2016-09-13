/*
 *************************************************************
 *                      NEXTSTEPRC                           *
 *                                                           *
 *             -> Build your DIY MEGA 2560 TX                *
 *                                                           *
 *      Based on code named                                  *
 *      OpenTx - https://github.com/opentx/opentx            *
 *                                                           *
 *         Only avr code here for lisibility ;-)             *
 *                                                           *
 *  License GPLv2: http://www.gnu.org/licenses/gpl-2.0.html  *
 *                                                           *
 *************************************************************
 */

#include "nextsteprc.h"
#include "serial.h"
#include <stdarg.h>
#include <stdio.h>

#define PRINTF_BUFFER_SIZE    256

void serialPutc(char c)
{
#if defined(USB_SERIAL)
  usbSerialPutc(c);
#else
  serial2Putc(c);
#endif
}

void serialPrintf(const char * format, ...)
{
  va_list arglist;
  char tmp[PRINTF_BUFFER_SIZE];

  va_start(arglist, format);
  vsnprintf(tmp, PRINTF_BUFFER_SIZE, format, arglist);
  va_end(arglist);

  const char *t = tmp;
  while (*t) {
    serialPutc(*t++);
  }
}

void serialCrlf()
{
  serialPutc('\r');
  serialPutc('\n');
}
