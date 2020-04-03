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


#include "FrSkySportSingleWireSerial.h"

FrSkySportSingleWireSerial::FrSkySportSingleWireSerial()
{

}

void FrSkySportSingleWireSerial::setMode(SerialMode mode)
{

}

void FrSkySportSingleWireSerial::sendHeader(uint8_t id)
{
    setMode(TX);
    SendSerialDataToUART(FRSKY_TELEMETRY_START_FRAME);
    SendSerialDataToUART(id);
    FrSkySportSingleWireSerialmemdata = id;
    setMode(RX);
}


void FrSkySportSingleWireSerial::sendByte(uint8_t byte)
{
    if(byte == 0x7E)
    {
      SendSerialDataToUART(FRSKY_STUFFING);
      SendSerialDataToUART(0x5E); // 0x7E xor 0x20
      FrSkySportSingleWireSerialmemdata = 0x5E;
    }
    else if(byte == 0x7D)
    {
      SendSerialDataToUART(FRSKY_STUFFING);
      SendSerialDataToUART(0x5D); // 0x7D xor 0x20
      FrSkySportSingleWireSerialmemdata = 0x5D;
    }
    else
    {
      SendSerialDataToUART(byte);
      FrSkySportSingleWireSerialmemdata = byte;
    }
    crc += byte;
    crc += crc >> 8; crc &= 0x00ff;
}

void FrSkySportSingleWireSerial::sendCrc()
{
  // Send and reset CRC
  sendByte(0xFF - crc);
  FrSkySportSingleWireSerialmemdata = 0xFF - crc;
  crc = 0;
}

void FrSkySportSingleWireSerial::sendData(uint16_t dataTypeId, uint32_t data)
{
    setMode(TX);
    sendByte(FRSKY_SENSOR_FRSKY_DATA_FRAME);
    uint8_t *bytes = (uint8_t*)&dataTypeId;
    sendByte(bytes[0]);
    sendByte(bytes[1]);
    bytes = (uint8_t*)&data;
    sendByte(bytes[0]);
    sendByte(bytes[1]);
    sendByte(bytes[2]);
    sendByte(bytes[3]);
    sendCrc();
    setMode(RX);
}

void FrSkySportSingleWireSerial::sendEmpty(uint16_t dataTypeId)
{
    setMode(TX);
    sendByte(0x00);
    uint8_t *bytes = (uint8_t*)&dataTypeId;
    sendByte(bytes[0]);
    sendByte(bytes[1]);
    for(uint8_t i = 0; i < 4; i++) sendByte(0x00);
    sendCrc();
    setMode(RX);
}

uint8_t FrSkySportSingleWireSerial::read()
{
  return FrSkySportSingleWireSerialmemdata;
}
