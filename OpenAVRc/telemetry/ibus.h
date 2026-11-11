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


#ifndef IBUS_H
#define IBUS_H


// Telemetry sensors ID
#define    AFHDS2A_SENSOR_RX_VOLTAGE    0x00
#define    AFHDS2A_SENSOR_TEMPERATURE   0x01
#define    AFHDS2A_SENSOR_RPM           0x02
#define    AFHDS2A_SENSOR_EXT_VOLTAGE   0x03

#define    AFHDS2A_SENSOR_CLIMB_RATE    0x09  // 2 bytes m/s *100
#define    AFHDS2A_SENSOR_COG           0x0A  // 2 bytes  Course over ground (NOT heading, but direction of movement) in degrees * 100, 0.0..359.99 degrees. unknown max uint
//#define    AFHDS2A_SENSOR_GPS_STATUS    0x0B  // 2 bytes
#define    AFHDS2A_SENSOR_GROUND_SPEED  0x13  // 2 bytes m/s *100 different unit than build-in sensor

#define   AFHDS2A_SENSOR_GPS_LAT        0x80  // 4bytes signed WGS84 in degrees * 1E7
#define   AFHDS2A_SENSOR_GPS_LON        0x81  // 4bytes signed WGS84 in degrees * 1E7
#define   AFHDS2A_SENSOR_GPS_ALT        0x82  // 4bytes signed!!! GPS alt m*100
#define   AFHDS2A_SENSOR_ALT            0x83  // 4bytes signed!!! Alt m*100

// #define AFHDS2A_SENSOR_RX_SNR        0xfa
#define    AFHDS2A_SENSOR_RX_NOISE      0xfb
#define    AFHDS2A_SENSOR_RX_RSSI       0xfc
// #define AFHDS2A_SENSOR_RX_ERR_RATE   0xfe


#endif
