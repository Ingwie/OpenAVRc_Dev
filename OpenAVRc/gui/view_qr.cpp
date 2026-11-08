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
 *       GPS last-position QR view for 128x64 telemetry displays.         *
 *                                                                        *
 *   OpenAVRc is free software: you can redistribute it and/or modify     *
 *   it under the terms of the GNU General Public License as published by *
 *   the Free Software Foundation, either version 2 of the License, or    *
 *   (at your option) any later version.                                  *
 *                                                                        *
 *****************************************************************************
 */


#include "../OpenAVRc.h"
#include "view_qr.h"

#if defined(GPS)

#define QR_DATA_CODEWORDS   55
#define QR_ECC_CODEWORDS    15
#define QR_TOTAL_CODEWORDS  70
#define QR_MAX_TEXT_BYTES   53

#define QR_LCD_X            3
#define QR_LCD_Y            3
#define QR_LCD_SCALE        2

static const char STR_QR_NO_GPS[] PROGMEM = "NO GPS";
static const char STR_QR_FIX[] PROGMEM = "FIX";
static const char STR_QR_LAST_GPS[] PROGMEM = "LAST GPS";
static const char STR_QR_ALT[] PROGMEM = "ALT";
static const char STR_QR_DIST[] PROGMEM = "DIST";
static const char STR_QR_HDG[] PROGMEM = "HDG";
static const char STR_QR_EXIT[] PROGMEM = "[EXIT]";
static const char QR_GOOGLE_PREFIX[] PROGMEM = "https://maps.google.com/?q=";

/*
 * Reed-Solomon generator polynomial for QR Version 3-L, 15 ECC codewords.
 * Keeping it in flash avoids a 15-byte temporary divisor on the AVR stack.
 */
static const uint8_t QR_RS_DIVISOR[QR_ECC_CODEWORDS] PROGMEM = {
  29, 196, 111, 163, 112, 74, 10, 105, 105, 139, 132, 151, 32, 134, 26
};


/*
 * The QR payload is intentionally short enough for QR Version 3-L:
 *
 *   https://maps.google.com/?q=48.85660,2.35220
 *
 * Five decimal degrees give roughly metre-level coordinate resolution while
 * keeping the complete Google Maps URL below the Version 3-L byte capacity.
 */

static uint16_t qrModuleIndex(uint8_t x, uint8_t y)
{
  return (uint16_t)y * QR_SIZE + x;
}

static uint8_t qrGetModule(const uint8_t *matrix, uint8_t x, uint8_t y)
{
  uint16_t bit = qrModuleIndex(x, y);
  return (matrix[bit >> 3] >> (bit & 7)) & 1U;
}

static void qrSetModule(uint8_t *matrix, uint8_t x, uint8_t y, uint8_t black)
{
  uint16_t bit = qrModuleIndex(x, y);
  uint8_t mask = (uint8_t)(1U << (bit & 7));

  if (black)
    matrix[bit >> 3] |= mask;
  else
    matrix[bit >> 3] &= (uint8_t)~mask;
}

/*
 * Version 3 has fixed function-module locations.  Computing this predicate
 * removes the former 116-byte functionModules[][] temporary bitmap.
 */
static uint8_t qrIsFunctionModule(uint8_t x, uint8_t y)
{
  if (x == 6 || y == 6)
    return 1;

  // Finder patterns, separators and format information.
  if (x <= 8 && y <= 8)
    return 1;
  if (x >= QR_SIZE - 8 && y <= 8)
    return 1;
  if (x <= 8 && y >= QR_SIZE - 8)
    return 1;

  // Version 3 alignment pattern centred at (22,22).
  if (x >= 20 && x <= 24 && y >= 20 && y <= 24)
    return 1;

  return 0;
}

