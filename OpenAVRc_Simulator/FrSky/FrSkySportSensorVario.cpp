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


#include "FrSkySportSensorVario.h" 

FrSkySportSensorVario::FrSkySportSensorVario(SensorId id) : FrSkySportSensor(id) { }

void FrSkySportSensorVario::setData(float altitude, float vsi)
{
  altitudeData = (int32_t)(altitude * 100);
  vsiData = (int32_t)(vsi * 100);
}

void FrSkySportSensorVario::send(FrSkySportSingleWireSerial& serial, uint8_t id, uint32_t now)
{
  if(sensorId == id)
  {
    switch(sensorDataIdx)
    {
      case 0:
        if(now > altitudeTime)
        {
          altitudeTime = now + VARIO_ALT_DATA_PERIOD;
          serial.sendData(VARIO_ALT_DATA_ID, altitudeData);
        }
        else
        {
          serial.sendEmpty(VARIO_ALT_DATA_ID);
        }
        break;
      case 1:
        if(now > vsiTime)
        {
          vsiTime = now + VARIO_VSI_DATA_PERIOD;
          serial.sendData(VARIO_VSI_DATA_ID, vsiData);
        }
        else
        {
          serial.sendEmpty(VARIO_VSI_DATA_ID);
        }
        break;
    }
    sensorDataIdx++;
    if(sensorDataIdx >= VARIO_DATA_COUNT) sensorDataIdx = 0;
  }
}

uint16_t FrSkySportSensorVario::decodeData(uint8_t id, uint16_t appId, uint32_t data)
{
  if((sensorId == id) || (sensorId == FrSkySportSensor::ID_IGNORE))
  {
    switch(appId)
    {
      case VARIO_ALT_DATA_ID:
        altitude = ((int32_t)data) / 100.0;
        return appId;
      case VARIO_VSI_DATA_ID:
        vsi = ((int32_t)data) / 100.0;
        return appId;
    }
  }
  return SENSOR_NO_DATA_ID;
}

float FrSkySportSensorVario::getAltitude() { return altitude; }
float FrSkySportSensorVario::getVsi() { return vsi; }
