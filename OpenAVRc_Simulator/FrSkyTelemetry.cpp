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

#include "FrSkyTelemetry.h"

FrSkyTelemetry::FrSkyTelemetry() : enabledSensors(SENSOR_NONE), cellIdx(0), frame1Time(0), frame2Time(0), frame3Time(0) {}

void FrSkyTelemetry::setFgsData(float fuel)
{
  enabledSensors |= SENSOR_FGS;
  FrSkyTelemetry::fuel = (uint16_t)fuel;
}

void FrSkyTelemetry::setFlvsData(float cell1, float cell2, float cell3, float cell4, float cell5, float cell6,
                                 float cell7, float cell8, float cell9, float cell10, float cell11, float cell12)
{
  enabledSensors |= SENSOR_FLVS;
  // DEVIATION FROM SPEC: in reality cells are numbered from 0 not from 1 like in the FrSky protocol spec
  FrSkyTelemetry::cell[0] = 0x0000 | (uint16_t)(cell1 * 500.0);
  if(cell2  != 0) FrSkyTelemetry::cell[1]  = 0x1000 | (uint16_t)(cell2  * 500.0);
  if(cell3  != 0) FrSkyTelemetry::cell[2]  = 0x2000 | (uint16_t)(cell3  * 500.0);
  if(cell4  != 0) FrSkyTelemetry::cell[3] = 0x3000 | (uint16_t)(cell4  * 500.0);
  if(cell5  != 0) FrSkyTelemetry::cell[4]  = 0x4000 | (uint16_t)(cell5  * 500.0);
  if(cell6  != 0) FrSkyTelemetry::cell[5]  = 0x5000 | (uint16_t)(cell6  * 500.0);
  if(cell7  != 0) FrSkyTelemetry::cell[6]  = 0x6000 | (uint16_t)(cell7  * 500.0);
  if(cell8  != 0) FrSkyTelemetry::cell[7]  = 0x7000 | (uint16_t)(cell8  * 500.0);
  if(cell9  != 0) FrSkyTelemetry::cell[8]  = 0x8000 | (uint16_t)(cell9  * 500.0);
  if(cell10 != 0) FrSkyTelemetry::cell[9]  = 0x9000 | (uint16_t)(cell10 * 500.0);
  if(cell11 != 0) FrSkyTelemetry::cell[10] = 0xA000 | (uint16_t)(cell11 * 500.0);
  if(cell12 != 0) FrSkyTelemetry::cell[11] = 0xB000 | (uint16_t)(cell12 * 500.0);
}

void FrSkyTelemetry::setFasData(float current, float voltage)
{
  enabledSensors |= SENSOR_FAS;
  // DEVIATION FROM SPEC: FrSky protocol spec suggests 0.5 ratio, but in reality this ratio is 0.5238 (based on the information from internet).
  voltage *= 0.5238;
  FrSkyTelemetry::voltageBD = (uint16_t)voltage;
  FrSkyTelemetry::voltageAD = (uint16_t)((voltage - voltageBD) * 10.0);
  FrSkyTelemetry::current = (uint16_t)(current * 10.0);
}

void FrSkyTelemetry::setFvasData(float alt, float vsi)
{
  enabledSensors |= SENSOR_FVAS;
  FrSkyTelemetry::altBD = (int16_t)alt;
  FrSkyTelemetry::altAD = abs((int16_t)((alt - FrSkyTelemetry::altBD) * 100.0));
  FrSkyTelemetry::vsi = (int16_t)(vsi * 100.0);
}

void FrSkyTelemetry::setGpsData(float lat, float lon, float alt, float speed, float cog, uint16_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t minute, uint8_t second)
{
  enabledSensors |= SENSOR_GPS;
  FrSkyTelemetry::latNS = (uint16_t)(lat < 0 ? 'S' : 'N');
  if(lat < 0) lat = -lat;
  FrSkyTelemetry::latBD = (uint16_t)lat;
  lat = (lat - (float)FrSkyTelemetry::latBD) * 60.0;
  FrSkyTelemetry::latBD = FrSkyTelemetry::latBD * 100 + (uint16_t)lat;
  FrSkyTelemetry::latAD = (uint16_t)((lat - (uint16_t)lat) * 10000.0);
  FrSkyTelemetry::lonEW = (uint16_t)(lon < 0 ? 'W' : 'E');
  if(lon < 0) lon = -lon;
  FrSkyTelemetry::lonBD = (uint16_t)lon;
  lon = (lon - (float)FrSkyTelemetry::lonBD) * 60.0;
  FrSkyTelemetry::lonBD = FrSkyTelemetry::lonBD * 100 + (uint16_t)lon;
  FrSkyTelemetry::lonAD = (uint16_t)((lon - (uint16_t)lon) * 10000.0);
  FrSkyTelemetry::gpsAltBD = (int16_t)alt;
  FrSkyTelemetry::gpsAltAD = abs((int16_t)((alt - FrSkyTelemetry::gpsAltBD) * 100.0));
  speed *= 1.94384; // Convert m/s to knots
  FrSkyTelemetry::speedBD = (uint16_t)speed;
  FrSkyTelemetry::speedAD = (uint16_t)((speed - FrSkyTelemetry::speedBD) * 100.0);
  FrSkyTelemetry::cogBD = (uint16_t)cog;
  FrSkyTelemetry::cogAD = (uint16_t)((cog - FrSkyTelemetry::cogBD) * 100.0);
  FrSkyTelemetry::year  = (uint16_t)(year);
  FrSkyTelemetry::dayMonth = (uint16_t)day;
  FrSkyTelemetry::dayMonth <<= 8;
  FrSkyTelemetry::dayMonth |= (uint16_t)month;
  FrSkyTelemetry::hourMinute = (uint16_t)hour;
  FrSkyTelemetry::hourMinute <<= 8;
  FrSkyTelemetry::hourMinute |= (uint16_t)minute;
  FrSkyTelemetry::second = (uint16_t)second;
}

