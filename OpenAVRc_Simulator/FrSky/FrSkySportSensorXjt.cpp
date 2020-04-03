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


#include "FrSkySportSensorXjt.h" 

FrSkySportSensorXjt::FrSkySportSensorXjt(SensorId id) : FrSkySportSensor(id) { }

uint16_t FrSkySportSensorXjt::decodeData(uint8_t id, uint16_t appId, uint32_t data)
{
  if((sensorId == id) || (sensorId == FrSkySportSensor::ID_IGNORE))
  {
    switch(appId)
    {
      case 0xF101:
        rssi = (uint8_t)data;
        return appId;
      case 0xF102:
        adc1 = ((uint8_t)data) * (3.3 / 255.0);
        return appId;
      case 0xF103:
        adc2 = ((uint8_t)data) * (3.3 / 255.0);
        return appId;
      case 0xF104:
        rxBatt = ((uint8_t)data) * (3.3 / 255.0) * 4.0;
        return appId;
      case 0xF105:
        swr = (uint8_t)data;
        return appId;
      case 0x0028:
        current = data / 10.0;
        return appId;
      case 0x003A:
        voltBD = data;
        break;
      case 0x003B:
        // DEVIATION FROM SPEC: FrSky protocol spec suggests 0.5 ratio, but in reality this ratio is 0.5238 (based on the information from internet).
        voltAD = data;
        voltage = (voltAD / 10.0 + voltBD);
        voltage /= 0.5238;
        return appId;
      case 0x0004:
        fuel = data;
        return appId;
      case 0x0006:
        // DEVIATION FROM SPEC: in reality cells are numbered from 0 not from 1 like in the FrSky protocol spec
        if((data & 0xF000) == 0x0) cell[0]  = (data & 0x0FFF) / 500.0; 
        if((data & 0xF000) == 0x1) cell[1]  = (data & 0x0FFF) / 500.0; 
        if((data & 0xF000) == 0x2) cell[2]  = (data & 0x0FFF) / 500.0; 
        if((data & 0xF000) == 0x3) cell[3]  = (data & 0x0FFF) / 500.0; 
        if((data & 0xF000) == 0x4) cell[4]  = (data & 0x0FFF) / 500.0; 
        if((data & 0xF000) == 0x5) cell[5]  = (data & 0x0FFF) / 500.0; 
        if((data & 0xF000) == 0x6) cell[6]  = (data & 0x0FFF) / 500.0; 
        if((data & 0xF000) == 0x7) cell[7]  = (data & 0x0FFF) / 500.0; 
        if((data & 0xF000) == 0x8) cell[8]  = (data & 0x0FFF) / 500.0; 
        if((data & 0xF000) == 0x9) cell[9]  = (data & 0x0FFF) / 500.0; 
        if((data & 0xF000) == 0xA) cell[10] = (data & 0x0FFF) / 500.0; 
        if((data & 0xF000) == 0xB) cell[11] = (data & 0x0FFF) / 500.0; 
        return appId;
      case 0x0010:
        altBD = (int16_t)data;
        break;
      case 0x0021:
        altAD = data;
        altitude = altBD + altAD / 100.0;
        return appId;
      case 0x0030:
        // DEVIATION FROM SPEC: Not documented in FrSky spec, added based on OpenTX sources.
        vsi = data / 100.0;
        return appId;
      case 0x0001:
        gpsAltBD = (int16_t)data;
        break;
      case 0x0009:
        gpsAltAD = (int16_t)data;  
        gpsAltitude = gpsAltBD + gpsAltAD / 100.0;
        return appId;
      case 0x0011:
        speedBD = data;
        break;
      case 0x0019:
        speedAD = data;  
        speed = speedBD + speedAD / 100.0;
        speed *= 0.51444; // Convert knots to m/s
        return appId;
      case 0x0012:
        // DEVIATION FROM SPEC: FrSky protocol spec says lat shall be sent as big endian, but it reality little endian is sent
        lonBD = data;
        break;
      case 0x001A:
        // DEVIATION FROM SPEC: FrSky protocol spec says lat shall be sent as big endian, but it reality little endian is sent
        lonAD = data;
        break;
      case 0x0022: 
        lonEW = (char)data;
        if(lonEW == 'W') lon = -((uint16_t)(lonBD / 100) + ((lonBD % 100) + (lonAD / 10000.0)) / 60.0);
        else if(lonEW == 'E') lon = (uint16_t)(lonBD / 100) + ((lonBD % 100) + (lonAD / 10000.0)) / 60.0;
        else lon = 0;
        return appId;
      case 0x0013:
        // DEVIATION FROM SPEC: FrSky protocol spec says lat shall be sent as big endian, but it reality little endian is sent
        latBD = data;
        break;
      case 0x001B:
        // DEVIATION FROM SPEC: FrSky protocol spec says lat shall be sent as big endian, but it reality little endian is sent
        latAD = data;
        break;
      case 0x0023:
        latNS = (char)data;
        if(latNS == 'S') lat = -((uint16_t)(latBD / 100) + ((latBD % 100) + (latAD / 10000.0)) / 60.0);
        else if(latNS == 'N') lat = (uint16_t)(latBD / 100) + ((latBD % 100) + (latAD / 10000.0)) / 60.0;
        else lat = 0;
        return appId;
      case 0x0014:
        cogBD = data; 
        break;
      case 0x001C:
        cogAD = data;
        cog = cogBD + cogAD / 100.0;
        return appId;
      case 0x0015:
        day = data & 0x00FF; data >>= 8;
        month = data & 0x00FF;
        break;
      case 0x0016:
        year = data;
        return appId;
      case 0x0017:
        hour = data & 0x00FF; data >>= 8;
        minute = data & 0x00FF;
        break;
      case 0x0018:
        second = data;  
        return appId;
      case 0x0024:
        accX = ((int16_t)data) / 1000.0;  
        return appId;
      case 0x0025:
        accY = ((int16_t)data) / 1000.0;  
        return appId;
      case 0x0026:
        accZ = ((int16_t)data) / 1000.0;  
        return appId;
      case 0x0002:
        t1 = (int16_t)data;  
        return appId;
      case 0x0005:
        t2 = (int16_t)data;
        return appId;
      case 0x0003:
        rpm = data * 30.0;
        return appId;
    }
  }
  return SENSOR_NO_DATA_ID;
}

