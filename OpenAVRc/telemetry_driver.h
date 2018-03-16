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


#ifndef telemetry_driver_h
#define telemetry_driver_h

#define TLM_USART0 0

void Usart0TransmitBuffer();
void Usart0EnableTx();
void Usart0EnableRx();
void Usart0DisableTx();
void Usart0DisableRx();
void Usart0Set8N1();
void Usart0Set8E2();
void Usart0Set9600BAUDS();    //Frsky "D" telemetry
void Usart0Set57600BAUDS();   //Frsky S.port telemetry
void Usart0Set125000BAUDS();  //DSM Serial protocol
void Usart0Set100000BAUDS();  //Multiprotocole Serial

#define USART0_RX_PACKET_SIZE 21 // Modified for FrSky via SPIMODULES.21
#define USART0_TX_PACKET_SIZE 26

extern uint8_t Usart0RxBuffer[USART0_RX_PACKET_SIZE];

extern uint8_t Usart0TxBuffer[USART0_TX_PACKET_SIZE];

extern uint8_t Usart0TxBufferCount;

extern uint8_t Usart0RxBufferCount;

///////////  Telemetry fields  ///////////////
#if defined(FRSKY)

#define RAW_FRSKY_MINMAX(v)       v.value
class TelemetryValueWithMin
{
public:
  uint8_t value;
  uint8_t min;
  void set(uint8_t value);
};

class TelemetryValueWithMinMax: public TelemetryValueWithMin
{
public:
  uint8_t max;
  void set(uint8_t value, uint8_t unit);
};

PACK(struct TelemetrySerialData {
  int16_t  baroAltitudeOffset;    //        spare reused
  int16_t  gpsAltitude_bp;        // 0x01   before punct
  int16_t  temperature1;          // 0x02   -20 .. 250 deg. celcius
  uint16_t rpm;                   // 0x03   0..60,000 revs. per minute
  uint16_t fuelLevel;             // 0x04   0, 25, 50, 75, 100 percent
  int16_t  temperature2;          // 0x05   -20 .. 250 deg. celcius
  uint16_t volts;                 // 0x06   1/500V increments (0..4.2V)
  uint32_t distFromEarthAxis;     //        2 spares reused
  int16_t  gpsAltitude_ap;        // 0x01+8 after punct
  uint8_t  cellVolts[12];         //        6 spares reused
  int16_t  baroAltitude_bp;       // 0x10   0..9,999 meters
  uint16_t gpsSpeed_bp;           // 0x11   before punct
  uint16_t gpsLongitude_bp;       // 0x12   before punct
  uint16_t gpsLatitude_bp;        // 0x13   before punct
  uint16_t gpsCourse_bp;          // 0x14   before punct (0..359.99 deg. -- seemingly 2-decimal precision)
  uint8_t  day;                   // 0x15
  uint8_t  month;                 // 0x15
  uint16_t year;                  // 0x16
  uint8_t  hour;                  // 0x17
  uint8_t  min;                   // 0x17
  uint16_t sec;                   // 0x18
  uint16_t gpsSpeed_ap;           // 0x11+8
  uint16_t gpsLongitude_ap;       // 0x12+8
  uint16_t gpsLatitude_ap;        // 0x13+8
  uint16_t gpsCourse_ap;          // 0x14+8
  uint32_t pilotLatitude;         //        2 spares reused
  uint32_t pilotLongitude;        //        2 spares reused
  uint16_t baroAltitude_ap;       // 0x21   after punct
  uint16_t gpsLongitudeEW;        // 0x1A+8 East/West
  uint16_t gpsLatitudeNS;         // 0x1B+8 North/South
  int16_t  accelX;                // 0x24   1/256th gram (-8g ~ +8g)
  int16_t  accelY;                // 0x25   1/256th gram (-8g ~ +8g)
  int16_t  accelZ;                // 0x26   1/256th gram (-8g ~ +8g)
  uint8_t  gpsDistNeeded:1;       //        1bits out of 16bits spare reused
  int8_t   gpsFix:2;              //        2bits out of 16bits spare reused: -1=never fixed, 0=not fixed now, 1=fixed
  uint8_t  openXsensor:1;         //        1bits out of 16bits spare reused: we receive data from the openXsensor
  uint8_t  cellsCount:4;          //        4bits out of 16bits spare reused
  uint8_t  minCellVolts;          //        8bits out of 16bits spare reused
  uint16_t current;               // 0x28   Current
  int16_t  spare[5];
  int32_t  varioAltitude_cm;
  int16_t  varioSpeed;            // 0x30  Vertical speed in cm/s

  uint16_t gpsDistance;
  int16_t  gpsAltitudeOffset;
  uint8_t  varioAltitudeQueuePointer;     // circular-buffer pointer
  uint8_t  minCellIdx;
  int16_t  cellsSum;
  uint16_t currentConsumption;    // 0x35 openXsensor only! Otherwise calculated by the Tx from current
  uint16_t currentPrescale;
  uint16_t power;                 // 0x37 openXsensor only! Otherwise calculated by the Tx from current and voltage
  int16_t  airSpeed;

  uint16_t vfas;                  // 0x39  Added to FrSky protocol for home made sensors with a better precision
  uint16_t volts_bp;              // 0x3A
  uint16_t volts_ap;              // 0x3B
  // end of FrSky Hub data

  /* next fields must keep this order! */
  int16_t  minAltitude;
  int16_t  maxAltitude;
  uint16_t maxRpm;
  int16_t  maxTemperature1;
  int16_t  maxTemperature2;
  uint16_t maxGpsSpeed;
  uint16_t maxGpsDistance;
  uint16_t maxAirSpeed;
  int16_t  minCell;
  int16_t  minCells;
  uint16_t minVfas;
  uint16_t maxCurrent;
  uint16_t maxPower;
  /* end */

  int16_t  dTE;
});


enum TelemAnas {
  TELEM_ANA_A1,
  TELEM_ANA_A2,
  TELEM_ANA_COUNT
};

struct TelemetryData {
  TelemetryValueWithMinMax analog[TELEM_ANA_COUNT];
  TelemetryValueWithMin    rssi[2];
  TelemetrySerialData value;
};
#endif

#endif