void FrSkyTelemetry::setTasData(float accX, float accY, float accZ)
{
  enabledSensors |= SENSOR_TAS;
  FrSkyTelemetry::accX = (int16_t)(accX * 1000.0);
  FrSkyTelemetry::accY = (int16_t)(accY * 1000.0);
  FrSkyTelemetry::accZ = (int16_t)(accZ * 1000.0);
}

void FrSkyTelemetry::setTemsData(float t1, float t2)
{
  enabledSensors |= SENSOR_TEMS;
  FrSkyTelemetry::t1 = (int16_t)(t1);
  FrSkyTelemetry::t2 = (int16_t)(t2);
}

void FrSkyTelemetry::setRpmsData(float rpm)
{
  enabledSensors |= SENSOR_RPMS;
  FrSkyTelemetry::rpm = (uint16_t)(rpm / 30.0);
}

void FrSkyTelemetry::sendToSimu(uint8_t data)
{
  processSerialData(data);
}

void FrSkyTelemetry::sendSeparator()
{
  sendToSimu(0x5E);
}

void FrSkyTelemetry::sendByte(uint8_t byte)
{

  if(byte == 0x5E) { // use 5D 3E sequence instead of 5E to distinguish between separator character and real data
    sendToSimu(0x5D);
    sendToSimu(0x3E);
  } else if(byte == 0x5D) { // use 5D 3D sequence instead of 5D to distinguish between stuffing character and real data
    sendToSimu(0x5D);
    sendToSimu(0x3D);
  } else {
    sendToSimu(byte);
  }
  // port->flush();
}

void FrSkyTelemetry::sendData(uint8_t dataId, uint16_t data, bool bigEndian)
{
  sendSeparator();
  sendByte(dataId);
  uint8_t *bytes = (uint8_t*)&data;
  if(bigEndian == false) {
    sendByte(bytes[0]);
    sendByte(bytes[1]);
  } else {
    sendByte(bytes[1]);
    sendByte(bytes[0]);
  }
  //sendByte->flush();
}

bool FrSkyTelemetry::sendFasData()
{
  bool enabled = enabledSensors & SENSOR_FAS;
  if(enabled == true) {
    sendData(0x28, current);
    sendData(0x3A, voltageBD);
    sendData(0x3B, voltageAD);
  }
  return enabled;
}

bool FrSkyTelemetry::sendFgsData()
{
  bool enabled = enabledSensors & SENSOR_FGS;
  if(enabled == true) {
    sendData(0x04, fuel);
  }
  return enabled;
}

bool FrSkyTelemetry::sendFlvsData()
{
  bool enabled = enabledSensors & SENSOR_FLVS;
  if(enabled == true) {
    // Only send one cell at a time
    if((cell[cellIdx] == 0) || (cellIdx == 12)) cellIdx = 0;
    sendData(0x06, cell[cellIdx], true);
    cellIdx++;
  }
  return enabled;
}

bool FrSkyTelemetry::sendFvasData()
{
  bool enabled = enabledSensors & SENSOR_FVAS;
  if(enabled == true) {
    sendData(0x10, altBD);
    sendData(0x21, altAD);
    sendData(0x30, vsi); // Not documented in FrSky spec, added based on OpenTX sources.
  }
  return enabled;
}

bool FrSkyTelemetry::sendGpsData()
{
  bool enabled = enabledSensors & SENSOR_GPS;
  if(enabled == true) {
    sendData(0x01, gpsAltBD);
    sendData(0x09, gpsAltAD);
    sendData(0x11, speedBD);
    sendData(0x19, speedAD);
    sendData(0x12, lonBD); // DEVIATION FROM SPEC: FrSky protocol spec says lat shall be sent as big endian, but it reality little endian is expected
    sendData(0x1A, lonAD); // DEVIATION FROM SPEC: FrSky protocol spec says lat shall be sent as big endian, but it reality little endian is expected
    sendData(0x22, lonEW); // DEVIATION FROM SPEC: FrSky protocol spec says lon shall be sent as big endian, but it reality little endian is expected
    sendData(0x13, latBD); // DEVIATION FROM SPEC: FrSky protocol spec says lon shall be sent as big endian, but it reality little endian is expected
    sendData(0x1B, latAD);
    sendData(0x23, latNS);
    sendData(0x14, cogBD);
    sendData(0x1C, cogAD);
  }
  return enabled;
}