uint8_t FrSkySportSensorXjt::getRssi() { return rssi; }
float FrSkySportSensorXjt::getAdc1() { return adc1; }
float FrSkySportSensorXjt::getAdc2() { return adc2; }
float FrSkySportSensorXjt::getRxBatt() { return rxBatt; }
uint8_t FrSkySportSensorXjt::getSwr() { return swr; }
float FrSkySportSensorXjt::getFuel() { return fuel; }
float FrSkySportSensorXjt::getCell1() { return cell[0]; }
float FrSkySportSensorXjt::getCell2() { return cell[1]; }
float FrSkySportSensorXjt::getCell3() { return cell[2]; }
float FrSkySportSensorXjt::getCell4() { return cell[3]; }
float FrSkySportSensorXjt::getCell5() { return cell[4]; }
float FrSkySportSensorXjt::getCell6() { return cell[5]; }
float FrSkySportSensorXjt::getCell7() { return cell[6]; }
float FrSkySportSensorXjt::getCell8() { return cell[7]; }
float FrSkySportSensorXjt::getCell9() { return cell[8]; }
float FrSkySportSensorXjt::getCell10() { return cell[9]; }
float FrSkySportSensorXjt::getCell11() { return cell[10]; }
float FrSkySportSensorXjt::getCell12() { return cell[11]; }
float FrSkySportSensorXjt::getCurrent() { return current; }
float FrSkySportSensorXjt::getVoltage() { return voltage; }
float FrSkySportSensorXjt::getAltitude() { return altitude; }
float FrSkySportSensorXjt::getVsi() { return vsi; }
float FrSkySportSensorXjt::getLat() { return lat; }
float FrSkySportSensorXjt::getLon() { return lon; }
float FrSkySportSensorXjt::getGpsAltitude() { return gpsAltitude; }
float FrSkySportSensorXjt::getSpeed() { return speed; }
float FrSkySportSensorXjt::getCog() { return cog; }
uint16_t FrSkySportSensorXjt::getYear() { return year; }
uint8_t FrSkySportSensorXjt::getMonth() { return month; }
uint8_t FrSkySportSensorXjt::getDay() { return day; }
uint8_t FrSkySportSensorXjt::getHour() { return hour; }
uint8_t FrSkySportSensorXjt::getMinute() { return minute; }
uint8_t FrSkySportSensorXjt::getSecond() { return second; }
float FrSkySportSensorXjt::getAccX() { return accX; }
float FrSkySportSensorXjt::getAccY() { return accY; }
float FrSkySportSensorXjt::getAccZ() { return accZ; }
float FrSkySportSensorXjt::getT1() { return t1; }
float FrSkySportSensorXjt::getT2() { return t2; }
float FrSkySportSensorXjt::getRpm() { return rpm; }
