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


#include "FrSkySportPolling.h"
#include "FrSkySimu.h"

FrSkySportPolling::FrSkySportPolling()
{
  nextPollIdIdx = 0;
  nextPollTime = 0;
  rssiPollTime = 0;
}

FrSkySportSensor::SensorId FrSkySportPolling::getNextId()
{
  if(nextPollIdIdx >= FRSKY_POLLIED_ID_COUNT) nextPollIdIdx = 0;
  return POLLED_ID_TABLE[nextPollIdIdx++];
}

FrSkySportSensor::SensorId FrSkySportPolling::pollData(FrSkySportSingleWireSerial& serial, uint32_t now)
{
  FrSkySportSensor::SensorId id = FrSkySportSensor::ID_IGNORE;

    // Send RSSI every 444ms independent of other IDs
    if(now >= rssiPollTime)
    {
      serial.sendHeader(FrSkySportSensor::ID25);
      serial.sendData(POLLING_RSSI_DATA_ID, Tele_RSSIRx);
      serial.sendHeader(FrSkySportSensor::ID25);
      serial.sendData(0xF105, Tele_RSSITx);
      rssiPollTime = now + POLLING_RSSI_POLL_TIME;
      nextPollTime = now + POLLING_ID_POLL_TIME;
    }
    // Poll next ID every 12ms
    else if(now >= nextPollTime)
    {
      id = getNextId();
      serial.sendHeader(id);
      nextPollTime = now + POLLING_ID_POLL_TIME;
    }

  return id;
}