bool FrSkyTelemetry::sendDateTimeData()
{
  bool enabled = enabledSensors & SENSOR_GPS;
  if(enabled == true) {
    sendData(0x15, dayMonth, true);
    sendData(0x16, year);
    sendData(0x17, hourMinute, true);
    sendData(0x18, second);
  }
  return enabled;
}

bool FrSkyTelemetry::sendTasData()
{
  bool enabled = enabledSensors & SENSOR_TAS;
  if(enabled == true) {
    sendData(0x24, accX);
    sendData(0x25, accY);
    sendData(0x26, accZ);
  }
  return enabled;
}

bool FrSkyTelemetry::sendTemsData()
{
  bool enabled = enabledSensors & SENSOR_TEMS;
  if(enabled == true) {
    sendData(0x02, t1);
    sendData(0x05, t2);
  }
  return enabled;
}

bool FrSkyTelemetry::sendRpmsData()
{
  bool enabled = enabledSensors & SENSOR_RPMS;
  if(enabled == true) {
    sendData(0x03, rpm);
  }
  return enabled;
}

void FrSkyTelemetry::sendFrame1()
{
  bool result = false;
  result  = sendFasData();
  result |= sendFlvsData();
  result |= sendFvasData();
  result |= sendTasData();
  result |= sendTemsData();
  result |= sendRpmsData();
  if (result == true) sendSeparator();
}

void FrSkyTelemetry::sendFrame2()
{
  bool result = false;
  result  = sendFgsData();
  result |= sendGpsData();
  if (result == true) sendSeparator();
}

void FrSkyTelemetry::sendFrame3()
{
  bool result = false;
  result = sendDateTimeData();
  if (result == true) sendSeparator();
}

void FrSkyTelemetry::send()
{
  //Add 5mS to time
  currentTime += 5;

  if(currentTime > frame3Time) { // Sent every 5s (5000ms)
    frame3Time = currentTime + 5000;
    frame2Time = currentTime + 200; // Postpone frame 2 to next cycle
    frame1Time = currentTime + 200; // Postpone frame 1 to next cycle
    sendFrame3();
  } else if(currentTime > frame2Time) { // Sent every 1s (1000ms)
    frame2Time = currentTime + 2000;
    frame1Time = currentTime + 200; // Postpone frame 1 to next cycle
    sendFrame2();
  } else if(currentTime > frame1Time) { // Sent every 200ms
    frame1Time = currentTime + 200;
    sendFrame1();
  }
}

FrSkyTelemetry telemetry; // Create telemetry object

void FrSkyTelemetry_loop()
{
  // Set current/voltage sensor (FAS) data
  // (set Voltage source to FAS in menu to use this data for battery voltage,
  //  set Current source to FAS in menu to use this data for current readins)
  telemetry.setFasData(25.3,   // Current consumption in amps
                       12.6);  // Battery voltage in volts

  // Set fuel sensor (FGS) data
  telemetry.setFgsData(55.5);  // Fuel level in percent

  // Set LiPo voltage sensor (FLVS) data (we use two sensors to simulate 8S battery
  // (set Voltage source to Cells in menu to use this data for battery voltage)
  telemetry.setFlvsData(4.07, 4.08, 4.09, 4.10, 4.11, 4.12, 4.13, 4.14);  // Cell voltages in volts (cells 1-8). Cells 9-12 are not used in this example

  // Set variometer sensor (FVAS) data
  telemetry.setFvasData(245.5,   // Altitude in m (can be nevative)
                        -3.5);  // Vertical speed in m/s (can be nevative, 0.0m/s will be set when skipped)

  // Set GPS data
  telemetry.setGpsData(0,0,//48.858289, 2.294502,   // Latitude and longitude in degrees decimal (positive for N/E, negative for S/W)
                       245.5,                 // Altitude in m (can be nevative)
                       100.0,                 // Speed in m/s
                       90.23,                 // Course over ground in degrees
                       14, 9, 14,             // Date (year - 2000, month, day)
                       12, 00, 00);           // Time (hour, minute, second) - will be affected by timezone setings in your radio

  // Set triaxial acceleration sensor (TAS) data
  telemetry.setTasData(17.95,    // x-axis acceleration in g (can be negative)
                       0.0,      // y-axis acceleration in g (can be negative)
                       -17.95);  // z-axis acceleration in g (can be negative)

  // Set temperature sensor (TEMS) data
  telemetry.setTemsData(25.6,   // Temperature #1 in degrees Celsuis (can be negative)
                        -7.8);  // Temperature #2 in degrees Celsuis (can be negative)

  // Set RPM sensor (RPMS) data
  // (set number of blades to 2 in telemetry menu to get correct rpm value)
  telemetry.setRpmsData(200);  // Rotations per minute

  // Send the telemetry data, note that the data will only be sent for sensors
  // that had their data set at least once. Also it will only be set in defined
  // time intervals, so not necessarily at every call to send() method.
  telemetry.send();
}
