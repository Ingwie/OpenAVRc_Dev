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

void FrSkyDTelemetry::setFlvsData(float cell1, float cell2, float cell3, float cell4, float cell5, float cell6
 ,float cell7, float cell8, float cell9, float cell10, float cell11, float cell12)
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
  if(cell7  != 0)
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
    FrSkyDTelemetry::cell[11] = 0xB000 | (uint16_t)round(cell12 * 500.0);
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
  SendSerialDataToUART(0x5E);
}

void FrSkyDTelemetry::sendByte(uint8_t byte)
{
  if(byte == 0x5E) { // use 5D 3E sequence instead of 5E to distinguish between separator character and real data
    SendSerialDataToUART(0x5D);
    SendSerialDataToUART(0x3E);
  } else if(byte == 0x5D) { // use 5D 3D sequence instead of 5D to distinguish between stuffing character and real data
    SendSerialDataToUART(0x5D);
    SendSerialDataToUART(0x3D);
  } else if(byte == 0x7E) {
    SendSerialDataToUART(0x7D);
    SendSerialDataToUART(0x5E);
  } else if(byte == 0x7D) {
    SendSerialDataToUART(0x7D);
    SendSerialDataToUART(0x5D);
  } else {
    SendSerialDataToUART(byte);
  }
}

void FrSkyDTelemetry::sendData(uint8_t dataId, uint16_t data, bool bigEndian)
{
  uint8_t numdata = 0;
  uint8_t *bytes = (uint8_t*)&data;

  for (int i=0; i<2; ++i) {
    if ((bytes[i] == 0X5E)||(bytes[i] == 0X5D)||(bytes[i] == 0X7E)||(bytes[i] == 0X7D)) {
      ++numdata;
    }
  }

  SendSerialDataToUART(0x7E);
  SendSerialDataToUART(0xFD);
  SendSerialDataToUART(0x05 + numdata);
  SendSerialDataToUART(0x00);
  sendSeparator();

  sendByte(dataId);

  if(bigEndian == false) {
    sendByte(bytes[0]);
    sendByte(bytes[1]);
  } else {
    sendByte(bytes[1]);
    sendByte(bytes[0]);
  }
  sendSeparator();
  if (numdata<2) SendSerialDataToUART(0x00);
  if (numdata<1) SendSerialDataToUART(0x00);
  SendSerialDataToUART(0x7E);
}

bool FrSkyDTelemetry::sendFasData()
{
  bool enabled = enabledSensors & SENSOR_FAS;
  static uint8_t dispatch = 0;
  if(enabled == true)
    {
      switch (dispatch)
        {
        case 0:
          sendData(0x28, current); break;
        case 1:
          sendData(0x3A, voltageBD); break;
        case 2:
          sendData(0x3B, voltageAD); break;
        }
      if (++dispatch > 2)
        dispatch = 0;
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
    if((cell[cellIdx] == 0) || (cellIdx == 12))
      cellIdx = 0;
    sendData(0x06, cell[cellIdx], true);
    cellIdx++;
  }
  return enabled;
}

bool FrSkyDTelemetry::sendFvasData()
{
  static uint8_t dispatch = 0;
  bool enabled = enabledSensors & SENSOR_FVAS;
  if(enabled == true)
    {
      switch (dispatch)
        {
        case 0:
          sendData(0x10, altBD);
          break;
        case 1:
          sendData(0x21, altAD);
          break;
        case 2:
          sendData(0x30, vsi);
           // Not documented in FrSky spec, added based on OpenTX sources.
        }
      if (++dispatch > 2)
        dispatch = 0;
    }
  return enabled;
}

