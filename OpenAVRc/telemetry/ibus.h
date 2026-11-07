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


// Telemetry sensor IDs
//
// Keep this list close to the original OpenAVRc layout.  The additional IDs
// below follow the FlySky iBUS / AFHDS2A sensor assignments used by OpenTX
// and FlySkyI6-derived implementations.
#define    AFHDS2A_SENSOR_RX_VOLTAGE      0x00
#define    AFHDS2A_SENSOR_TEMPERATURE     0x01
#define    AFHDS2A_SENSOR_RPM             0x02
#define    AFHDS2A_SENSOR_EXT_VOLTAGE     0x03
#define    AFHDS2A_SENSOR_CELL_VOLTAGE    0x04  // Average cell voltage, not individual C1..Cn
#define    AFHDS2A_SENSOR_BAT_CURRENT     0x05  // Battery current, A * 100
#define    AFHDS2A_SENSOR_FUEL            0x06  // Fuel / consumed capacity depending on sensor implementation
#define    AFHDS2A_SENSOR_RPM2            0x07  // Alternate RPM / capacity use on some implementations
#define    AFHDS2A_SENSOR_HEADING         0x08  // Heading, degrees
#define    AFHDS2A_SENSOR_CLIMB_RATE      0x09  // 2 bytes m/s * 100
#define    AFHDS2A_SENSOR_COG             0x0A  // Course over ground, degrees * 100
#define    AFHDS2A_SENSOR_GPS_STATUS      0x0B  // GPS status / satellite information
#define    AFHDS2A_SENSOR_ACC_X           0x0C  // Signed, m/s * 100
#define    AFHDS2A_SENSOR_ACC_Y           0x0D  // Signed, m/s * 100
#define    AFHDS2A_SENSOR_ACC_Z           0x0E  // Signed, m/s * 100
#define    AFHDS2A_SENSOR_VERTICAL_SPEED  0x12  // Signed, m/s * 100
#define    AFHDS2A_SENSOR_GROUND_SPEED    0x13  // m/s * 100
#define    AFHDS2A_SENSOR_GPS_DISTANCE    0x14  // Distance from home, metres
#define    AFHDS2A_SENSOR_AIRSPEED        0x7E  // Speed, km/h * 100 on FlySky implementations

#define    AFHDS2A_SENSOR_GPS_LAT         0x80  // 4 bytes signed WGS84, degrees * 1E7
#define    AFHDS2A_SENSOR_GPS_LON         0x81  // 4 bytes signed WGS84, degrees * 1E7
#define    AFHDS2A_SENSOR_GPS_ALT         0x82  // 4 bytes signed GPS altitude, m * 100
#define    AFHDS2A_SENSOR_ALT             0x83  // 4 bytes signed altitude, m * 100

#define    AFHDS2A_SENSOR_ALT_FLYSKY      0xF9  // Native FlySky altitude, signed metres
#define    AFHDS2A_SENSOR_GPS_FULL        0xFD  // Extended GPS group: fix, sats, lat, lon, alt
#define    AFHDS2A_SENSOR_VOLT_FULL       0xF0  // Extended voltage group: 0x03..0x07
#define    AFHDS2A_SENSOR_ACC_FULL        0xEF  // Extended motion group: 0x0C..0x11
#define    AFHDS2A_SENSOR_RX_SNR          0xFA
#define    AFHDS2A_SENSOR_RX_NOISE        0xFB
#define    AFHDS2A_SENSOR_RX_RSSI         0xFC
#define    AFHDS2A_SENSOR_RX_ERR_RATE     0xFE


#endif
