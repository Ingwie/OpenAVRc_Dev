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


#ifndef FRSKY_H
#define FRSKY_H

#include <inttypes.h>
#include "../OpenAVRc.h"

#define FRSKY_SPORT_PACKET_SIZE   9

#define FRSKY_TIMEOUT10ms         100 // 1 second
#define WSHH_TIMEOUT10ms          60  // 600ms

// Enumerate FrSky packet codes
#define LINKPKT                   0xfe
#define USRPKT                    0xfd
#define BFSPPKT 				          0x1b
#define RXSPPKT 			        	  0x98
#define A11PKT                    0xfc
#define A12PKT                    0xfb
#define A21PKT                    0xfa
#define A22PKT                    0xf9
#define ALRM_REQUEST              0xf8
#define RSSI1PKT                  0xf7
#define RSSI2PKT                  0xf6
#define RSSI_REQUEST              0xf1

// FrSky PRIM IDs (1 byte)
#define DATA_FRAME                0x10

// FrSky old DATA IDs (1 byte)
#define GPS_ALT_BP_ID             0x01
#define TEMP1_ID                  0x02
#define RPM_ID                    0x03
#define FUEL_ID                   0x04
#define TEMP2_ID                  0x05
#define VOLTS_ID                  0x06
#define GPS_ALT_AP_ID             0x09
#define BARO_ALT_BP_ID            0x10
#define GPS_SPEED_BP_ID           0x11
#define GPS_LONG_BP_ID            0x12
#define GPS_LAT_BP_ID             0x13
#define GPS_COURS_BP_ID           0x14
#define GPS_DAY_MONTH_ID          0x15
#define GPS_YEAR_ID               0x16
#define GPS_HOUR_MIN_ID           0x17
#define GPS_SEC_ID                0x18
#define GPS_SPEED_AP_ID           0x19
#define GPS_LONG_AP_ID            0x1A
#define GPS_LAT_AP_ID             0x1B
#define GPS_COURS_AP_ID           0x1C
#define BARO_ALT_AP_ID            0x21
#define GPS_LONG_EW_ID            0x22
#define GPS_LAT_NS_ID             0x23
#define ACCEL_X_ID                0x24
#define ACCEL_Y_ID                0x25
#define ACCEL_Z_ID                0x26
#define CURRENT_ID                0x28
#define VARIO_ID                  0x30
#define VFAS_ID                   0x39
#define VOLTS_BP_ID               0x3A
#define VOLTS_AP_ID               0x3B
#define FRSKY_LAST_ID             0x3F
#define D_RSSI_ID                 0xF0
#define D_A1_ID                   0xF1
#define D_A2_ID                   0xF2

#define VFAS_D_HIPREC_OFFSET      2000


// FrSky new DATA IDs (2 bytes)
#define ALT_FIRST_ID              0x0100
#define ALT_LAST_ID               0x010f
#define VARIO_FIRST_ID            0x0110
#define VARIO_LAST_ID             0x011f
#define CURR_FIRST_ID             0x0200
#define CURR_LAST_ID              0x020f
#define VFAS_FIRST_ID             0x0210
#define VFAS_LAST_ID              0x021f
#define CELLS_FIRST_ID            0x0300
#define CELLS_LAST_ID             0x030f
#define T1_FIRST_ID               0x0400
#define T1_LAST_ID                0x040f
#define T2_FIRST_ID               0x0410
#define T2_LAST_ID                0x041f
#define RPM_FIRST_ID              0x0500
#define RPM_LAST_ID               0x050f
#define FUEL_FIRST_ID             0x0600
#define FUEL_LAST_ID              0x060f
#define ACCX_FIRST_ID             0x0700
#define ACCX_LAST_ID              0x070f
#define ACCY_FIRST_ID             0x0710
#define ACCY_LAST_ID              0x071f
#define ACCZ_FIRST_ID             0x0720
#define ACCZ_LAST_ID              0x072f
#define GPS_LONG_LATI_FIRST_ID    0x0800
#define GPS_LONG_LATI_LAST_ID     0x080f
#define GPS_ALT_FIRST_ID          0x0820
#define GPS_ALT_LAST_ID           0x082f
#define GPS_SPEED_FIRST_ID        0x0830
#define GPS_SPEED_LAST_ID         0x083f
#define GPS_COURS_FIRST_ID        0x0840
#define GPS_COURS_LAST_ID         0x084f
#define GPS_TIME_DATE_FIRST_ID    0x0850
#define GPS_TIME_DATE_LAST_ID     0x085f
#define A3_FIRST_ID               0x0900
#define A3_LAST_ID                0x090f
#define A4_FIRST_ID               0x0910
#define A4_LAST_ID                0x091f
#define AIR_SPEED_FIRST_ID        0x0a00
#define AIR_SPEED_LAST_ID         0x0a0f
#define POWERBOX_BATT1_FIRST_ID   0x0b00
#define POWERBOX_BATT1_LAST_ID    0x0b0f
#define POWERBOX_BATT2_FIRST_ID   0x0b10
#define POWERBOX_BATT2_LAST_ID    0x0b1f
#define POWERBOX_STATE_FIRST_ID   0x0b20
#define POWERBOX_STATE_LAST_ID    0x0b2f
#define POWERBOX_CNSP_FIRST_ID    0x0b30
#define POWERBOX_CNSP_LAST_ID     0x0b3f
#define RSSI_ID                   0xf101
#define ADC1_ID                   0xf102
#define ADC2_ID                   0xf103
#define SP2UART_A_ID              0xfd00
#define SP2UART_B_ID              0xfd01
#define BATT_ID                   0xf104
#define SWR_ID                    0xf105
#define XJT_VERSION_ID            0xf106
#define FUEL_QTY_FIRST_ID         0x0a10
#define FUEL_QTY_LAST_ID          0x0a1f

