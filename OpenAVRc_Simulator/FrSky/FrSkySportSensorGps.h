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


#ifndef _FRSKY_SPORT_SENSOR_GPS_H_
#define _FRSKY_SPORT_SENSOR_GPS_H_

#include "FrSkySportSensor.h"

#define GPS_DEFAULT_ID ID4
#define GPS_DATA_COUNT 7
#define GPS_LAT_LON_DATA_ID   0x0800
#define GPS_ALT_DATA_ID       0x0820
#define GPS_SPEED_DATA_ID     0x0830
#define GPS_COG_DATA_ID       0x0840
#define GPS_DATE_TIME_DATA_ID 0x0850

#define GPS_LAT_LON_DATA_PERIOD   1000
#define GPS_ALT_DATA_PERIOD       500
#define GPS_SPEED_DATA_PERIOD     500
#define GPS_COG_DATA_PERIOD       500
#define GPS_DATE_TIME_DATA_PERIOD 10000;


class FrSkySportSensorGps : public FrSkySportSensor
{
  public:
    FrSkySportSensorGps(SensorId id = GPS_DEFAULT_ID);
    void setData(float lat, float lon, float alt, float speed, float cog, uint8_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t minute, uint8_t second);
    virtual void send(FrSkySportSingleWireSerial& serial, uint8_t id, uint32_t now);
    virtual uint16_t decodeData(uint8_t id, uint16_t appId, uint32_t data);
    float getLat();
    float getLon();
    float getAltitude();
    float getSpeed();
    float getCog();
    uint8_t getYear();
    uint8_t getMonth();
    uint8_t getDay();
    uint8_t getHour();
    uint8_t getMinute();
    uint8_t getSecond();

  private:
    static uint32_t setLatLon(float latLon, bool isLat);
    static uint32_t setDateTime(uint8_t yearOrHour, uint8_t monthOrMinute, uint8_t dayOrSecond, bool isDate);
    uint32_t latData;
    uint32_t lonData;
    int32_t altData;
    uint32_t speedData;
    uint32_t cogData;
    uint32_t dateData;
    uint32_t timeData;
    uint32_t latTime;
    uint32_t lonTime;
    uint32_t altTime;
    uint32_t speedTime;
    uint32_t cogTime;
    uint32_t dateTime;
    uint32_t timeTime;
    float lat;
    float lon;
    float altitude;
    float speed;
    float cog;
    uint8_t year;
    uint8_t month;
    uint8_t day;
    uint8_t hour;
    uint8_t minute;
    uint8_t second;
};

#endif // _FRSKY_SPORT_SENSOR_GPS_H_
