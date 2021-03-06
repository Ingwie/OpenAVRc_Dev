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

// #define CORRECT_NEGATIVE_SHIFTS
// open.20.fsguruh; shift right operations do the rounding different for negative values compared to positive values
// so all negative divisions round always further down, which give absolute values bigger compared to a usual division
// this is noticable on the display, because instead of -100.0 -99.9 is shown; While in praxis I doublt somebody will notice a
// difference this is more a mental thing. Maybe people are distracted, because the easy calculations are obviously wrong
// this define would correct this, but costs 34 bytes code for stock version

// currently we set this to active always, because it might cause a fault about 1% compared positive and negative values
// is done now in makefile

int16_t calc100to256_16Bits(int16_t x) // return x*2.56
{
  // y = 2*x + x/2 +x/16-x/512-x/2048
  // 512 and 2048 are out of scope from int8 input --> forget it
#ifdef CORRECT_NEGATIVE_SHIFTS
  int16_t res=(int16_t)x<<1;
  //int8_t  sign=(uint8_t) x>>7;
  int8_t sign=(x<0?1:0);

  x-=sign;
  res+=(x>>1);
  res+=sign;
  res+=(x>>4);
  res+=sign;
  return res;
#else
  return ((int16_t)x<<1)+(x>>1)+(x>>4);
#endif
}

int16_t calc100to256(int8_t x) // return x*2.56
{
  return calc100to256_16Bits(x);
}

int16_t calc100toRESX_16Bits(int16_t x) // return x*10.24
{
#ifdef CORRECT_NEGATIVE_SHIFTS
  int16_t res= ((int16_t)x*41)>>2;
  int8_t sign=(x<0?1:0);
  //int8_t  sign=(uint8_t) x>>7;
  x-=sign;
  res-=(x>>6);
  res-=sign;
  return res;
#else
  // return (int16_t)x*10 + x/4 - x/64;
  return ((x*41)>>2) - (x>>6);
#endif
}

int16_t calc100toRESX(int8_t x) // return x*10.24
{
  return calc100toRESX_16Bits(x);
}

// return x*1.024
int16_t calc1000toRESX(int16_t x) // improve calc time by Pat MacKenzie
{
  // return x + x/32 - x/128 + x/512;
  int16_t y = x>>5;
  x+=y;
  y=y>>2;
  x-=y;
  return x+(y>>2);
}

int16_t calcRESXto1000(int16_t x)  // return x/1.024
{
// *1000/1024 = x - x/32 + x/128
  return (x - (x>>5) + (x>>7));
}

int8_t calcRESXto100(int16_t x)
{
  return (x*25) >> 8;
}

#if defined(HELI) || defined(FRSKY)
uint16_t isqrt32(uint32_t n)
{
  uint16_t c = 0x8000;
  uint16_t g = 0x8000;

  for (;;) {
    if ((uint32_t)g*g > n)
      g ^= c;
    c >>= 1;
    if(c == 0)
      return g;
    g |= c;
  }
}
#endif

/*
  Division by 10 and rounding or fixed point arithmetic values

  Examples:
    value -> result
    105 ->  11
    104 ->  10
   -205 -> -21
   -204 -> -20
*/
getvalue_t div10_and_round(getvalue_t value)
{
  if (value >= 0 ) {
    value += 5;
  } else {
    value -= 5;
  }
  return value/10;
}

getvalue_t div100_and_round(getvalue_t value)
{
  if (value >= 0 ) {
    value += 50;
  } else {
    value -= 50;
  }
  return value/100;
}


#if defined(FRSKY)
void extractLatitudeLongitude(uint32_t * latitude, uint32_t * longitude)
{
  div_t qr = div(telemetryData.value.gpsLatitude_bp, 100);
  *latitude = ((uint32_t)(qr.quot) * 1000000) + (((uint32_t)(qr.rem) * 10000 + telemetryData.value.gpsLatitude_ap) * 5) / 3;

  qr = div(telemetryData.value.gpsLongitude_bp, 100);
  *longitude = ((uint32_t)(qr.quot) * 1000000) + (((uint32_t)(qr.rem) * 10000 + telemetryData.value.gpsLongitude_ap) * 5) / 3;
}

void getGpsPilotPosition()
{
  extractLatitudeLongitude(&telemetryData.value.pilotLatitude, &telemetryData.value.pilotLongitude);
  uint32_t lat = telemetryData.value.pilotLatitude / 10000;
  uint32_t angle2 = (lat*lat) / 10000;
  uint32_t angle4 = angle2 * angle2;
  telemetryData.value.distFromEarthAxis = 139*(((uint32_t)10000000-((angle2*(uint32_t)123370)/81)+(angle4/25))/12500);
  TRACE("telemetryData.value.distFromEarthAxis=%d", telemetryData.value.distFromEarthAxis);
}

void getGpsDistance()
{
  IF_GPS_IS_FIXED
  {
  uint32_t lat, lng;

  extractLatitudeLongitude(&lat, &lng);

  // printf("lat=%d (%d), long=%d (%d)\n", lat, abs(lat - telemetryData.value.pilotLatitude), lng, abs(lng - telemetryData.value.pilotLongitude));

  uint32_t angle = (lat > telemetryData.value.pilotLatitude) ? lat - telemetryData.value.pilotLatitude : telemetryData.value.pilotLatitude - lat;
  uint32_t dist = EARTH_RADIUS * angle / 1000000;
  uint32_t result = dist*dist;

  angle = (lng > telemetryData.value.pilotLongitude) ? lng - telemetryData.value.pilotLongitude : telemetryData.value.pilotLongitude - lng;
  dist = telemetryData.value.distFromEarthAxis * angle / 1000000;
  result += dist*dist;

  dist = abs(TELEMETRY_BARO_ALT_AVAILABLE() ? TELEMETRY_RELATIVE_BARO_ALT_BP : TELEMETRY_RELATIVE_GPS_ALT_BP);
  result += dist*dist;

  telemetryData.value.gpsDistance = isqrt32(result);
  if (telemetryData.value.gpsDistance > telemetryData.value.maxGpsDistance)
    telemetryData.value.maxGpsDistance = telemetryData.value.gpsDistance;
  }
}
#endif