static void qrDrawFinder(uint8_t cx, uint8_t cy)
{
  int8_t dy;
  int8_t dx;

  for (dy = -4; dy <= 4; ++dy)
  {
    for (dx = -4; dx <= 4; ++dx)
    {
      int8_t xx = (int8_t)cx + dx;
      int8_t yy = (int8_t)cy + dy;

      if (xx >= 0 && yy >= 0 && xx < QR_SIZE && yy < QR_SIZE)
      {
        uint8_t ax = (uint8_t)(dx < 0 ? -dx : dx);
        uint8_t ay = (uint8_t)(dy < 0 ? -dy : dy);
        uint8_t dist = ax > ay ? ax : ay;
        qrSetModule(ReBuff.qrView.qrModules, (uint8_t)xx, (uint8_t)yy,
                    (dist != 2 && dist != 4));
      }
    }
  }
}

static void qrDrawAlignment(uint8_t cx, uint8_t cy)
{
  int8_t dy;
  int8_t dx;

  for (dy = -2; dy <= 2; ++dy)
  {
    for (dx = -2; dx <= 2; ++dx)
    {
      uint8_t ax = (uint8_t)(dx < 0 ? -dx : dx);
      uint8_t ay = (uint8_t)(dy < 0 ? -dy : dy);
      uint8_t dist = ax > ay ? ax : ay;

      qrSetModule(ReBuff.qrView.qrModules,
                  (uint8_t)((int8_t)cx + dx),
                  (uint8_t)((int8_t)cy + dy),
                  dist != 1);
    }
  }
}

static void qrDrawFormatBits()
{
  // Error correction L = 01, mask pattern 0.
  uint16_t data = (uint16_t)(1U << 3);
  uint16_t rem = data;
  uint8_t i;

  for (i = 0; i < 10; ++i)
    rem = (uint16_t)((rem << 1) ^ (((rem >> 9) & 1U) ? 0x0537U : 0U));

  uint16_t bits = (uint16_t)(((data << 10) | rem) ^ 0x5412U);

  for (i = 0; i <= 5; ++i)
    qrSetModule(ReBuff.qrView.qrModules, 8, i, (bits >> i) & 1U);

  qrSetModule(ReBuff.qrView.qrModules, 8, 7, (bits >> 6) & 1U);
  qrSetModule(ReBuff.qrView.qrModules, 8, 8, (bits >> 7) & 1U);
  qrSetModule(ReBuff.qrView.qrModules, 7, 8, (bits >> 8) & 1U);

  for (i = 9; i < 15; ++i)
    qrSetModule(ReBuff.qrView.qrModules, (uint8_t)(14 - i), 8, (bits >> i) & 1U);

  for (i = 0; i < 8; ++i)
    qrSetModule(ReBuff.qrView.qrModules, (uint8_t)(QR_SIZE - 1 - i), 8, (bits >> i) & 1U);

  for (i = 8; i < 15; ++i)
    qrSetModule(ReBuff.qrView.qrModules, 8, (uint8_t)(QR_SIZE - 15 + i), (bits >> i) & 1U);

  // Fixed dark module.
  qrSetModule(ReBuff.qrView.qrModules, 8, QR_SIZE - 8, 1);
}

static uint8_t qrGfMultiply(uint8_t x, uint8_t y)
{
  uint16_t z = 0;
  uint8_t i;

  for (i = 0; i < 8; ++i)
  {
    z = (uint16_t)((z << 1) ^ ((z & 0x80U) ? 0x011DU : 0U));
    if (y & (uint8_t)(1U << (7 - i)))
      z ^= x;
  }

  return (uint8_t)z;
}

static void qrAppendBits(uint8_t data[QR_TOTAL_CODEWORDS], uint16_t *bitLength,
                         uint16_t value, uint8_t count)
{
  uint8_t i;

  for (i = 0; i < count; ++i)
  {
    uint16_t bit = (*bitLength)++;
    if (value & (uint16_t)(1U << (count - 1 - i)))
      data[bit >> 3] |= (uint8_t)(1U << (7 - (bit & 7)));
  }
}

static void qrAppendByte(uint8_t data[QR_TOTAL_CODEWORDS], uint16_t *bitLength,
                         uint8_t value)
{
  qrAppendBits(data, bitLength, value, 8);
}

