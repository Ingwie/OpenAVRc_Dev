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
  FrSky Telemetry for Teensy 3.x/LC and 328P/168 based boards (e.g. Pro Mini, Nano, Uno)
  (c) Pawelsky 20170831
  Not for commercial use
*/

#include "FrSkyDTelemetry.h"

FrSkyDTelemetry::FrSkyDTelemetry() : enabledSensors(SENSOR_NONE), cellIdx(0), frame1Time(0), frame2Time(0), frame3Time(0), frameRSSITime(0) {}

void FrSkyDTelemetry::setFgsData(float fuel)
{
  enabledSensors |= SENSOR_FGS;
  FrSkyDTelemetry::fuel = (uint16_t)round(fuel);
}

void FrSkyDTelemetry::setFlvsData(float cell1, float cell2, float cell3, float cell4, float cell5, float cell6)
                                  //,float cell7, float cell8, float cell9, float cell10, float cell11, float cell12)
{
  enabledSensors |= SENSOR_FLVS;
  // DEVIATION FROM SPEC: in reality cells are numbered from 0 not from 1 like in the FrSky protocol spec
  FrSkyDTelemetry::cell[0] = 0x0000 | (uint16_t)round(cell1 * 500.0);
  if(cell2  != 0)
    FrSkyDTelemetry::cell[1]  = 0x1000 | (uint16_t)round(cell2  * 500.0);
  if(cell3  != 0)
    FrSkyDTelemetry::cell[2]  = 0x2000 | (uint16_t)round(cell3  * 500.0);
  if(cell4  != 0)
    FrSkyDTelemetry::cell[3] = 0x3000 | (uint16_t)round(cell4  * 500.0);
  if(cell5  != 0)
    FrSkyDTelemetry::cell[4]  = 0x4000 | (uint16_t)round(cell5  * 500.0);
  if(cell6  != 0)
    FrSkyDTelemetry::cell[5]  = 0x5000 | (uint16_t)round(cell6  * 500.0);
  /*if(cell7  != 0)
    FrSkyDTelemetry::cell[6]  = 0x6000 | (uint16_t)round(cell7  * 500.0);
  if(cell8  != 0)
    FrSkyDTelemetry::cell[7]  = 0x7000 | (uint16_t)round(cell8  * 500.0);
  if(cell9  != 0)
    FrSkyDTelemetry::cell[8]  = 0x8000 | (uint16_t)round(cell9  * 500.0);
  if(cell10 != 0)
    FrSkyDTelemetry::cell[9]  = 0x9000 | (uint16_t)round(cell10 * 500.0);
  if(cell11 != 0)
    FrSkyDTelemetry::cell[10] = 0xA000 | (uint16_t)round(cell11 * 500.0);
  if(cell12 != 0)
    FrSkyDTelemetry::cell[11] = 0xB000 | (uint16_t)round(cell12 * 500.0);*/
}

void FrSkyDTelemetry::setFasData(float current, float voltage)
{
  enabledSensors |= SENSOR_FAS;
  // DEVIATION FROM SPEC: FrSky protocol spec suggests 0.5 ratio, but in reality this ratio is 0.5238 (based on the information from internet).
  voltage *= 0.5238;
  FrSkyDTelemetry::voltageBD = (uint16_t)voltage;
  FrSkyDTelemetry::voltageAD = (uint16_t)round((voltage - voltageBD) * 10.0);
  FrSkyDTelemetry::current = (uint16_t)round(current * 10.0);
}

void FrSkyDTelemetry::setFvasData(float alt, float vsi)
{
  enabledSensors |= SENSOR_FVAS;
  FrSkyDTelemetry::altBD = (int16_t)alt;
  FrSkyDTelemetry::altAD = abs((int16_t)round((alt - FrSkyDTelemetry::altBD) * 100.0));
  FrSkyDTelemetry::vsi = (int16_t)(vsi * 100.0);
}

