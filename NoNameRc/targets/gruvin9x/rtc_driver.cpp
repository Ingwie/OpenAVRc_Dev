/*
 *************************************************************
 *                      NoNameRc                           *
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

/*--------------------------------------------------------------------------*/
/*  RTC controls                                                            */

#include "../../thirdparty/FatFs/integer.h"
#include "../../i2c_master.h"
#include "../../NoNameRc.h"

#define RTC_ADRESS  (0x68 << 1) //0x68 for ZS042

typedef struct {
        WORD    year;   /* 2000..2099 */
        uint8_t    month;  /* 1..12 */
        uint8_t    mday;   /* 1.. 31 */
        uint8_t    wday;   /* 1..7 */
        uint8_t    hour;   /* 0..23 */
        uint8_t    min;    /* 0..59 */
        uint8_t    sec;    /* 0..59 */
} RTC;

/*-------------------------------------------------*/
/* RTC functions                                   */

uint8_t g9x_rtcGetTime (RTC *rtc)
{
  uint8_t buf[7];

  if (!iic_read(RTC_ADRESS, 0, 7, buf)) return 0;

  rtc->sec = (buf[0] & 0x0F) + ((buf[0] >> 4) & 7) * 10;
  rtc->min = (buf[1] & 0x0F) + (buf[1] >> 4) * 10;
  rtc->hour = (buf[2] & 0x0F) + ((buf[2] >> 4) & 3) * 10;
  rtc->wday = (buf[2] & 0x07);
  rtc->mday = (buf[4] & 0x0F) + ((buf[4] >> 4) & 3) * 10;
  rtc->month = (buf[5] & 0x0F) + ((buf[5] >> 4) & 1) * 10;
  rtc->year = 2000 + (buf[6] & 0x0F) + (buf[6] >> 4) * 10;

  return 1;
}

uint8_t g9x_rtcSetTime (const RTC *rtc)
{
  uint8_t buf[7];

  buf[0] = rtc->sec / 10 * 16 + rtc->sec % 10;
  buf[1] = rtc->min / 10 * 16 + rtc->min % 10;
  buf[2] = rtc->hour / 10 * 16 + rtc->hour % 10;
  buf[3] = rtc->wday & 7;
  buf[4] = rtc->mday / 10 * 16 + rtc->mday % 10;
  buf[5] = rtc->month / 10 * 16 + rtc->month % 10;
  buf[6] = (rtc->year - 2000) / 10 * 16 + (rtc->year - 2000) % 10;
  return iic_write(RTC_ADRESS, 0, 7, buf);
}

void rtcGetTime(struct gtm * utm)
{
  RTC rtc = {0,0,0,0,0,0,0};

  g9x_rtcGetTime(&rtc);

  utm->tm_year = rtc.year - 1900;
  utm->tm_mon =  rtc.month - 1;
  utm->tm_mday = rtc.mday;
  utm->tm_hour = rtc.hour;
  utm->tm_min =  rtc.min;
  utm->tm_sec =  rtc.sec;
  utm->tm_wday = rtc.wday - 1;
}

void rtcSetTime(struct gtm * t)
{
  g_rtcTime = gmktime(t); // update local timestamp and get wday calculated
  g_ms100 = 0; // start of next second begins now

  RTC rtc;
  rtc.year = t->tm_year + 1900;
  rtc.month = t->tm_mon + 1;
  rtc.mday = t->tm_mday;
  rtc.hour = t->tm_hour;
  rtc.min = t->tm_min;
  rtc.sec = t->tm_sec;
  rtc.wday = t->tm_wday + 1;
  g9x_rtcSetTime(&rtc);
}

void rtcInit (void)
{
  uint8_t buf[8];	/* RTC R/W buffer */

  /* Read RTC registers */
  if (!iic_read(RTC_ADRESS, 0, 8, buf)) return;	/* IIC error */

  if (buf[7] & 0x20) {	/* When data has been volatiled, set default time */
          /* Clear nv-ram. Reg[8..63] */
          memset(buf, 0, 8);
          for (uint8_t adr = 8; adr < 64; adr += 8)
                  iic_write(0x0D, adr, 8, buf);
          /* Reset time to Jan 1, '08. Reg[0..7] */
          buf[4] = 1; buf[5] = 1; buf[6] = 8;
          iic_write(0x0D, 0, 8, buf);
  }

  struct gtm utm;
  rtcGetTime(&utm);
  g_rtcTime = gmktime(&utm);
}

