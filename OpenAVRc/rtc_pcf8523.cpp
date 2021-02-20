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

#include "i2c_master.h"
#include "OpenAVRc.h"

#define RTC_ADDRESS  (0x68 << 1) // PCF8523

#define PCF8523_CONTROL_1     0x00
#define PCF8523_CONTROL_2     0x01
#define PCF8523_CONTROL_3     0x02
#define PCF8523_SECONDS       0x03
#define PCF8523_MINUTES       0x04
#define PCF8523_HOURS         0x05
#define PCF8523_DAYS          0x06
#define PCF8523_WEEKDAYS      0x07
#define PCF8523_MONTHS        0x08
#define PCF8523_YEARS         0x09
#define PCF8523_MINUTE_ALARM    0x0A
#define PCF8523_HOUR_ALARM      0x0B
#define PCF8523_DAY_ALARM       0x0C
#define PCF8523_WEEKDAY_ALARM   0x0D
#define PCF8523_OFFSET          0x0E
#define PCF8523_TMR_CLKOUT_CTRL   0x0F
#define PCF8523_TMR_A_FREQ_CTRL   0x10
#define PCF8523_TMR_A_REG         0x11
#define PCF8523_TMR_B_FREQ_CTRL   0x12
#define PCF8523_TMR_B_REG         0x13

uint8_t bin2bcd(int8_t bin)
{
  return (uint8_t)(bin + 6 * (bin / 10));
}

int8_t bcd2bin(uint8_t bcd)
{
  return (int8_t)(bcd - 6 * (bcd >> 4));
}

/*-------------------------------------------------*/
/* RTC functions                                   */

void rtcSetTime(struct tm *t)
{
  g_ms100 = 0; // start of next second begins now
  //g_rtcTime = MKTIME(t); needed ?

  uint8_t buf[7];

  buf[0] = bin2bcd(t->tm_sec);
  buf[1] = bin2bcd(t->tm_min);
  buf[2] = bin2bcd(t->tm_hour);
  buf[3] = bin2bcd(t->tm_mday);
  buf[4] = bin2bcd(t->tm_wday);
  buf[5] = bin2bcd(t->tm_mon + 1);
  buf[6] = bin2bcd(t->tm_year - 100);

  i2c_writeReg(RTC_ADDRESS, PCF8523_SECONDS, buf, 7);
}

void rtcInit()
{
  uint8_t buf[7]; // RTC R/W buffer.

  struct offset_reg_t
  {
    signed offset :7;
    unsigned mode :1;
  };

  union
  {
    offset_reg_t offset_reg;
    uint8_t state;
  };

  // Check Oscillator stop flag (Seconds register bit 7).
  if (i2c_readReg(RTC_ADDRESS, PCF8523_SECONDS, &state, 1))
  {
    return; /* Check data corruption and return on IIC error */
  }

  if (state & 0x80) // When oscillator has stopped, reset RTC and set the default time.
  {
    // Perform Software reset.
    state = 0x58; // Software Reset.
    i2c_writeReg(RTC_ADDRESS, PCF8523_CONTROL_1, &state, 1); // Control register 1.

    // Select Crystal load capacitance.
#define  PCF8523_12PF5
#if defined  PCF8523_12PF5
    state = 0x80; // 12.5pF
#else
       state = 0x00; // 7pF
#endif
    i2c_writeReg(RTC_ADDRESS, PCF8523_CONTROL_1, &state, 1); // Control register 1.

    // Battery switch over in standard mode (like DS3231) and Battery low detection.
    state = 0x00;
    i2c_writeReg(RTC_ADDRESS, PCF8523_CONTROL_3, &state, 1); // Control register 3.

    // Offset adjustment. RTC accuracy is determined by specification of crystal.
    offset_reg.offset = +2; // Offset mode 0 (low power). +2 is about 8.68 ppm.
    offset_reg.mode = 0;
    i2c_writeReg(RTC_ADDRESS, PCF8523_OFFSET, &state, 1); // Offset Adjustment.

    state = 0x38; // Tri-state Clock output. Normally open collector. (Maybe DS3231 driver should do this too).
    i2c_writeReg(RTC_ADDRESS, PCF8523_TMR_CLKOUT_CTRL, &state, 1); // Timer Clock Output Control.

    // Reset time to just before "Happy New Year 2021" !.
    buf[0] = 0x58; // Seconds .. Note OSF, bit 7 is reset.
    buf[1] = 0x59; // Minutes.
    buf[2] = 0x23; // Hours.
    buf[4] = 0x04; // Weekday Sunday = 0.
    buf[3] = 0x31; // Day.
    buf[5] = 0x12; // Month January = 1.
    buf[6] = 0x20; // Year.
    // Write buffer to RTC.
    i2c_writeReg(RTC_ADDRESS, PCF8523_SECONDS, buf, 7);

    state = buf[0] & ~0x80; // Try again to reset OSF bit, only possible if oscillator is now running.
    // Datasheet states 0.2 to 2 seconds dependent on crystal, temperature and supply voltage.
    i2c_writeReg(RTC_ADDRESS, PCF8523_SECONDS, &state, 1);
  }

  // Read back the time.
  if (i2c_readReg(RTC_ADDRESS, PCF8523_SECONDS, buf, 7))
  {
    return;
  }

  struct tm utm;

  utm.tm_year = bcd2bin(buf[6]) + 100;       // Offset from 1900
  utm.tm_mon = bcd2bin((buf[5] & 0x1F) - 1); // 12 month
  utm.tm_mday = bcd2bin(buf[3] & 0x3F);      // 31 days
  utm.tm_hour = bcd2bin(buf[2]);             // 23 hours
  utm.tm_min = bcd2bin(buf[1]);              // 59 min
  utm.tm_sec = bcd2bin(buf[0] & 0x7F);       // 59 sec, mask out OSF.
  utm.tm_wday = bcd2bin((buf[4] & 0x07));    // 7 week days

  g_rtcTime = MKTIME(&utm);
#if defined(SIMU)
  g_rtcTime = time(NULL);
#endif
}

uint8_t rtcReadTemp(int16_t *temp)
{
  // The PCF8523 does not have temperature measurement.

  int16_t calc = 0; // Brrrh !.
#if defined(SIMU)
  calc = 2000; // 20.00'C in Simu
#endif

  *temp = calc;
  return 0;
}

