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


#ifndef _FRSKY_SPORT_TELEMETRY_H_
#define _FRSKY_SPORT_TELEMETRY_H_

#include "Nutbolts.h"
#include "FrSkySportPolling.h"
#include "FrSkySportSensor.h"
#include "FrSkySportSingleWireSerial.h"

#define FRSKY_TELEMETRY_MAX_SENSORS 28

class FrSkySportTelemetry
{
  public:
    FrSkySportTelemetry(bool polling = false);
    void begin(
                FrSkySportSensor* sensor1,         FrSkySportSensor* sensor2 =  NULL, FrSkySportSensor* sensor3 =  NULL,
                FrSkySportSensor* sensor4  = NULL, FrSkySportSensor* sensor5 =  NULL, FrSkySportSensor* sensor6 =  NULL,
                FrSkySportSensor* sensor7  = NULL, FrSkySportSensor* sensor8 =  NULL, FrSkySportSensor* sensor9 =  NULL,
                FrSkySportSensor* sensor10 = NULL, FrSkySportSensor* sensor11 = NULL, FrSkySportSensor* sensor12 = NULL,
                FrSkySportSensor* sensor13 = NULL, FrSkySportSensor* sensor14 = NULL, FrSkySportSensor* sensor15 = NULL,
                FrSkySportSensor* sensor16 = NULL, FrSkySportSensor* sensor17 = NULL, FrSkySportSensor* sensor18 = NULL,
                FrSkySportSensor* sensor19 = NULL, FrSkySportSensor* sensor20 = NULL, FrSkySportSensor* sensor21 = NULL,
                FrSkySportSensor* sensor22 = NULL, FrSkySportSensor* sensor23 = NULL, FrSkySportSensor* sensor24 = NULL,
                FrSkySportSensor* sensor25 = NULL, FrSkySportSensor* sensor26 = NULL, FrSkySportSensor* sensor27 = NULL,
                FrSkySportSensor* sensor28 = NULL);
    void send();

  private:
    FrSkySportSensor* sensors[FRSKY_TELEMETRY_MAX_SENSORS];
    FrSkySportSingleWireSerial serial;
    uint8_t  sensorCount;
    uint8_t  prevData;
    FrSkySportPolling *pollingClass;
};

#endif // _FRSKY_SPORT_TELEMETRY_H_
