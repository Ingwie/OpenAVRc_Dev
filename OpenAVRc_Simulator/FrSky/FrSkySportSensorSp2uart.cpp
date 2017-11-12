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


#include "FrSkySportSensorSp2uart.h" 

FrSkySportSensorSp2uart::FrSkySportSensorSp2uart(SensorId id) : FrSkySportSensor(id) { }

void FrSkySportSensorSp2uart::setData(float adc3, float adc4)
{
  adc3Data = (uint32_t)(adc3 * 100);
  adc4Data = (uint32_t)(adc4 * 100);
}

void FrSkySportSensorSp2uart::send(FrSkySportSingleWireSerial& serial, uint8_t id, uint32_t now)
{
  if(sensorId == id)
  {
    switch(sensorDataIdx)
    {
      case 0:
        if(now > adc3Time)
        {
          adc3Time = now + SP2UARTB_ADC3_DATA_PERIOD;
          serial.sendData(SP2UARTB_ADC3_DATA_ID, adc3Data);
        }
        else
        {
          serial.sendEmpty(SP2UARTB_ADC3_DATA_ID);
        }
        break;
      case 1:
        if(now > adc4Time)
        {
          adc4Time = now + SP2UARTB_ADC4_DATA_PERIOD;
          serial.sendData(SP2UARTB_ADC4_DATA_ID, adc4Data);
        }
        else
        {
          serial.sendEmpty(SP2UARTB_ADC4_DATA_ID);
        }
        break;
    }
    sensorDataIdx++;
    if(sensorDataIdx >= SP2UARTB_DATA_COUNT) sensorDataIdx = 0;
  }
}

uint16_t FrSkySportSensorSp2uart::decodeData(uint8_t id, uint16_t appId, uint32_t data)
{
  if((sensorId == id) || (sensorId == FrSkySportSensor::ID_IGNORE))
  {
    switch(appId)
    {
      case SP2UARTB_ADC3_DATA_ID:
        adc3 = data / 100.0;
        return appId;
      case SP2UARTB_ADC4_DATA_ID:
        adc4 = data / 100.0;
        return appId;
    }
  }
  return SENSOR_NO_DATA_ID;
}

float FrSkySportSensorSp2uart::getAdc3() { return adc3; }
float FrSkySportSensorSp2uart::getAdc4() { return adc4; }
