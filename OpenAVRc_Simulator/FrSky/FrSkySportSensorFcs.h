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


#ifndef _FRSKY_SPORT_SENSOR_FCS_H_
#define _FRSKY_SPORT_SENSOR_FCS_H_

#include "FrSkySportSensor.h"

#define FCS_DEFAULT_ID ID3 // ID3 is the default for FCS-40A sensor, for FCS-150A use ID8.
#define FCS_DATA_COUNT 2
#define FCS_CURR_DATA_ID 0x0200
#define FCS_VOLT_DATA_ID 0x0210

#define FCS_CURR_DATA_PERIOD 500
#define FCS_VOLT_DATA_PERIOD 500

class FrSkySportSensorFcs : public FrSkySportSensor
{
  public:
    FrSkySportSensorFcs(SensorId id = FCS_DEFAULT_ID);
    void setData(float current, float voltage);
    virtual void send(FrSkySportSingleWireSerial& serial, uint8_t id, uint32_t now);
    virtual uint16_t decodeData(uint8_t id, uint16_t appId, uint32_t data);
    float getCurrent();
    float getVoltage();

  private:
    uint32_t currentData;
    uint32_t voltageData;
    uint32_t currentTime;
    uint32_t voltageTime;
    float current;
    float voltage;
};

#endif // _FRSKY_SPORT_SENSOR_FCS_H_