void FrSkyDTelemetry::setGpsData(float lat, float lon, float alt, float speed, float cog, uint16_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t minute, uint8_t second)
{
  enabledSensors |= SENSOR_GPS;
  FrSkyDTelemetry::latNS = (uint16_t)(lat < 0 ? 'S' : 'N');
  if(lat < 0)
    lat = -lat;
  FrSkyDTelemetry::latBD = (uint16_t)lat;
  lat = (lat - (float)FrSkyDTelemetry::latBD) * 60.0;
  FrSkyDTelemetry::latBD = FrSkyDTelemetry::latBD * 100 + (uint16_t)lat;
  FrSkyDTelemetry::latAD = (uint16_t)round((lat - (uint16_t)lat) * 10000.0);
  FrSkyDTelemetry::lonEW = (uint16_t)(lon < 0 ? 'W' : 'E');
  if(lon < 0)
    lon = -lon;
  FrSkyDTelemetry::lonBD = (uint16_t)lon;
  lon = (lon - (float)FrSkyDTelemetry::lonBD) * 60.0;
  FrSkyDTelemetry::lonBD = FrSkyDTelemetry::lonBD * 100 + (uint16_t)lon;
  FrSkyDTelemetry::lonAD = (uint16_t)round((lon - (uint16_t)lon) * 10000.0);
  FrSkyDTelemetry::gpsAltBD = (int16_t)alt;
  FrSkyDTelemetry::gpsAltAD = abs((int16_t)round((alt - FrSkyDTelemetry::gpsAltBD) * 100.0));
  speed *= 1.94384; // Convert m/s to knots
  FrSkyDTelemetry::speedBD = (uint16_t)speed;
  FrSkyDTelemetry::speedAD = (uint16_t)round((speed - FrSkyDTelemetry::speedBD) * 100.0);
  FrSkyDTelemetry::cogBD = (uint16_t)cog;
  FrSkyDTelemetry::cogAD = (uint16_t)round((cog - FrSkyDTelemetry::cogBD) * 100.0);
  FrSkyDTelemetry::year  = (uint16_t)(year);
  FrSkyDTelemetry::dayMonth = (uint16_t)day;
  FrSkyDTelemetry::dayMonth <<= 8;
  FrSkyDTelemetry::dayMonth |= (uint16_t)month;
  FrSkyDTelemetry::hourMinute = (uint16_t)hour;
  FrSkyDTelemetry::hourMinute <<= 8;
  FrSkyDTelemetry::hourMinute |= (uint16_t)minute;
  FrSkyDTelemetry::second = (uint16_t)second;
}

void FrSkyDTelemetry::setTasData(float accX, float accY, float accZ)
{
  enabledSensors |= SENSOR_TAS;
  FrSkyDTelemetry::accX = (int16_t)round(accX * 1000.0);
  FrSkyDTelemetry::accY = (int16_t)round(accY * 1000.0);
  FrSkyDTelemetry::accZ = (int16_t)round(accZ * 1000.0);
}

void FrSkyDTelemetry::setTemsData(float t1, float t2)
{
  enabledSensors |= SENSOR_TEMS;
  FrSkyDTelemetry::t1 = (int16_t)round(t1);
  FrSkyDTelemetry::t2 = (int16_t)round(t2);
}

void FrSkyDTelemetry::setRpmsData(float rpm)
{
  enabledSensors |= SENSOR_RPMS;
  FrSkyDTelemetry::rpm = (uint16_t)round(rpm / 30.0);
}

void FrSkyDTelemetry::sendSeparator()
{
  processSerialData(0x5E);
}

void FrSkyDTelemetry::sendByte(uint8_t byte)
{
  if(byte == 0x5E) { // use 5D 3E sequence instead of 5E to distinguish between separator character and real data
    processSerialData(0x5D);
    processSerialData(0x3E);
  } else if(byte == 0x5D) { // use 5D 3D sequence instead of 5D to distinguish between stuffing character and real data
    processSerialData(0x5D);
    processSerialData(0x3D);
  } else if(byte == 0x7E) {
    processSerialData(0x7D);
    processSerialData(0x5E);
  } else if(byte == 0x7D) {
    processSerialData(0x7D);
    processSerialData(0x5D);
  } else {
    processSerialData(byte);
  }
}

void FrSkyDTelemetry::sendData(uint8_t dataId, uint16_t data, bool bigEndian)
{
  processSerialData(0x7E);
  processSerialData(0xFD);
  processSerialData(0x06);
  processSerialData(0x00);
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
  sendSeparator();
  processSerialData(0x00);
  processSerialData(0x00);
  processSerialData(0x00);
  processSerialData(0x7E);
}