// Default sensor data IDs (Physical IDs + CRC)
#define DATA_ID_VARIO             0x00 // 0
#define DATA_ID_FLVSS             0xA1 // 1
#define DATA_ID_FAS               0x22 // 2
#define DATA_ID_GPS               0x83 // 3
#define DATA_ID_RPM               0xE4 // 4
#define DATA_ID_SP2UH             0x45 // 5
#define DATA_ID_SP2UR             0xC6 // 6

// FrSky wrong IDs ?
#define BETA_VARIO_ID      0x8030
#define BETA_BARO_ALT_ID   0x8010

#define SPORT_DATA_U8(packet)   (packet[4])
#define SPORT_DATA_S32(packet)  (*((int32_t *)(packet+4)))
#define SPORT_DATA_U32(packet)  (*((uint32_t *)(packet+4)))
#define HUB_DATA_U16(packet)    (*((uint16_t *)(packet+4)))

// Global Fr-Sky telemetry data variables
extern uint8_t frskyStreaming; // >0 (true) == data is streaming in. 0 = nodata detected for some time

void parseTelemHubByte(uint8_t byte);
#if defined(WS_HOW_HIGH)
extern uint8_t frskyUsrStreaming;
void parseTelemWSHowHighByte(uint8_t byte);
#endif

enum AlarmLevel {
  alarm_off = 0,
  alarm_yellow = 1,
  alarm_orange = 2,
  alarm_red = 3
};

#define ALARM_GREATER(channel, alarm)   ((g_model.telemetry.channels[channel].alarms_greater >> alarm) & 1)
#define ALARM_LEVEL(channel, alarm)     ((g_model.telemetry.channels[channel].alarms_level >> (2*alarm)) & 3)

#define TELEMETRY_STREAMING()           (frskyStreaming > 0)
#define TELEMETRY_RSSI()                (telemetryData.rssi[0].value)
#define TELEMETRY_RSSI_MIN()            (telemetryData.rssi[0].min)

#define TELEMETRY_CELL_VOLTAGE_MUTLIPLIER  2

#define TELEMETRY_BARO_ALT_AVAILABLE()  (telemetryData.value.baroAltitudeOffset)
#if defined(IMPERIAL_UNITS)
#define TELEMETRY_BARO_ALT_UNIT         "ft"
#else
#define TELEMETRY_BARO_ALT_UNIT         "m"
#endif

#define TELEMETRY_RELATIVE_BARO_ALT_BP  telemetryData.value.baroAltitude_bp
#define TELEMETRY_RELATIVE_BARO_ALT_AP  telemetryData.value.baroAltitude_ap
#define TELEMETRY_RELATIVE_GPS_ALT_BP   telemetryData.value.gpsAltitude_bp
#define TELEMETRY_GPS_SPEED_BP          telemetryData.value.gpsSpeed_bp
#define TELEMETRY_GPS_SPEED_AP          telemetryData.value.gpsSpeed_ap

