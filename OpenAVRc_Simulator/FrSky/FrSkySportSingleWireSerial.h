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


#ifndef _FRSKY_SPORT_SINGLE_WIRE_SERIAL_
#define _FRSKY_SPORT_SINGLE_WIRE_SERIAL_

#include "../../OpenAVRc/OpenAVRc.h"

extern void SendSerialDataToUART(uint8_t data);

#define FRSKY_TELEMETRY_START_FRAME 0x7E
#define FRSKY_SENSOR_FRSKY_DATA_FRAME 0x10
#define FRSKY_STUFFING 0x7D

uint8_t FrSkySportSingleWireSerialmemdata = 0xFF;

class FrSkySportSingleWireSerial
{
  public:
    FrSkySportSingleWireSerial();
    void sendHeader(uint8_t id);
    void sendData(uint16_t dataTypeId, uint32_t id);
    void sendEmpty(uint16_t dataTypeId);
    uint8_t read();

  private:
    enum SerialMode { RX = 0, TX = 1 };
    void setMode(SerialMode mode);
    void sendByte(uint8_t byte);
    void sendCrc();
    volatile uint8_t *uartC3;
    uint16_t crc;
};

#endif // _FRSKY_SPORT_SINGLE_WIRE_SERIAL_