bool FrSkyDTelemetry::sendGpsData()
{
  static uint8_t dispatch = 0;
  bool enabled = enabledSensors & SENSOR_GPS;
  if(enabled == true)
    {
      switch (dispatch)
        {
        case 0:
          sendData(0x01, gpsAltBD);
          sendData(0x09, gpsAltAD);
          break;
        case 1:
          sendData(0x11, speedBD);
          break;
        case 2:
          sendData(0x19, speedAD);
          break;
        case 3:
          sendData(0x12, lonBD);
          break; // DEVIATION FROM SPEC: FrSky protocol spec says lat shall be sent as big endian, but it reality little endian is expected
        case 4:
          sendData(0x1A, lonAD);
          break; // DEVIATION FROM SPEC: FrSky protocol spec says lat shall be sent as big endian, but it reality little endian is expected
        case 5:
          sendData(0x22, lonEW);
          break; // DEVIATION FROM SPEC: FrSky protocol spec says lon shall be sent as big endian, but it reality little endian is expected
        case 6:
          sendData(0x13, latBD);
          break; // DEVIATION FROM SPEC: FrSky protocol spec says lon shall be sent as big endian, but it reality little endian is expected
        case 7:
          sendData(0x1B, latAD);
          break;
        case 8:
          sendData(0x23, latNS);
          break;
        case 9:
          sendData(0x14, cogBD);
          break;
        case 10:
          sendData(0x1C, cogAD);
          break;
        }
      if (++dispatch > 10)
        dispatch = 0;
    }
  return enabled;
}

bool FrSkyDTelemetry::sendDateTimeData()
{
  static uint8_t dispatch = 0;
  bool enabled = enabledSensors & SENSOR_GPS;
  if(enabled == true)
    {
      switch (dispatch)
        {
        case 0:
          sendData(0x15, dayMonth, true);
          break;
        case 1:
          sendData(0x16, year);
          break;
        case 2:
          sendData(0x17, hourMinute, true);
          break;
        case 3:
          sendData(0x18, second);
          break;
        }
      if (++dispatch > 3)
        dispatch = 0;
    }
  return enabled;
}

bool FrSkyDTelemetry::sendTasData()
{
  static uint8_t dispatch = 0;
  bool enabled = enabledSensors & SENSOR_TAS;
  if(enabled == true)
    {
      switch (dispatch)
        {
        case 0:
          sendData(0x24, accX);
          break;
        case 1:
          sendData(0x25, accY);
          break;
        case 2:
          sendData(0x26, accZ);
          break;
        }
      if (++dispatch > 2)
        dispatch = 0;
    }
  return enabled;
}

bool FrSkyDTelemetry::sendTemsData()
{
  static uint8_t dispatch = 0;
  bool enabled = enabledSensors & SENSOR_TEMS;
  if(enabled == true)
    {
      switch (dispatch)
        {
        case 0:
          sendData(0x02, t1);
          break;
        case 1:
          sendData(0x05, t2);
          break;
        }
      if (++dispatch > 1)
        dispatch = 0;
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
  static uint8_t dispatch = 0;

  switch (dispatch)
    {
    case 0:
      sendFasData();
      break; // 3 occurences
    case 1:
      sendFlvsData();
      break;
    case 2:
      sendFvasData();
      break;
    case 3:
      sendTasData();
      break;
    case 4:
      sendTemsData();
      break;
    case 5:
      sendRpmsData();
      break;
    }
  if (++dispatch > 5)
    dispatch = 0;
}

void FrSkyDTelemetry::sendFrame2()
{
  static uint8_t dispatch = 0;

  switch (dispatch)
    {
    case 0:
      sendFgsData();
      break;
    case 1:
      sendGpsData();
      break;
    }
  if (++dispatch > 1)
    dispatch = 0;
}

void FrSkyDTelemetry::sendFrame3()
{
  bool result = false;
  result = sendDateTimeData();
  if (result == true) sendSeparator();
}

void FrSkyDTelemetry::send()
{
  static uint8_t dispatch = 0;

  switch (dispatch)
    {
    case 1:
      sendFrame3();
      break;
    case 2:
    case 4:
    case 6:
      sendFrame2();
      break;
    case 3:
    case 5:
    case 7:
    case 8:
      sendFrame1();
      break;
    }
  if (++dispatch > 8)
    dispatch = 0;
  /* Send analog & rssi */
  SendSerialDataToUART(0x7E);
  SendSerialDataToUART(0xFE);
  SendSerialDataToUART((uint8_t)(Tele_Analog1));//A1
  SendSerialDataToUART((uint8_t)(Tele_Analog2));//A2
  SendSerialDataToUART((uint8_t)Tele_RSSIRx);//RX
  SendSerialDataToUART((uint8_t)Tele_RSSITx*2);//TX*2
  SendSerialDataToUART(0x00);
  SendSerialDataToUART(0x00);
  SendSerialDataToUART(0x00);
  SendSerialDataToUART(0x00);
  SendSerialDataToUART(0x7E);
}