#define TELEMETRY_BARO_ALT_PREPARE()
#define TELEMETRY_BARO_ALT_FORMAT       "%d,"
#define TELEMETRY_BARO_ALT_ARGS         telemetryData.value.baroAltitude_bp,
#define TELEMETRY_GPS_ALT_FORMAT        "%d,"
#define TELEMETRY_GPS_ALT_ARGS          telemetryData.value.gpsAltitude_bp,
#define TELEMETRY_SPEED_UNIT            (IS_IMPERIAL_ENABLE() ? SPEED_UNIT_IMP : SPEED_UNIT_METR)
#define TELEMETRY_GPS_SPEED_FORMAT      "%d,"
#define TELEMETRY_GPS_SPEED_ARGS        telemetryData.value.gpsSpeed_bp,
#define TELEMETRY_CELLS_ARGS            telemetryData.value.cellsSum / 10, telemetryData.value.cellsSum % 10, telemetryData.value.cellVolts[0]*2/100, telemetryData.value.cellVolts[0]*2%100, telemetryData.value.cellVolts[1]*2/100, telemetryData.value.cellVolts[1]*2%100, telemetryData.value.cellVolts[2]*2/100, telemetryData.value.cellVolts[2]*2%100, telemetryData.value.cellVolts[3]*2/100, telemetryData.value.cellVolts[3]*2%100, telemetryData.value.cellVolts[4]*2/100, telemetryData.value.cellVolts[4]*2%100, telemetryData.value.cellVolts[5]*2/100, telemetryData.value.cellVolts[5]*2%100,
#define TELEMETRY_CELLS_FORMAT          "%d.%d,%d.%02d,%d.%02d,%d.%02d,%d.%02d,%d.%02d,%d.%02d,"
#define TELEMETRY_CELLS_LABEL           "Cell volts,Cell 1,Cell 2,Cell 3,Cell 4,Cell 5,Cell 6,"
#define TELEMETRY_CURRENT_FORMAT        "%d.%02d,"
#define TELEMETRY_CURRENT_ARGS          telemetryData.value.current / 100, telemetryData.value.current % 100,
#define TELEMETRY_VFAS_FORMAT           "%d.%d,"
#define TELEMETRY_VFAS_ARGS             telemetryData.value.vfas / 10, telemetryData.value.vfas % 10,
#define TELEMETRY_VSPEED_FORMAT         "%c%d.%02d,"
#define TELEMETRY_VSPEED_ARGS           telemetryData.value.varioSpeed < 0 ? '-' : ' ', telemetryData.value.varioSpeed / 100, telemetryData.value.varioSpeed % 100,
#define TELEMETRY_ASPEED_FORMAT         "%d.%d,"
#define TELEMETRY_ASPEED_ARGS           telemetryData.value.airSpeed / 10, telemetryData.value.airSpeed % 10,

#define TELEMETRY_OPENXSENSOR()         (telemetryData.value.openXsensor)

#define TELEMETRY_CELL_VOLTAGE(k)         (telemetryData.value.cellVolts[k] * TELEMETRY_CELL_VOLTAGE_MUTLIPLIER)
#define TELEMETRY_MIN_CELL_VOLTAGE        (telemetryData.value.minCellVolts * TELEMETRY_CELL_VOLTAGE_MUTLIPLIER)

extern TelemetryData telemetryData;

#define START_STOP              0x7e
#define BYTESTUFF               0x7d
#define STUFF_MASK              0x20

// Receive buffer state machine state enum
enum FrSkyDataState {
  STATE_DATA_IDLE,
  STATE_DATA_START,
  STATE_DATA_IN_FRAME,
  STATE_DATA_XOR,
};

#define SEND_RSSI_ALARMS  6
#define SEND_MODEL_ALARMS 4
extern uint8_t frskyAlarmsSendState;

typedef enum {
  TS_IDLE = 0,  // waiting for 0x5e frame marker
  TS_DATA_ID,   // waiting for dataID
  TS_DATA_LOW,  // waiting for data low byte
  TS_DATA_HIGH, // waiting for data high byte
  TS_XOR = 0x80 // decode stuffed byte
} TS_STATE;

// FrSky D Protocol
void processHubPacket(uint8_t id, uint16_t value);
void frskyDProcessPacket(uint8_t *packet);

// FrSky S.PORT Protocol
void processSportPacket(uint8_t *packet);

void checkMinMaxAltitude();
void telemetryResetValue();
void telemetryReset();
void telemetryInit();

void telemetryInterrupt10ms();

typedef uint8_t frskyCellVoltage_t;

void frskySetCellsCount(uint8_t cellscount);
void frskySetCellVoltage(uint8_t battnumber, frskyCellVoltage_t cellVolts);
void frskyUpdateCells();
void adjustRTChour();

#endif
