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



/* Original file from
  FrSky Telemetry for Teensy 3.x/LC and 328P/168 based boards (e.g. Pro Mini, Nano, Uno)
  (c) Pawelsky 20170831
  Not for commercial use
*/

#ifndef _FRSKY_TELEMETRY_H_
#define _FRSKY_TELEMETRY_H_

#include "../OpenAVRc/OpenAVRc.h"

#define SENSOR_NONE 0x0000
#define SENSOR_FAS  0x0001
#define SENSOR_FGS  0x0002
#define SENSOR_FLVS 0x0004
#define SENSOR_FVAS 0x0008
#define SENSOR_GPS  0x0010
#define SENSOR_TAS  0x0020
#define SENSOR_TEMS 0x0040
#define SENSOR_RPMS 0x0080

extern void processSerialData(uint8_t data);

void FrSkyTelemetry_loop();

class FrSkyTelemetry
{
  public:
    // Constructor - used to create the telemetry object
    FrSkyTelemetry();
    uint32_t currentTime;

    // Start the telemetry
    void begin();
    // NOTE: the set methods below do not need to be called on every loop - only when data changes
    // Set the FGS sensor data: fuel in percent
    void setFgsData(float fuel);
    // Set the FLVS sensor data: each cell voltage in volts. Skip non-existing cells
    void setFlvsData(float cell1, float cell2 = 0.0, float cell3 = 0.0, float cell4 = 0.0, float cell5 = 0.0, float cell6 = 0.0,
                     float cell7 = 0.0, float cell8 = 0.0, float cell9 = 0.0, float cell10 = 0.0, float cell11 = 0.0, float cell12 = 0.0);
    // Set the FAS sensor data: current in amperes, voltage in volts
    void setFasData(float current, float voltage);
    // Set the FVAS sensor data: altitude in meters (can be negative), vertical speed in m/s (can be nevative, 0.0m/s will be set when skipped)
    void setFvasData(float altitude, float vsi = 0.0);
    // Set the GPS sensor data: lat/lon in degrees decimal (positive for N/E, negative for S/W), alt in meters (can be negative), speed in m/s, course over ground (cog) in degrees 0-359
    //                          subtract 2000 from year, hours in 24h format
    void setGpsData(float lat, float lon, float alt, float speed, float cog, uint16_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t minute, uint8_t second);
    // Set the TAS sensor data: X/Y/Z acceleration in Gs (can be negative)
    void setTasData(float accX, float accY, float accZ);
    // Set the TEMS sensor data: t1/t2 in degrees Celsius (can be negative)
    void setTemsData(float t1, float t2);
    // Set the RPMS sensor data: rpm in rotations per minute. Set number of blades to 2 in your radio
    void setRpmsData(float rpm);
    // Send the telemetry data, needs to be called in every loop (but data will only be sent in defined time periods and only for sensors for which the set method was called at least once)
    void send();
    // Send to simu serial register
    void sendToSimu(uint8_t data);

  private:
    uint16_t enabledSensors;
    uint8_t  cellIdx;
    uint32_t frame1Time;
    uint32_t frame2Time;
    uint32_t frame3Time;
    //Stream* port;

    // FAS sensor
    uint16_t voltageBD;
    uint16_t voltageAD;
    uint16_t current;

    // FGS sensor
    uint16_t fuel;

    // FLVS sensor
    uint16_t cell[12];

    // FVAS sensor
    int16_t  altBD;
    uint16_t altAD;
    int16_t  vsi;

    // GPS sensor
    int16_t  latBD;
    uint16_t latAD;
    uint16_t latNS;
    int16_t  lonBD;
    uint16_t lonAD;
    uint16_t lonEW;
    int16_t  gpsAltBD;
    uint16_t gpsAltAD;
    int16_t  speedBD;
    uint16_t speedAD;
    uint16_t cogBD;
    uint16_t cogAD;
    uint16_t year;
    uint16_t dayMonth;
    uint16_t hourMinute;
    uint16_t second;

    // TAS sensor
    int16_t accX;
    int16_t accY;
    int16_t accZ;

    // TEMS sensor
    int16_t t1;
    int16_t t2;

    // RPMS sensor
    uint16_t rpm;

    void sendSeparator();
    void sendByte(uint8_t byte);
    void sendData(uint8_t dataId, uint16_t data, bool bigEndian = false);
    bool sendFgsData();
    bool sendFlvsData();
    bool sendFasData();
    bool sendFvasData();
    bool sendGpsData();
    bool sendDateTimeData();
    bool sendTasData();
    bool sendTemsData();
    bool sendRpmsData();
    void sendFrame1();
    void sendFrame2();
    void sendFrame3();
};

#endif // _FRSKY_TELEMETRY_H_
