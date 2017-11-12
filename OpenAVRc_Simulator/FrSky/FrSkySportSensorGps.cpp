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



/*  Based on code:

  FrSky telemetry decoder class for Teensy 3.x/LC and 328P based boards (e.g. Pro Mini, Nano, Uno)
  (c) Pawelsky 20160919
  Not for commercial use
*/


#include "FrSkySportSensorGps.h" 

FrSkySportSensorGps::FrSkySportSensorGps(SensorId id) : FrSkySportSensor(id) { }

void FrSkySportSensorGps::setData(float lat, float lon, float alt, float speed, float cog, uint8_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t minute, uint8_t second)
{
  latData = setLatLon(lat, true);
  lonData = setLatLon(lon, false);
  cogData = cog * 100;
  speedData = speed * 1944; // Convert m/s to knots
  altData = alt * 100;
  dateData = setDateTime(year, month, day, true);
  timeData = setDateTime(hour, minute, second, false);
}

uint32_t FrSkySportSensorGps::setLatLon(float latLon, bool isLat)
{
  uint32_t data = (uint32_t)((latLon < 0 ? -latLon : latLon) * 60 * 10000) & 0x3FFFFFFF;
  if(isLat == false) data |= 0x80000000;
  if(latLon < 0) data |= 0x40000000;

  return data;
}

uint32_t FrSkySportSensorGps::setDateTime(uint8_t yearOrHour, uint8_t monthOrMinute, uint8_t dayOrSecond, bool isDate)
{
  uint32_t data = yearOrHour;
  data <<= 8;
  data |= monthOrMinute;
  data <<= 8;
  data |= dayOrSecond;
  data <<= 8;
  if(isDate == true) data |= 0xFF;

  return data;
}

void FrSkySportSensorGps::send(FrSkySportSingleWireSerial& serial, uint8_t id, uint32_t now)
{
  if(sensorId == id)
  {
    switch(sensorDataIdx)
    {
      case 0:
        if(now > latTime)
        {
          latTime = now + GPS_LAT_LON_DATA_PERIOD;
          serial.sendData(GPS_LAT_LON_DATA_ID, latData);
        }
        else
        {
          serial.sendEmpty(GPS_LAT_LON_DATA_ID);
        }
        break;
      case 1:
        if(now > lonTime)
        {
          lonTime = now + GPS_LAT_LON_DATA_PERIOD;
          serial.sendData(GPS_LAT_LON_DATA_ID, lonData);
        }
        else
        {
          serial.sendEmpty(GPS_LAT_LON_DATA_ID);
        }
        break;
      case 2:
        if(now > altTime)
        {
          altTime = now + GPS_ALT_DATA_PERIOD;
          serial.sendData(GPS_ALT_DATA_ID, altData);
        }
        else
        {
          serial.sendEmpty(GPS_ALT_DATA_ID);
        }
        break;
      case 3:
        if(now > speedTime)
        {
          speedTime = now + GPS_SPEED_DATA_PERIOD;
          serial.sendData(GPS_SPEED_DATA_ID, speedData);
        }
        else
        {
          serial.sendEmpty(GPS_SPEED_DATA_ID);
        }
        break;
      case 4:
        if(now > cogTime)
        {
          cogTime = now + GPS_COG_DATA_PERIOD;
          serial.sendData(GPS_COG_DATA_ID, cogData);
        }
        else
        {
          serial.sendEmpty(GPS_COG_DATA_ID);
        }
        break;
      case 5:
        if(now > dateTime)
        {
          dateTime = now + GPS_DATE_TIME_DATA_PERIOD;
          serial.sendData(GPS_DATE_TIME_DATA_ID, dateData);
        }
        else
        {
          serial.sendEmpty(GPS_DATE_TIME_DATA_ID);
        }
        break;
      case 6:
        if(now > timeTime)
        {
          timeTime = now + GPS_DATE_TIME_DATA_PERIOD;
          serial.sendData(GPS_DATE_TIME_DATA_ID, timeData);
        }
        else
        {
          serial.sendEmpty(GPS_DATE_TIME_DATA_ID);
        }
        break;
    }
    sensorDataIdx++;
    if(sensorDataIdx >= GPS_DATA_COUNT) sensorDataIdx = 0;
  }
}

uint16_t FrSkySportSensorGps::decodeData(uint8_t id, uint16_t appId, uint32_t data)
{
  if((sensorId == id) || (sensorId == FrSkySportSensor::ID_IGNORE))
  {
    switch(appId)
    {
      case GPS_LAT_LON_DATA_ID:
        {
          float latLonData = (data & 0x3FFFFFFF) / 10000.0 / 60.0;
          if((data & 0x40000000) > 0) latLonData = -latLonData;                 // is negative?
          if((data & 0x80000000) == 0) lat = latLonData; else lon = latLonData; // is latitude?
        }
        return appId;
      case GPS_ALT_DATA_ID:
        altitude = ((int32_t)data) / 100.0;
        return appId;
      case GPS_SPEED_DATA_ID:
        speed = data / 1944.0; // Convert knots to m/s
        return appId;
      case GPS_COG_DATA_ID:
        cog = data / 100.0;
        return appId;
      case GPS_DATE_TIME_DATA_ID:
        if((data & 0xFF) > 0)  // is date?
        {
          data >>= 8; day = data & 0xFF;
          data >>= 8; month = data & 0xFF;
          data >>= 8; year = data & 0xFF;
        }
        else
        {
          data >>= 8; second = data & 0xFF;
          data >>= 8; minute = data & 0xFF;
          data >>= 8; hour = data & 0xFF;
        }
        return appId;
    }
  }
  return SENSOR_NO_DATA_ID;
}

float FrSkySportSensorGps::getLat() { return lat; }
float FrSkySportSensorGps::getLon() { return lon; }
float FrSkySportSensorGps::getAltitude() { return altitude; }
float FrSkySportSensorGps::getSpeed() { return speed; }
float FrSkySportSensorGps::getCog() { return cog; }
uint8_t FrSkySportSensorGps::getYear() { return year; }
uint8_t FrSkySportSensorGps::getMonth() { return month; }
uint8_t FrSkySportSensorGps::getDay() { return day; }
uint8_t FrSkySportSensorGps::getHour() { return hour; }
uint8_t FrSkySportSensorGps::getMinute() { return minute; }
uint8_t FrSkySportSensorGps::getSecond() { return second; }
