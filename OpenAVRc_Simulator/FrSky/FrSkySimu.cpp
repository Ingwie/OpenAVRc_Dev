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
#include "../OpenAVRc_SimulatorMain.h"

#include "FrSkySimu.h"
#include <wx/log.h>
FrSkySportSensorAss ass;                               // Create ASS sensor with default ID
FrSkySportSensorFcs fcs;                               // Create FCS-40A sensor with default ID (use ID8 for FCS-150A)
FrSkySportSensorFlvss flvss1;                          // Create FLVSS sensor with default ID
//FrSkySportSensorFlvss flvss2(FrSkySportSensor::ID15);  // Create FLVSS sensor with given ID
FrSkySportSensorGps gps;                               // Create GPS sensor with default ID
FrSkySportSensorRpm rpm;                               // Create RPM sensor with default ID
FrSkySportSensorSp2uart sp2uart;                       // Create SP2UART Type B sensor with default ID
FrSkySportSensorVario vario;                           // Create Variometer sensor with default ID

FrSkySportTelemetry telemetrySport(true);              // Create telemetry object with polling

FrSkyDTelemetry telemetryD;                             // Create telemetry D object


void SendSerialDataToUART(uint8_t data)
{
  //wxString dat = wxString::Format(wxT("%X"),data);
  //wxLogDebug(dat);
  SendByteCom(data);
  parseTelemFrskyByte(data);
}


void frskySportSimuSetup()
{
  // Configure the telemetry serial port and sensors (remember to use & to specify a pointer to sensor)
  telemetrySport.begin( &ass, &fcs, &flvss1, /*&flvss2,*/ &gps, &rpm, &sp2uart, &vario);
}

void frskySportSimuloop()
{
#define POLLING_ENABLED

  // Set airspeed sensor (ASS) data
  ass.setData(Tele_Airspeed);  // Airspeed in km/h

  // Set current/voltage sensor (FCS) data
  // (set Voltage source to FAS in menu to use this data for battery voltage,
  //  set Current source to FAS in menu to use this data for current readins)
  fcs.setData(Tele_BattCurrent,   // Current consumption in amps
              Tele_BattVolt);  // Battery voltage in volts

  // Set LiPo voltage sensor (FLVSS) data (we use two sensors to simulate 8S battery
  // (set Voltage source to Cells in menu to use this data for battery voltage)
  flvss1.setData(Tele_Cell1, Tele_Cell2, Tele_Cell3, Tele_Cell4, Tele_Cell5, Tele_Cell6);  // Cell voltages in volts (cells 1-6)
  //flvss2.setData(Tele_Cell7, Tele_Cell8, Tele_Cell9, Tele_Cell10, Tele_Cell11, Tele_Cell12);                          // Cell voltages in volts (cells 7-8)

  // Set GPS data
  gps.setData(Tele_GPSLat, Tele_GPSLong,   // Latitude and longitude in degrees decimal (positive for N/E, negative for S/W)
              Tele_GPSAlt,                 // Altitude in m (can be negative)
              Tele_GPSSpeed,                 // Speed in m/s
              Tele_GPSSpeed,                 // Course over ground in degrees (0-359, 0 = north)
              Tele_GPSYear, Tele_GPSMonth, Tele_GPSDay,             // Date (year - 2000, month, day)
              Tele_GPSHour, Tele_GPSMinute, Tele_GPSSecond);           // Time (hour, minute, second) - will be affected by timezone setings in your radio

  // Set RPM/temperature sensor data
  // (set number of blades to 2 in telemetry menu to get correct rpm value)
  rpm.setData(Tele_RPM,    // Rotations per minute
              Tele_Temp1,   // Temperature #1 in degrees Celsuis (can be negative, will be rounded)
              Tele_Temp2);  // Temperature #2 in degrees Celsuis (can be negative, will be rounded)

  // Set SP2UART sensor data
  // (values from 0.0 to 3.3 are accepted)
  sp2uart.setData(Tele_Analog3,    // ADC3 voltage in volts
                  Tele_Analog4);   // ADC4 voltage in volts

// Set variometer data
  // (set Variometer source to VSpd in menu to use the vertical speed data from this sensor for variometer).
  vario.setData(Tele_VarioAlt,  // Altitude in meters (can be negative)
                Tele_VarioAccelH);  // Vertical speed in m/s (positive - up, negative - down)

  // Send the telemetry data, note that the data will only be sent for sensors
  // that are being polled at given moment
  telemetrySport.send();
}

void frskyDSimuloop()
{
  // Set current/voltage sensor (FAS) data
  // (set Voltage source to FAS in menu to use this data for battery voltage,
  //  set Current source to FAS in menu to use this data for current readins)
  telemetryD.setFasData(Tele_BattCurrent,   // Current consumption in amps
                        Tele_BattVolt);  // Battery voltage in volts

  // Set fuel sensor (FGS) data
  telemetryD.setFgsData(Tele_Fuel);  // Fuel level in percent

  // Set LiPo voltage sensor (FLVS) data (we use two sensors to simulate 8S battery
  // (set Voltage source to Cells in menu to use this data for battery voltage)
  telemetryD.setFlvsData(Tele_Cell1, Tele_Cell2, Tele_Cell3, Tele_Cell4, Tele_Cell5, Tele_Cell6
                         ,Tele_Cell7, Tele_Cell8, Tele_Cell9, Tele_Cell10, Tele_Cell11, Tele_Cell12);  // Cell voltages in volts

  // Set variometer sensor (FVAS) data
  telemetryD.setFvasData(Tele_VarioAlt,   // Altitude in m (can be nevative)
                         Tele_VarioAccelH);  // Vertical speed in m/s (can be nevative, 0.0m/s will be set when skipped)

  // Set GPS data
  telemetryD.setGpsData(Tele_GPSLat, Tele_GPSLong,   // Latitude and longitude in degrees decimal (positive for N/E, negative for S/W)
              Tele_GPSAlt,                 // Altitude in m (can be negative)
              Tele_GPSSpeed,                 // Speed in m/s
              Tele_GPSSpeed,                 // Course over ground in degrees (0-359, 0 = north)
              Tele_GPSYear, Tele_GPSMonth, Tele_GPSDay,             // Date (year - 2000, month, day)
              Tele_GPSHour, Tele_GPSMinute, Tele_GPSSecond);           // Time (hour, minute, second) - will be affected by timezone setings in your radio

  // Set triaxial acceleration sensor (TAS) data
  telemetryD.setTasData(Tele_AccelX,    // x-axis acceleration in g (can be negative)
                        Tele_AccelY,      // y-axis acceleration in g (can be negative)
                        Tele_AccelZ);  // z-axis acceleration in g (can be negative)

  // Set temperature sensor (TEMS) data
  telemetryD.setTemsData(Tele_Temp1,   // Temperature #1 in degrees Celsuis (can be negative)
                         Tele_Temp2);  // Temperature #2 in degrees Celsuis (can be negative)

  // Set RPM sensor (RPMS) data
  // (set number of blades to 2 in telemetry menu to get correct rpm value)
  telemetryD.setRpmsData(Tele_RPM);  // Rotations per minute

  // Send the telemetry data, note that the data will only be sent for sensors
  // that had their data set at least once. Also it will only be set in defined
  // time intervals, so not necessarily at every call to send() method.
  telemetryD.send();
}
