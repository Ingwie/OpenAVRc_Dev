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

#include <cmath>
#include "FrSkySportSensorRpm.h"

FrSkySportSensorRpm::FrSkySportSensorRpm(SensorId id) : FrSkySportSensor(id) { }

void FrSkySportSensorRpm::setData(uint32_t rpm, float t1, float t2)
{
  rpmData = rpm * 2;
  t1Data = (int32_t)round(t1);
  t2Data = (int32_t)round(t2);
}

void FrSkySportSensorRpm::send(FrSkySportSingleWireSerial& serial, uint8_t id, uint32_t now)
{
  if(sensorId == id)
  {
    switch(sensorDataIdx)
    {
      case 0:
        if(now > t1Time)
        {
          t1Time = now + RPM_T1_DATA_PERIOD;
          serial.sendData(RPM_T1_DATA_ID, t1Data);
        }
        else
        {
          serial.sendEmpty(RPM_T1_DATA_ID);
        }
        break;
      case 1:
        if(now > t2Time)
        {
          t2Time = now + RPM_T2_DATA_PERIOD;
          serial.sendData(RPM_T2_DATA_ID, t2Data);
        }
        else
        {
          serial.sendEmpty(RPM_T2_DATA_ID);
        }
        break;
      case 2:
        if(now > rpmTime)
        {
          rpmTime = now + RPM_ROT_DATA_PERIOD;
          serial.sendData(RPM_ROT_DATA_ID, rpmData);
        }
        else
        {
          serial.sendEmpty(RPM_ROT_DATA_ID);
        }
        break;
    }
    sensorDataIdx++;
    if(sensorDataIdx >= RPM_DATA_COUNT) sensorDataIdx = 0;
  }
}

uint16_t FrSkySportSensorRpm::decodeData(uint8_t id, uint16_t appId, uint32_t data)
{
  if((sensorId == id) || (sensorId == FrSkySportSensor::ID_IGNORE))
  {
    switch(appId)
    {
      case RPM_T1_DATA_ID:
        t1 = (int32_t)data;
        return appId;
      case RPM_T2_DATA_ID:
        t2 = (int32_t)data;
        return appId;
      case RPM_ROT_DATA_ID:
        rpm = (uint32_t)(data / 2);
        return appId;
    }
  }
  return SENSOR_NO_DATA_ID;
}

uint32_t FrSkySportSensorRpm::getRpm() { return rpm; }
int32_t FrSkySportSensorRpm::getT1() { return t1; }
int32_t FrSkySportSensorRpm::getT2() { return t2; }
