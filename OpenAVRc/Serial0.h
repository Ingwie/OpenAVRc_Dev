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

#if defined(CPUM2560)
#define TLM_USART0 0
#define MULTI_USART TLM_USART0
#define DSM_USART TLM_USART0
#define FRSKY_USART TLM_USART0
#endif


#define NUM_TELEM_RX_BUFFER  2
#define TELEM_RX_PACKET_SIZE 9   // Frsky packet size
#define USART0_TX_PACKET_SIZE 26 // used in Multiprotocole serial

extern uint8_t TelemetryRxBuffer[NUM_TELEM_RX_BUFFER][TELEM_RX_PACKET_SIZE];
extern uint8_t Usart0TxBufferCount;

///////////  Telemetry fields  ///////////////
#if defined(FRSKY)

void LoadTelemBuffer(uint8_t *data);

#define RAW_FRSKY_MINMAX(v)       v.value
#define IF_GPS_IS_FIXED           if (telemetryData.value.gpsFix)
#define IF_GPS_IS_NOT_FIXED       if (!telemetryData.value.gpsFix)
#define NUMCELLS 6

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
  int16_t  baroAltitudeOffset;
  int16_t  gpsAltitude;
  int16_t  temperature1;
  uint16_t rpm;
  uint16_t fuelLevel;
  int16_t  temperature2;
  uint16_t volts;
  uint32_t distFromEarthAxis;
  uint8_t  cellVolts[NUMCELLS];
  int16_t  baroAltitude;
  uint16_t gpsSpeed_bp;
  uint16_t gpsLongitude_bp;
  uint16_t gpsLatitude_bp;
  uint16_t gpsCourse_bp;
  uint8_t  day;
  uint8_t  month;
  uint8_t  year;
  uint8_t  hour;
  uint8_t  min;
  uint8_t  sec;
  uint16_t gpsSpeed_ap;
  uint16_t gpsLongitude_ap;
  uint16_t gpsLatitude_ap;
  uint16_t gpsCourse_ap;
  uint32_t pilotLatitude;
  uint32_t pilotLongitude;
  uint8_t  gpsLongitudeEW;
  uint8_t  gpsLatitudeNS;
  int16_t  accelX;
  int16_t  accelY;
  int16_t  accelZ;
  uint8_t  notUsed:1;         // to use
  uint8_t  gpsFix:1;          // 0=not fixed, 1=fixed
  uint8_t  gpsGetFirstData:1; // Used to wait another GPS data frame before setting gpsFix to 1;
  uint8_t  openXsensor:1;
  uint8_t  cellsCount:4;
  uint8_t  minCellVolts;
  uint16_t current;
  int16_t  varioSpeed;

  uint16_t gpsDistance;
  int16_t  gpsAltitudeOffset;
  uint8_t  minCellIdx;
  int16_t  cellsSum;
  uint16_t currentConsumption;
  uint16_t currentPrescale;
  uint16_t power;
  int16_t  airSpeed;
  uint16_t vfas;
  uint16_t volts_bp;
  uint16_t volts_ap;
  // end of FrSky Hub data + OXS

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
