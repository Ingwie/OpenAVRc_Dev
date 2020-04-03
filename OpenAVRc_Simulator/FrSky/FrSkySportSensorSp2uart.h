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


#ifndef _FRSKY_SPORT_SENSOR_SP2UART_H_
#define _FRSKY_SPORT_SENSOR_SP2UART_H_

#include "FrSkySportSensor.h"

#define SP2UARTB_DEFAULT_ID ID7
#define SP2UARTB_DATA_COUNT 2
#define SP2UARTB_ADC3_DATA_ID 0x0900
#define SP2UARTB_ADC4_DATA_ID 0x0910

#define SP2UARTB_ADC3_DATA_PERIOD 500
#define SP2UARTB_ADC4_DATA_PERIOD 500

class FrSkySportSensorSp2uart : public FrSkySportSensor
{
  public:
    FrSkySportSensorSp2uart(SensorId id = SP2UARTB_DEFAULT_ID);
    void setData(float adc3, float adc4);
    virtual void send(FrSkySportSingleWireSerial& serial, uint8_t id, uint32_t now);
    virtual uint16_t decodeData(uint8_t id, uint16_t appId, uint32_t data);
    float getAdc3();
    float getAdc4();

  private:
    uint32_t adc3Data;
    uint32_t adc4Data;
    uint32_t adc3Time;
    uint32_t adc4Time;
    float adc3;
    float adc4;
};

#endif // _FRSKY_SPORT_SENSOR_SP2UART_H_
