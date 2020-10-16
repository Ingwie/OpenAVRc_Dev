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

#define FRSKY_TIMEOUT10ms         100 // 1 second
#define FRSKY_TIMEOUT_FIRST       FRSKY_TIMEOUT10ms+1
#define WSHH_TIMEOUT10ms          60  // 600ms

// Enumerate FrSky packet_p2M codes
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

// FrSky old DATA IDs + OXS (1 byte)

/*  FrSky Hub Info
DataID Meaning       Unit   Range   Note
0x01   GPS altitude  m              Before”.”
0x02   Temperature1  °C     -30-250
0x03   RPM           BPS    0-60000
0x04   Fuel Level    %      0, 25, 50, 75, 100
0x05   Temperature2  °C     -30-250
0x06   Cell Volt     1/500v 0-4.2v, top 4 bits are cell #
0x09   GPS altitude  m              After “.”
0x10   Altitude      m      0-9999  Before “.”
0x11   GPS speed     Knots          Before “.”
0x12   Longitude     dddmm.mmmm     Before “.”
0x13   Latitude      ddmm.mmmm      Before “.”
0x14   Course        degree 0-360   Before “.”
0x15   Date/Month
0x16   Year
0x17   Hour /Minute
0x18   Second
0x19   GPS speed     Knots          After “.”
0x1A   Longitude     dddmm.mmmm     After “.”
0x1B   Latitude      ddmm.mmmm      After “.”
0x1C   Course        degree 0-360   After “.”
0x21   Altitude      m              After "."
0x22   Long - E/W
0x23   Lat. N/S
0x24   Acc-x         1/256g -8g ~ +8g
0x25   Acc-y         1/256g -8g ~ +8g
0x26   Acc-z         1/256g -8g ~ +8g
0x28   Current       1A   0-100A
0x3A   Voltage(amp sensor) 0.5v 0-48V Before “.”
0x3B   Voltage(amp sensor)            After “.”
*/
#define GPS_ALT_BP_ID             0x01<<1
#define TEMP1_ID                  0x02<<1
#define RPM_ID                    0x03<<1
#define FUEL_ID                   0x04<<1
#define TEMP2_ID                  0x05<<1
#define VOLTS_ID                  0x06<<1
#define GPS_ALT_AP_ID             0x09<<1
#define BARO_ALT_BP_ID            0x10<<1
#define GPS_SPEED_BP_ID           0x11<<1
#define GPS_LONG_BP_ID            0x12<<1
#define GPS_LAT_BP_ID             0x13<<1
#define GPS_COURS_BP_ID           0x14<<1
#define GPS_DAY_MONTH_ID          0x15<<1
#define GPS_YEAR_ID               0x16<<1
#define GPS_HOUR_MIN_ID           0x17<<1
#define GPS_SEC_ID                0x18<<1
#define GPS_SPEED_AP_ID           0x19<<1
#define GPS_LONG_AP_ID            0x1A<<1
#define GPS_LAT_AP_ID             0x1B<<1
#define GPS_COURS_AP_ID           0x1C<<1
#define BARO_ALT_AP_ID            0x21<<1
#define GPS_LONG_EW_ID            0x22<<1
#define GPS_LAT_NS_ID             0x23<<1
#define ACCEL_X_ID                0x24<<1
#define ACCEL_Y_ID                0x25<<1
#define ACCEL_Z_ID                0x26<<1
#define CURRENT_ID                0x28<<1
#define VARIO_ID                  0x30<<1
#define OXS_MAH_ID                0x35<<1
#define OXS_POWER_ID              0x37<<1
#define VFAS_ID                   0x39<<1
#define VOLTS_BP_ID               0x3A<<1
#define VOLTS_AP_ID               0x3B<<1
#define FRSKY_LAST_ID             0x3F<<1
#define D_RSSI_ID                 0xF0<<1
#define D_A1_ID                   0xF1<<1
#define D_A2_ID                   0xF2<<1

#define VFAS_D_HIPREC_OFFSET      2000


