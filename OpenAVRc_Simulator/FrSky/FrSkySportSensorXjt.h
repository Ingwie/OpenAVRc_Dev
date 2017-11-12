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


#ifndef _FRSKY_SPORT_SENSOR_XJT_H_
#define _FRSKY_SPORT_SENSOR_XJT_H_

#include "FrSkySportSensor.h"

#define XJT_DEFAULT_ID ID25

#define XJT_RSSI_DATA_ID          0xF101
#define XJT_ADC1_DATA_ID          0xF102
#define XJT_ADC2_DATA_ID          0xF103
#define XJT_RXBATT_DATA_ID        0xF104
#define XJT_SWR_DATA_ID           0xF105
#define XJT_FAS_VOLTAGE_DATA_ID   0x003B
#define XJT_FAS_CURRENT_DATA_ID   0x0028
#define XJT_FGS_FUEL_DATA_ID      0x0004
#define XJT_FLVS_VOLTAGE_DATA_ID  0x0006
#define XJT_FVAS_ALTITUDE_DATA_ID 0x0021
#define XJT_FVAS_VSI_DATA_ID      0x0030 // Not documented in FrSky spec, added based on OpenTX sources.
#define XJT_GPS_LAT_DATA_ID       0x0023
#define XJT_GPS_LON_DATA_ID       0x0022
#define XJT_GPS_ALTITUDE_DATA_ID  0x0009
#define XJT_GPS_SPEED_DATA_ID     0x0019
#define XJT_GPS_COG_DATA_ID       0x001C
#define XJT_GPS_DATE_DATA_ID      0x0016
#define XJT_GPS_TIME_DATA_ID      0x0018
#define XJT_TAS_ACCX_DATA_ID      0x0024
#define XJT_TAS_ACCY_DATA_ID      0x0025
#define XJT_TAS_ACCZ_DATA_ID      0x0026
#define XJT_TEMS_T1_DATA_ID       0x0002
#define XJT_TEMS_T2_DATA_ID       0x0005
#define XJT_RPMS_RPM_DATA_ID      0x0003

class FrSkySportSensorXjt : public FrSkySportSensor
{
  public:
    FrSkySportSensorXjt(SensorId id = XJT_DEFAULT_ID);
    virtual uint16_t decodeData(uint8_t id, uint16_t appId, uint32_t data);

    // System data
    uint8_t getRssi();
    float getAdc1();
    float getAdc2();
    float getRxBatt();
    uint8_t getSwr();

    // FGS sensor data: fuel in percent
    float getFuel();

    // FLVS sensor data: each cell voltage in volts.
    float getCell1();
    float getCell2();
    float getCell3();
    float getCell4();
    float getCell5();
    float getCell6();
    float getCell7();
    float getCell8();
    float getCell9();
    float getCell10();
    float getCell11();
    float getCell12();

    // FAS sensor data: current in amperes, voltage in volts
    float getCurrent();
    float getVoltage();

    // FVAS sensor data: altitude in meters (can be negative), vertical speed in m/s (can be nevative)
    float getAltitude();
    float getVsi();

    // GPS sensor data: lat/lon in degrees decimal (positive for N/E, negative for S/W), alt in meters (can be negative), speed in m/s, course over ground (cog) in degrees 0-359
    //                  add 2000 to year, hours in 24h format
    float getLat();
    float getLon();
    float getGpsAltitude();
    float getSpeed();
    float getCog();
    uint16_t getYear();
    uint8_t getMonth();
    uint8_t getDay();
    uint8_t getHour();
    uint8_t getMinute();
    uint8_t getSecond();

    // TAS sensor data: X/Y/Z acceleration in Gs (can be negative)
    float getAccX();
    float getAccY();
    float getAccZ();

    // TEMS sensor data: t1/t2 in degrees Celsius (can be negative)
    float getT1();
    float getT2();

    // RPMS sensor data: rpm in rotations per minute. Set number of blades to 2 in your radio
    float getRpm();
    
  private:
    // Temporary variables for collecting telemetry values that are combined from multiple data IDs
    uint16_t voltBD;
    uint16_t voltAD;
    int16_t altBD;
    int16_t altAD;
    char latNS;
    uint16_t latBD;
    uint16_t latAD;
    char lonEW;
    uint16_t lonBD;
    uint16_t lonAD;
    int16_t gpsAltBD;
    int16_t gpsAltAD;
    uint16_t speedBD;
    uint16_t speedAD;
    uint16_t cogBD;
    uint16_t cogAD;

    // Variables holding decoded telemetry data
    uint8_t rssi;
    float adc1;
    float adc2;
    float rxBatt;
    uint8_t swr;
    float fuel;
    float cell[12];
    float current;
    float voltage;
    float altitude;
    float vsi;
    float lat;
    float lon;
    float gpsAltitude;
    float speed;
    float cog;
    uint16_t year;
    uint8_t month;
    uint8_t day;
    uint8_t hour;
    uint8_t minute;
    uint8_t second;
    float accX;
    float accY;
    float accZ;
    float t1;
    float t2;
    float rpm;
};

#endif // _FRSKY_SPORT_SENSOR_XJT_H_