static uint8_t gpsQrCoordinateTextLength(int32_t value)
{
  uint32_t absValue = value < 0 ? (uint32_t)(-value) : (uint32_t)value;
  uint16_t whole = (uint16_t)(absValue / 100000UL);
  uint8_t length = value < 0 ? 1 : 0;

  if (whole >= 100)
    length += 3;
  else if (whole >= 10)
    length += 2;
  else
    length += 1;

  // Decimal point + five decimal digits.
  return (uint8_t)(length + 6);
}

static void qrAppendCoordinate(uint8_t data[QR_TOTAL_CODEWORDS], uint16_t *bitLength,
                               int32_t value)
{
  uint32_t absValue;
  uint16_t whole;
  uint32_t frac;
  uint32_t div;
  uint8_t i;

  if (value < 0)
  {
    qrAppendByte(data, bitLength, '-');
    absValue = (uint32_t)(-value);
  }
  else
  {
    absValue = (uint32_t)value;
  }

  whole = (uint16_t)(absValue / 100000UL);

  if (whole >= 100)
    qrAppendByte(data, bitLength, (uint8_t)('0' + whole / 100U));

  if (whole >= 10)
    qrAppendByte(data, bitLength, (uint8_t)('0' + (whole / 10U) % 10U));

  qrAppendByte(data, bitLength, (uint8_t)('0' + whole % 10U));
  qrAppendByte(data, bitLength, '.');

  frac = absValue % 100000UL;
  div = 10000UL;

  for (i = 0; i < 5; ++i)
  {
    qrAppendByte(data, bitLength, (uint8_t)('0' + (frac / div) % 10U));
    div /= 10U;
  }
}

static uint8_t qrEncodeCoordinates(int32_t latitudeE5, int32_t longitudeE5,
                                   uint8_t codewords[QR_TOTAL_CODEWORDS])
{
  uint16_t bitLength = 0;
  uint8_t payloadLength;
  uint8_t i;
  uint8_t pad = 0;
  const char *p;

  payloadLength = (uint8_t)((sizeof(QR_GOOGLE_PREFIX) - 1U) +
                            gpsQrCoordinateTextLength(latitudeE5) + 1U +
                            gpsQrCoordinateTextLength(longitudeE5));

  if (payloadLength > QR_MAX_TEXT_BYTES)
    return 0;

  memclear(codewords, QR_TOTAL_CODEWORDS);

  // Byte mode, Version 1..9 uses an 8-bit character count.
  qrAppendBits(codewords, &bitLength, 0x4, 4);
  qrAppendBits(codewords, &bitLength, payloadLength, 8);

  p = QR_GOOGLE_PREFIX;
  while (1)
  {
    uint8_t c = pgm_read_byte(p++);
    if (!c)
      break;
    qrAppendByte(codewords, &bitLength, c);
  }

  qrAppendCoordinate(codewords, &bitLength, latitudeE5);
  qrAppendByte(codewords, &bitLength, ',');
  qrAppendCoordinate(codewords, &bitLength, longitudeE5);

  // Terminator, then byte alignment.
  {
    uint16_t capacityBits = QR_DATA_CODEWORDS * 8U;
    uint8_t terminator = (capacityBits - bitLength >= 4U) ? 4U :
                         (uint8_t)(capacityBits - bitLength);
    bitLength += terminator;
  }

  bitLength = (uint16_t)((bitLength + 7U) & (uint16_t)~7U);

  i = (uint8_t)(bitLength >> 3);
  while (i < QR_DATA_CODEWORDS)
  {
    codewords[i++] = pad ? 0x11 : 0xEC;
    pad ^= 1U;
  }

  /*
   * Build the Reed-Solomon remainder directly in codewords[55..69].
   * This removes both former 15-byte divisor and 15-byte remainder arrays.
   */
  for (i = 0; i < QR_DATA_CODEWORDS; ++i)
  {
    uint8_t j;
    uint8_t factor = codewords[i] ^ codewords[QR_DATA_CODEWORDS];

    for (j = 0; j + 1 < QR_ECC_CODEWORDS; ++j)
      codewords[QR_DATA_CODEWORDS + j] = codewords[QR_DATA_CODEWORDS + j + 1];
    codewords[QR_TOTAL_CODEWORDS - 1] = 0;

    for (j = 0; j < QR_ECC_CODEWORDS; ++j)
    {
      uint8_t divisor = pgm_read_byte(&QR_RS_DIVISOR[j]);
      codewords[QR_DATA_CODEWORDS + j] ^= qrGfMultiply(divisor, factor);
    }
  }

  return 1;
}

