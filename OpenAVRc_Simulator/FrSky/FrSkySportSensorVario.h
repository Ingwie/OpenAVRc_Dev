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


#ifndef _FRSKY_SPORT_SENSOR_VARIO_H_
#define _FRSKY_SPORT_SENSOR_VARIO_H_

#include "FrSkySportSensor.h"

#define VARIO_DEFAULT_ID ID1
#define VARIO_DATA_COUNT 2
#define VARIO_ALT_DATA_ID 0x0100
#define VARIO_VSI_DATA_ID 0x0110

#define VARIO_ALT_DATA_PERIOD 200
#define VARIO_VSI_DATA_PERIOD 100

class FrSkySportSensorVario : public FrSkySportSensor
{
  public:
    FrSkySportSensorVario(SensorId id = VARIO_DEFAULT_ID);
    void setData(float altitude, float vsi);
    virtual void send(FrSkySportSingleWireSerial& serial, uint8_t id, uint32_t now);
    virtual uint16_t decodeData(uint8_t id, uint16_t appId, uint32_t data);
    float getAltitude();
    float getVsi();

  private:
    int32_t altitudeData;
    int32_t vsiData;
    uint32_t altitudeTime;
    uint32_t vsiTime;
    float altitude;
    float vsi;
};

#endif // _FRSKY_SPORT_SENSOR_VARIO_H_
