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


#include "FrSkySportSensorFlvss.h" 

FrSkySportSensorFlvss::FrSkySportSensorFlvss(SensorId id) : FrSkySportSensor(id) { }

uint32_t FrSkySportSensorFlvss::setCellData(uint8_t cellNum, uint8_t firstCellNo, float cell1, float cell2)
{
  uint16_t cell1Data = cell1 * 500.0;
  uint16_t cell2Data = cell2 * 500.0;
  uint32_t cellData = 0;
  
  cellData = cell2Data & 0x0FFF;
  cellData <<= 12;
  cellData |= cell1Data & 0x0FFF;
  cellData <<= 4;
  cellData |= cellNum & 0x0F;
  cellData <<= 4;
  cellData |= firstCellNo & 0x0F;
  
  return cellData;
}

void FrSkySportSensorFlvss::setData(float cell1, float cell2, float cell3, float cell4, float cell5, float cell6)
{
  uint8_t numCells = 1; // We assume at least one cell
  if(cell2 > 0)
  {
    numCells++;
    if(cell3 > 0)
    {
      numCells++;
      if(cell4 > 0)
      {
        numCells++;
        if(cell5 > 0)
        {
          numCells++;
          if(cell6 > 0)
          {
            numCells++;
          }
        }
      }
    }
  }
  cellData1 = setCellData(numCells, 0, cell1, cell2); 
  if(numCells > 2) cellData2 = setCellData(numCells, 2, cell3, cell4); else cellData2 = 0;
  if(numCells > 4) cellData3 = setCellData(numCells, 4, cell5, cell6); else cellData3 = 0;
}

void FrSkySportSensorFlvss::send(FrSkySportSingleWireSerial& serial, uint8_t id, uint32_t now)
{
  if(sensorId == id)
  {
    if(now > cellDataTime)
    {
      cellDataTime = now + FLVSS_CELL_DATA_PERIOD;
      switch(sensorDataIdx)
      {
        case 0:
          serial.sendData(FLVSS_CELL_DATA_ID, cellData1); if(cellData2 != 0) sensorDataIdx = 1; else sensorDataIdx = 0; 
          break;
        case 1:
          serial.sendData(FLVSS_CELL_DATA_ID, cellData2); if(cellData3 != 0) sensorDataIdx = 2; else sensorDataIdx = 0; 
          break;
        case 2:
          serial.sendData(FLVSS_CELL_DATA_ID, cellData3); sensorDataIdx = 0;
          break;
      }
    }
    else
    {
      serial.sendEmpty(FLVSS_CELL_DATA_ID);
    }
  }
}

uint16_t FrSkySportSensorFlvss::decodeData(uint8_t id, uint16_t appId, uint32_t data)
{
  if((sensorId == id) || (sensorId == FrSkySportSensor::ID_IGNORE))
  {
    if(appId == FLVSS_CELL_DATA_ID)
    {
      uint8_t firstCellNo = data & 0x0F; data >>= 4;
      uint8_t cellNum = data & 0x0F; data >>= 4;
      cell[firstCellNo]     = (data & 0x0FFF) / 500.0; data >>= 12;
      cell[firstCellNo + 1] = (data & 0x0FFF) / 500.0;
      for(uint8_t i = cellNum; i <= 5; i++) cell[i] = 0.0;
      return appId;
    }
  }
  return SENSOR_NO_DATA_ID;
}

float FrSkySportSensorFlvss::getCell1() { return cell[0]; }
float FrSkySportSensorFlvss::getCell2() { return cell[1]; }
float FrSkySportSensorFlvss::getCell3() { return cell[2]; }
float FrSkySportSensorFlvss::getCell4() { return cell[3]; }
float FrSkySportSensorFlvss::getCell5() { return cell[4]; }
float FrSkySportSensorFlvss::getCell6() { return cell[5]; }