static uint8_t qrBuild(int32_t latitudeE5, int32_t longitudeE5)
{
  uint8_t codewords[QR_TOTAL_CODEWORDS];
  uint8_t i;
  int8_t right;
  uint16_t bitIndex = 0;

  memclear(ReBuff.qrView.qrModules, sizeof(ReBuff.qrView.qrModules));

  if (!qrEncodeCoordinates(latitudeE5, longitudeE5, codewords))
    return 0;

  // Timing patterns.
  for (i = 0; i < QR_SIZE; ++i)
  {
    qrSetModule(ReBuff.qrView.qrModules, 6, i, (i & 1U) == 0);
    qrSetModule(ReBuff.qrView.qrModules, i, 6, (i & 1U) == 0);
  }

  // Finder patterns including their separators.
  qrDrawFinder(3, 3);
  qrDrawFinder(QR_SIZE - 4, 3);
  qrDrawFinder(3, QR_SIZE - 4);

  // Version 3 has one non-overlapping alignment pattern at (22,22).
  qrDrawAlignment(22, 22);

  // Reserve and draw format information for L / mask 0.
  qrDrawFormatBits();

  // Place data and ECC codewords in the standard two-column zig-zag.
  right = QR_SIZE - 1;
  while (right >= 1)
  {
    uint8_t vert;

    if (right == 6)
      right = 5;

    for (vert = 0; vert < QR_SIZE; ++vert)
    {
      uint8_t upward = ((((uint8_t)right + 1U) & 2U) == 0);
      uint8_t y = upward ? (uint8_t)(QR_SIZE - 1 - vert) : vert;
      uint8_t j;

      for (j = 0; j < 2; ++j)
      {
        uint8_t x = (uint8_t)(right - j);

        if (!qrIsFunctionModule(x, y))
        {
          uint8_t black = 0;

          if (bitIndex < QR_TOTAL_CODEWORDS * 8U)
          {
            black = (codewords[bitIndex >> 3] >>
                    (7 - (bitIndex & 7))) & 1U;
            ++bitIndex;
          }

          qrSetModule(ReBuff.qrView.qrModules, x, y, black);
        }
      }
    }

    right -= 2;
  }

  // Apply mask pattern 0 to data/remainder modules only.
  {
    uint8_t y;
    for (y = 0; y < QR_SIZE; ++y)
    {
      uint8_t x;
      for (x = 0; x < QR_SIZE; ++x)
      {
        if (!qrIsFunctionModule(x, y) && (((x + y) & 1U) == 0))
          qrSetModule(ReBuff.qrView.qrModules, x, y, !qrGetModule(ReBuff.qrView.qrModules, x, y));
      }
    }
  }

  // Re-draw format bits after masking.
  qrDrawFormatBits();

  return 1;
}

static int32_t gpsQrCoordinateE5(uint8_t direction, uint16_t bp, uint16_t ap)
{
  uint16_t degrees = bp / 100U;
  uint32_t minute10000 = (uint32_t)(bp % 100U) * 10000UL + ap;

  // DDMM.mmmm -> decimal degrees x 100000.
  // minute10000 / 6 is exactly minutes * 100000 / 60, rounded here.
  int32_t value = (int32_t)degrees * 100000L +
                  (int32_t)((minute10000 + 3UL) / 6UL);

  if (direction == 'S' || direction == 'W')
    value = -value;

  return value;
}