bool FrSkyDTelemetry::sendFasData()
{
  bool enabled = enabledSensors & SENSOR_FAS;
  if(enabled == true) {
    sendData(0x28, current);
    sendData(0x3A, voltageBD);
    sendData(0x3B, voltageAD);
  }
  return enabled;
}

bool FrSkyDTelemetry::sendFgsData()
{
  bool enabled = enabledSensors & SENSOR_FGS;
  if(enabled == true) {
    sendData(0x04, fuel);
  }
  return enabled;
}

bool FrSkyDTelemetry::sendFlvsData()
{
  bool enabled = enabledSensors & SENSOR_FLVS;
  if(enabled == true) {
    // Only send one cell at a time
    if((cell[cellIdx] == 0) || (cellIdx == 5))
      cellIdx = 0;
    sendData(0x06, cell[cellIdx], true);
    cellIdx++;
  }
  return enabled;
}

bool FrSkyDTelemetry::sendFvasData()
{
  bool enabled = enabledSensors & SENSOR_FVAS;
  if(enabled == true) {
    sendData(0x10, altBD);
    sendData(0x21, altAD);
    sendData(0x30, vsi); // Not documented in FrSky spec, added based on OpenTX sources.
  }
  return enabled;
}

bool FrSkyDTelemetry::sendGpsData()
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

bool FrSkyDTelemetry::sendDateTimeData()
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

bool FrSkyDTelemetry::sendTasData()
{
  bool enabled = enabledSensors & SENSOR_TAS;
  if(enabled == true) {
    sendData(0x24, accX);
    sendData(0x25, accY);
    sendData(0x26, accZ);
  }
  return enabled;
}

bool FrSkyDTelemetry::sendTemsData()
{
  bool enabled = enabledSensors & SENSOR_TEMS;
  if(enabled == true) {
    sendData(0x02, t1);
    sendData(0x05, t2);
  }
  return enabled;
}

bool FrSkyDTelemetry::sendRpmsData()
{
  bool enabled = enabledSensors & SENSOR_RPMS;
  if(enabled == true) {
    sendData(0x03, rpm);
  }
  return enabled;
}

void FrSkyDTelemetry::sendFrame1()
{
  bool result = false;
  result  = sendFasData();
  result |= sendFlvsData();
  result |= sendFvasData();
  result |= sendTasData();
  result |= sendTemsData();
  result |= sendRpmsData();
  //if (result == true) sendSeparator();
}

void FrSkyDTelemetry::sendFrame2()
{
  bool result = false;
  result  = sendFgsData();
  result |= sendGpsData();
  //if (result == true) sendSeparator();
}

void FrSkyDTelemetry::sendFrame3()
{
  bool result = false;
  result = sendDateTimeData();
  //if (result == true) sendSeparator();
}

void FrSkyDTelemetry::send()
{
  uint32_t currentTime = GetTickCount();

  if(currentTime > frame3Time) { // Sent every 5s (5000ms)
    frame3Time = currentTime + 5000;
    frame2Time = currentTime + 200; // Postpone frame 2 to next cycle
    frame1Time = currentTime + 200; // Postpone frame 1 to next cycle
    frameRSSITime = currentTime + 100;
    sendFrame3();
  } else if(currentTime > frame2Time) { // Sent every 1s (1000ms)
    frame2Time = currentTime + 2000;
    frame1Time = currentTime + 200; // Postpone frame 1 to next cycle
    frameRSSITime = currentTime + 100;
    sendFrame2();
  } else if(currentTime > frame1Time) { // Sent every 200ms
    frame1Time = currentTime + 200;
    frameRSSITime = currentTime + 100;
    sendFrame1();
  }

  else if(currentTime > frameRSSITime) { // Sent every 100ms
    frameRSSITime = currentTime + 100;

    processSerialData(0x7E);
    processSerialData(0xFE);
    processSerialData(0x40);//A1
    processSerialData(0x40);//A2
    processSerialData(0x40);//RX
    processSerialData(0x82);//TX*2
    processSerialData(0x00);
    processSerialData(0x00);
    processSerialData(0x00);
    processSerialData(0x00);
    processSerialData(0x7E);
  }
}
