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


#ifndef _FRSKY_SPORT_SENSOR_FLVSS_H_
#define _FRSKY_SPORT_SENSOR_FLVSS_H_

#include "FrSkySportSensor.h"

#define FLVSS_DEFAULT_ID ID2
#define FLVSS_DATA_COUNT 3
#define FLVSS_CELL_DATA_ID 0x0300

#define FLVSS_CELL_DATA_PERIOD 300

class FrSkySportSensorFlvss : public FrSkySportSensor
{
  public:
    FrSkySportSensorFlvss(SensorId id = FLVSS_DEFAULT_ID);
    void setData(float cell1, float cell2 = 0.0, float cell3 = 0.0, float cell4 = 0.0, float cell5 = 0.0, float cell6 = 0.0);
    virtual void send(FrSkySportSingleWireSerial& serial, uint8_t id, uint32_t now);
    virtual uint16_t decodeData(uint8_t id, uint16_t appId, uint32_t data);
    float getCell1();
    float getCell2();
    float getCell3();
    float getCell4();
    float getCell5();
    float getCell6();

  private:
    static uint32_t setCellData(uint8_t cellNum, uint8_t firstCellNo, float cell1, float cell2);
    uint32_t cellData1;
    uint32_t cellData2;
    uint32_t cellData3;
    uint32_t cellDataTime;
    float cell[6];
};

#endif // _FRSKY_SPORT_SENSOR_FLVSS_H_