void gpsQrUpdateLastPosition()
{
  IF_GPS_IS_FIXED
  {
    ReBuff.qrView.gpsQrLast.latitudeE5 = gpsQrCoordinateE5(telemetryData.value.gpsLatitudeNS,
                                             telemetryData.value.gpsLatitude_bp,
                                             telemetryData.value.gpsLatitude_ap);
    ReBuff.qrView.gpsQrLast.longitudeE5 = gpsQrCoordinateE5(telemetryData.value.gpsLongitudeEW,
                                              telemetryData.value.gpsLongitude_bp,
                                              telemetryData.value.gpsLongitude_ap);
    ReBuff.qrView.gpsQrLast.altitude = telemetryData.value.gpsAltitude;
    ReBuff.qrView.gpsQrLast.distance = telemetryData.value.gpsDistance;
    ReBuff.qrView.gpsQrLast.heading = telemetryData.value.gpsCourse_bp;
    ReBuff.qrView.gpsQrLast.valid = 1;
  }
}

uint8_t gpsQrHasValidPosition()
{
  return ReBuff.qrView.gpsQrLast.valid;
}

static void gpsQrPrepare()
{
  gpsQrUpdateLastPosition();
  ReBuff.qrView.qrReady = 0;

  if (ReBuff.qrView.gpsQrLast.valid)
    ReBuff.qrView.qrReady = qrBuild(ReBuff.qrView.gpsQrLast.latitudeE5, ReBuff.qrView.gpsQrLast.longitudeE5);
}

static void gpsQrDrawMatrix()
{
  uint8_t y;

  // Keep the 64x64 left half clear around the 58x58 QR image.
  lcdDrawFilledRect(0, 0, 64, 64, SOLID, ERASE);

  if (!ReBuff.qrView.qrReady)
    return;

  for (y = 0; y < QR_SIZE; ++y)
  {
    uint8_t x;
    for (x = 0; x < QR_SIZE; ++x)
    {
      if (qrGetModule(ReBuff.qrView.qrModules, x, y))
      {
        lcdDrawFilledRect(QR_LCD_X + x * QR_LCD_SCALE,
                          QR_LCD_Y + y * QR_LCD_SCALE,
                          QR_LCD_SCALE, QR_LCD_SCALE);
      }
    }
  }
}

static void displayGpsQrScreen()
{
  lcdClear();

  gpsQrDrawMatrix();

  if (!ReBuff.qrView.gpsQrLast.valid || !ReBuff.qrView.qrReady)
  {
    lcdDrawText(68, 8, STR_QR_NO_GPS);
    lcdDrawText(68, 17, STR_QR_FIX);
    lcdDrawTextAtt(78, 55, STR_QR_EXIT, INVERS);
    return;
  }

  lcdDrawText(66, 1, STR_QR_LAST_GPS);

  lcdDrawText(66, 12, STR_QR_ALT);
  lcdPutsTelemetryChannelValue(LCD_W, 12, TELEM_GPSALT-1,
                               ReBuff.qrView.gpsQrLast.altitude, 0);

  lcdDrawText(66, 23, STR_QR_DIST);
  lcdPutsTelemetryChannelValue(LCD_W, 23, TELEM_DIST-1,
                               ReBuff.qrView.gpsQrLast.distance, 0);

  lcdDrawText(66, 34, STR_QR_HDG);
  lcdPutsTelemetryChannelValue(LCD_W, 34, TELEM_HDG-1,
                               ReBuff.qrView.gpsQrLast.heading, 0);

  lcdDrawTextAtt(78, 55, STR_QR_EXIT, INVERS);
}

void menuGpsQr(uint8_t event)
{
  switch (event)
  {
  case EVT_ENTRY:
    gpsQrPrepare();
    break;

  case EVT_KEY_FIRST(KEY_EXIT):
  case EVT_KEY_FIRST(KEY_ENTER):
    killEvents(event);
    popMenu();
    return;
  }

  displayGpsQrScreen();
}

#endif