// FrSky new DATA IDs (was 2 bytes) -> Keep just one "middle" byte
#define X_ALT_ID                  0x10
#define X_VARIO_ID                0x11
#define X_CURR_ID                 0x20
#define X_VFAS_ID                 0x21
#define X_CELLS_ID                0x30
#define X_T1_ID                   0x40
#define X_T2_ID                   0x41
#define X_RPM_ID                  0x50
#define X_FUEL_ID                 0x60
#define X_ACCX_ID                 0x70
#define X_ACCY_ID                 0x71
#define X_ACCZ_ID                 0x72
#define X_GPS_LONG_LATI_ID        0x80
#define X_GPS_ALT_ID              0x82
#define X_GPS_SPEED_ID            0x83
#define X_GPS_COURS_ID            0x84
#define X_GPS_TIME_DATE_ID        0x85
#define X_A3_ID                   0x90
#define X_A4_ID                   0x91
#define X_AIR_SPEED_ID            0xA0
#define X_FUEL_QTY_ID             0xA1
#define X_POWERBOX_BATT1_ID       0xB0
#define X_POWERBOX_BATT2_ID       0xB1
#define X_POWERBOX_STATE_ID       0xB2
#define X_POWERBOX_CNSP_ID        0xB3
#define X_RSSI_ID                 0xF101
#define X_ADC1_ID                 0xF102
#define X_ADC2_ID                 0xF103
#define X_SP2UART_A_ID            0xFD00
#define X_SP2UART_B_ID            0xFD01
#define X_BATT_ID                 0xF104
#define X_SWR_ID                  0xF105
#define X_XJT_VERSION_ID          0xF106

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

#define SPORT_DATA_U8(packet_p2M)   (packet_p2M[4])
#define SPORT_DATA_S32(packet_p2M)  (*((int32_t *)(packet_p2M+4)))
#define SPORT_DATA_U32(packet_p2M)  (*((uint32_t *)(packet_p2M+4)))
#define HUB_DATA_U16(packet_p2M)    (*((uint16_t *)(packet_p2M+4)))

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

#define TELEMETRY_RELATIVE_BARO_ALT_BP  telemetryData.value.baroAltitude
#define TELEMETRY_RELATIVE_GPS_ALT_BP   (telemetryData.value.gpsAltitude  + telemetryData.value.gpsAltitudeOffset)
#define TELEMETRY_GPS_SPEED_BP          telemetryData.value.gpsSpeed_bp
#define TELEMETRY_GPS_SPEED_AP          telemetryData.value.gpsSpeed_ap

#define TELEMETRY_BARO_ALT_FORMAT       "%d,"
#define TELEMETRY_BARO_ALT_ARGS         telemetryData.value.baroAltitude,
#define TELEMETRY_GPS_ALT_FORMAT        "%d,"
#define TELEMETRY_GPS_ALT_ARGS          telemetryData.value.gpsAltitude,
#define TELEMETRY_SPEED_UNIT            (IS_IMPERIAL_ENABLE() ? SPEED_UNIT_IMP : SPEED_UNIT_METR)
#define TELEMETRY_GPS_SPEED_FORMAT      "%d,"
#define TELEMETRY_GPS_SPEED_ARGS        telemetryData.value.gpsSpeed_bp/10,
#define TELEMETRY_CELLS_ARGS            telemetryData.value.cellsSum / 10, telemetryData.value.cellsSum % 10, telemetryData.value.cellVolts[0]*2/100, telemetryData.value.cellVolts[0]*2%100, telemetryData.value.cellVolts[1]*2/100, telemetryData.value.cellVolts[1]*2%100, telemetryData.value.cellVolts[2]*2/100, telemetryData.value.cellVolts[2]*2%100, telemetryData.value.cellVolts[3]*2/100, telemetryData.value.cellVolts[3]*2%100, telemetryData.value.cellVolts[4]*2/100, telemetryData.value.cellVolts[4]*2%100, telemetryData.value.cellVolts[5]*2/100, telemetryData.value.cellVolts[5]*2%100,
#define TELEMETRY_CELLS_FORMAT          "%d.%d,%d.%02d,%d.%02d,%d.%02d,%d.%02d,%d.%02d,%d.%02d,"
#define TELEMETRY_CELLS_LABEL           "Cell volts,Cell 1,Cell 2,Cell 3,Cell 4,Cell 5,Cell 6,"
#define TELEMETRY_CURRENT_FORMAT        "%d.%02d,"
#define TELEMETRY_CURRENT_ARGS          telemetryData.value.current / 10, telemetryData.value.current % 10,
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
void frskyDProcessPacket(uint8_t *packet_p2M);

// FrSky S.PORT Protocol
void processSportPacket(uint8_t *packet_p2M);

void telemetryResetValue();
void telemetryPPMInit();

void telemetryInterrupt10ms();

typedef uint8_t frskyCellVoltage_t;

void frskySetCellsCount(uint8_t cellscount);
void frskySetCellVoltage(uint8_t battnumber, frskyCellVoltage_t cellVolts);
void frskyUpdateCells();
void adjustRTChour();

#endif
