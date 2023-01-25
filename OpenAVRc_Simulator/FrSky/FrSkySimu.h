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

#ifndef frskysimu_h
#define frskysimu_h

#if !defined(__UNIX__)
 #include  <windows.h>
#else

#endif

#include "FrSkySportSensor.h"
//#include "FrSkySportSensorXjt.h"
#include "FrSkySportSensorAss.h"
#include "FrSkySportSensorFcs.h"
#include "FrSkySportSensorFlvss.h"
#include "FrSkySportSensorGps.h"
#include "FrSkySportSensorRpm.h"
#include "FrSkySportSensorSp2uart.h"
#include "FrSkySportSensorVario.h"
#include "FrSkySportSingleWireSerial.h"
#include "FrSkySportTelemetry.h"

#include "FrSkyDTelemetry.h"

// Telemetry datas
extern float Tele_GPSLat;
extern float Tele_GPSLong;
extern float Tele_GPSAlt;
extern float Tele_GPSSpeed;
extern float Tele_GPSCourse;
extern int Tele_GPSYear;
extern int Tele_GPSMonth;
extern int Tele_GPSDay;
extern int Tele_GPSHour;
extern int Tele_GPSMinute;
extern int Tele_GPSSecond;

extern float Tele_AccelX;
extern float Tele_AccelY;
extern float Tele_AccelZ;

extern float Tele_VarioAlt;
extern float Tele_VarioAccelH;

extern int Tele_RSSITx;
extern int Tele_RSSIRx;

extern int Tele_Analog1;
extern int Tele_Analog2;
extern int Tele_Analog3;
extern int Tele_Analog4;

extern float Tele_BattVolt;
extern float Tele_BattCurrent;

extern float Tele_RPM;
extern float Tele_Fuel;
extern float Tele_Temp1;
extern float Tele_Temp2;
extern float Tele_Airspeed;

extern float Tele_Cell1;
extern float Tele_Cell2;
extern float Tele_Cell3;
extern float Tele_Cell4;
extern float Tele_Cell5;
extern float Tele_Cell6;


void frskySportSimuSetup();
void frskySportSimuloop();
void frskyDSimuloop();
void SendSerialDataToUART(uint8_t data);
#endif
