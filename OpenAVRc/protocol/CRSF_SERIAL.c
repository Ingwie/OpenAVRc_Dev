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
// Original file from https://github.com/phobos-/deviation/blob/elrs/src/protocol/crsf_uart.c
/*
 * ============================================================================
 * OPENAVRc / ATmega2560 : CRSF + ExpressLRS 4.x + FrSky S.Port bridge
 * ============================================================================
 * DATA PATH (normal operation):
 *   mixer -> build_CRSF_data_pkt() -> USART0 TX -> ELRS JR module -> receiver.
 *   module -> USART0 RX ISR -> CRSF_MenuRxByte() -> CRC8 D5 validation.
 *   0x14 => S.Port Rx/Tx = uplink/downlink LQ (%), not dBm.
 *   0x08 battery ID 0 => RxBt; ID 1 => oXs VFAS/Current/Capacity.
 *   0x0E source_id 0..127 => Cells (C1..C6); source_id 128 => RxBt; 0x02 => GPS.
 *   0x0D/0x0C/0x03 => one-slot deferred T1/T2, RPM, optional GPS time.
 *   CRSF_SportTelemetryMain() in perMain() emits deferred sensor packets.
 *   FrSky's existing 10ms handler drains the 3-slot S.Port queue.
 *
 * MENU (only when model setup is open):
 *   CRSF_MenuTick() is called by the LCD/menu task and sets an 8-bit heartbeat.
 *   The radio timer is allowed to send 0x28/0x2C/0x2D requests while the
 *   heartbeat is fresh. No direct menuHandlers[] pointer access from timer ISR.
 *   Long 0x2B parameter strings are decoded outside the UART and radio ISRs.
 *
 * STABILITY RULES FOR AVR 8-BIT:
 *   - Keep UART callback short; never draw or calculate GPS coordinates there.
 *   - 16-bit timing values shared with RF ISR must be updated atomically.
 *   - Do not enqueue an unlimited number of S.Port packets (3 slots only).
 *   - Never access a 16-bit function pointer while another context edits it.
 *   - Preserve the validated GPS speed conversion (oXs 2468 cm/s -> 88.8 km/h).
 *
 * The model's baudrate bit is NOT the RF Packet Rate. Changing Packet Rate
 * sends the field index supplied by the ELRS module; UART remains at 400K.
 * WARNING: the heartbeat change reduces a menu/timer concurrency risk; it
 * does not by itself prove the cause of previous unexpected resets.
 * ============================================================================
 */

#include "../OpenAVRc.h"

// IMPORTANT: pulses2MHz shares CRSF TX frame bytes 0..25 and
// protocol-tail bytes >= PULSES_BYTE_OFFSET_VAR (125 on ATmega2560).
// Extend the already existing union member; do not allocate a second buffer.
// D: historical unused fields removed only on CPUM2560, with these checks.
#if defined(CPUM2560) || defined(CPUXMEGA)
typedef char CRSF_PULSE_REUSE_FITS[
  (sizeof(crsfSt_t) <= PULSES_BYTE_OFFSET_VAR) ? 1 : -1];
// The ISR sends 26 bytes from Usart0TxBuffer_p2M. ELRS states start
// strictly beyond that payload, so TX never overwrites decoder state.
typedef char CRSF_TX_REUSE_DOES_NOT_OVERLAP[
  (offsetof(crsfSt_t, elrs_reuse) == CRSF_CHAN_PACKET_SIZE) ? 1 : -1];
typedef char CRSF_TAIL_REUSE_DOES_NOT_OVERLAP[
  ((offsetof(crsfSt_t, elrs_reuse) + sizeof(((crsfSt_t*)0)->elrs_reuse))
       <= PULSES_BYTE_OFFSET_VAR) ? 1 : -1];
// G from E: four bytes saved by one phase tick plus one spare byte
// are now used for S.Port snapshots. 99-byte state ends at offset 125.
typedef char CRSF_REUSE_G_FROM_E_EXACT_LAYOUT[
  (sizeof(((crsfSt_t*)0)->elrs_reuse) == 99U) ? 1 : -1];
#endif

// define pulses2MHz reusable values (13 bytes max)
#define CRSF_FREQ_RATE_MEM  BYTE_P2M(1)
#define CRSF_40_MS_Flag     BYTE_P2M(2)
#define CRSF_40_MS_Flipflop BYTE_P2M(3)

#define CRSF_RATE_PERIOD    WORD_P2M(1)
//***********************************************//


// Device addresses
#define CRSF_ADDR_BROADCAST  0x00  //  Broadcast address
#define CRSF_ADDR_USB        0x10  //  USB Device
#define CRSF_ADDR_BLUETOOTH  0x12  //  Bluetooth Module
#define CRSF_ADDR_PRO_CORE   0x80  //  TBS CORE PNP PRO
//  #define CRSF_ADDR_  0x8A       //  Reserved
#define CRSF_ADDR_PRO_CURR   0xC0  //  PNP PRO digital current sensor
#define CRSF_ADDR_PRO_GPS    0xC2  //  PNP PRO GPS
#define CRSF_ADDR_BLACKBOX   0xC4  //  TBS Blackbox
#define CRSF_ADDR_FC         0xC8  //  Flight controller
//  #define CRSF_ADDR_       0xCA  //  Reserved
#define CRSF_ADDR_RACETAG    0xCC  //  Race tag
#define CRSF_ADDR_RADIO      0xEA  //  Radio Transmitter
//  #define CRSF_ADDR_       0xEB  //  Reserved
#define CRSF_ADDR_RECEIVER   0xEC  //  Crossfire / UHF receiver
#define CRSF_ADDR_MODULE     0xEE  //  Crossfire transmitter

// Frame Type
#define CRSF_TYPE_GPS              0x02
// Additional oXs RP2040 sensors (ExpressLRS 4.x).
// Time is a distinct CRSF frame; it is NOT included in the GPS position 0x02.
#define CRSF_TYPE_GPS_TIME         0x03
#define CRSF_TYPE_RPM              0x0C
#define CRSF_TYPE_TEMPERATURE      0x0D
#define CRSF_TYPE_BATTERY          0x08
#define CRSF_TYPE_VOLTAGES         0x0E  // Voltage Group: source ID + big-endian millivolts
#define CRSF_TYPE_VIDEO            0x08
#define CRSF_TYPE_LINK             0x14
#define CRSF_TYPE_CHANNELS         0x16
#define CRSF_TYPE_ATTITUDE         0x1E
#define CRSF_TYPE_FLIGHT_MODE      0x21
#define CRSF_TYPE_PING_DEVICES     0x28
#define CRSF_TYPE_DEVICE_INFO      0x29
#define CRSF_TYPE_REQUEST_SETTINGS 0x2A
#define CRSF_TYPE_SETTINGS_ENTRY   0x2B
#define CRSF_TYPE_SETTINGS_READ    0x2C
#define CRSF_TYPE_SETTINGS_WRITE   0x2D

#define TELEMETRY_RX_PACKET_SIZE   64


#define CRSF_DATARATE             115200UL // Half-duplex external module: first TX-only link test at 115K2
#define CRSF_FRAME_PERIOD         4000   // 4ms
#define CRSF_CHANNELS             16
#define CRSF_CHAN_PACKET_SIZE     26
#define CRSF_SET_PACKET_SIZE      8

// ELRS command
#define ELRS_ADDRESS               0xEE
#define ELRS_BIND_COMMAND          0xFF
#define ELRS_WIFI_COMMAND          0xFE
#define ELRS_PKT_RATE_COMMAND      0x01
#define ELRS_TLM_RATIO_COMMAND     0x02
#define ELRS_POWER_COMMAND         0x03

const static RfOptionSettingsvar_t RfOpt_CRSF_Ser[] PROGMEM =
{
  /*rfProtoNeed*/BOOL1USED | BOOL3USED, // Telemetry option + baudrate (4-row CRSF menu)
  /*rfSubTypeMax*/0,
  /*rfOptionValue1Min*/0,
  /*rfOptionValue1Max*/0x7F, // 0x6x=subGHz / 0x7x=2.4G cached rate indices
  /*rfOptionValue2Min*/0, // TLM ratio menu selection; written through CRSF parameter 0x2D
  /*rfOptionValue2Max*/9, // OFF, 1:128 .. 1:2, Std, Race
  /*rfOptionValue3Max*/9, // SIMU selection; actual module RF values read into RAM
};

// Fixed 5-character menu labels; hardware edits are written through the discovered CRSF parameter.
const pm_char STR_CRSF_TLM_VALUES[] PROGMEM = "OFF  ""1:128""1:64 ""1:32 ""1:16 ""1:8  ""1:4  ""1:2  ""STD  ""RACE ";
const pm_char STR_CRSF_TLM_LABEL[] PROGMEM = "TLM Ratio";
// Local S.Port switch: 0=ON by default (compatible with existing models),
// 1=OFF. This does NOT change the module's RF TLM Ratio.
#define CRSF_LOCAL_TELEM_OFF (g_model.rfOptionBool1)
const pm_char STR_CRSF_BAUD_LABEL[] PROGMEM = "Baudrate";
const pm_char STR_CRSF_BAUD_VALUES[] PROGMEM = "115.2K""400K  ";
// Six-character labels, indexed by CRSF_MenuBand: unknown / 868 / 915 / 2.4.
const pm_char STR_CRSF_BAND_VALUES[] PROGMEM = "------""868MHz""915MHz""2.4GHz""NO-ACK""RF?   ""NO-PAR""P-CRC ""P-ADR ""P-ID  ""P-FMT ""RX-ERR";
const pm_char STR_CRSF_WAIT_RATE[] PROGMEM = "--";
// TX discovery feedback on the MODEL/Module line. Six characters each.
// The module, not EEPROM, provides all three RF settings below.
// Actual RF domain from ELRS CRSF_INFO (parameter response 0x2B).
// 0 unknown; 1=EU868,2=915,3=2.4G,4=433,5=866,6..9=dual-band.
// Each label is exactly six characters, for the existing LCD row.
const pm_char STR_CRSF_TX_BAND_READ[] PROGMEM =
    "------" "868MHz" "915MHz" "2.4GHz" "433MHz" "866MHz"
    "868/2G" "915/2G" "433/2G" "866/2G";
const pm_char STR_CRSF_TX_SCAN_STATUS[] PROGMEM =
    "INIT  "  // startup grace period
    "PING  "  // waiting for DEVICE_INFO 0x29
    "TX-OK "  // DEVICE_INFO received, parameters being read
    "READ  "  // fallback to direct reads, no 0x29
    "OK    "  // some TX settings identified
    "NO-ACK"  // no DEVICE_INFO or PARAMETER_SETTINGS_ENTRY
    "NO-PAR"  // DEVICE_INFO replied but no usable RF settings
    "CRCERR"  // module-side frame received with bad CRC
    "ADDR? "  // device frame addressed to another origin
    "WAIT  "; // no data yet

// Fixed six-character RF-power UI labels. On hardware, selectable entries are
// validated against the discovered ELRS Max Power parameter and written via 0x2D.
// SIMU reuses the same index convention locally. Stored in PROGMEM.
const pm_char STR_CRSF_POWER_UI_TEST_VALUES[] PROGMEM =
    "--    " "DYN   " "10mW  " "25mW  " "50mW  "
    "100mW " "250mW " "500mW " "1000mW" "2000mW";
#define CRSF_MENU_FAST_BAUD         (g_model.rfOptionBool3) // 0=115200, 1=400000 (M2560)
const uint8_t CRSF_RATE24[] PROGMEM = {0xFF,250,150,50,25}; // 0xFF mean 500
const uint8_t CRSF_RATE900[] PROGMEM = {200,100,50,25};

#define READ_CRSF_FREQ      (g_model.rfOptionValue1>>4)
#define WRITE_CRSF_FREQ(x)  (g_model.rfOptionValue1 = (g_model.rfOptionValue1 & 0x0F) | (x<<4))
#define READ_CRSF_RATE      (g_model.rfOptionValue1&0x0F)
#define WRITE_CRSF_RATE(x)  (g_model.rfOptionValue1 = (g_model.rfOptionValue1 & 0xF0) | x)
#define IS_CRSF_24_FREQ     (READ_CRSF_FREQ == 5)
#define GET_CRSF_NUM_RATE   (IS_CRSF_24_FREQ ? sizeof(CRSF_RATE24) : sizeof(CRSF_RATE900))

static void CRSF_Reset()
{
  USART_DISABLE_TX(CRSF_USART);
  USART_DISABLE_RX(CRSF_USART);
#if defined(CPUXMEGA)
  CRSF_USART.CTRLA &= ~USART_TXCINTLVL_gm;
  RF_PORT.DIRCLR = USART_TXD_PIN_bm;
#endif
#if defined(CPUM2560) || defined(CPUXMEGA)
  parseSerialTelemFunction = parseSerialTelemFakeByte;
#endif
}

static const uint8_t ZZcrsf_crc8tab[] PROGMEM =
{
  0x00, 0xD5, 0x7F, 0xAA, 0xFE, 0x2B, 0x81, 0x54, 0x29, 0xFC, 0x56, 0x83, 0xD7, 0x02, 0xA8, 0x7D,
  0x52, 0x87, 0x2D, 0xF8, 0xAC, 0x79, 0xD3, 0x06, 0x7B, 0xAE, 0x04, 0xD1, 0x85, 0x50, 0xFA, 0x2F,
  0xA4, 0x71, 0xDB, 0x0E, 0x5A, 0x8F, 0x25, 0xF0, 0x8D, 0x58, 0xF2, 0x27, 0x73, 0xA6, 0x0C, 0xD9,
  0xF6, 0x23, 0x89, 0x5C, 0x08, 0xDD, 0x77, 0xA2, 0xDF, 0x0A, 0xA0, 0x75, 0x21, 0xF4, 0x5E, 0x8B,
  0x9D, 0x48, 0xE2, 0x37, 0x63, 0xB6, 0x1C, 0xC9, 0xB4, 0x61, 0xCB, 0x1E, 0x4A, 0x9F, 0x35, 0xE0,
  0xCF, 0x1A, 0xB0, 0x65, 0x31, 0xE4, 0x4E, 0x9B, 0xE6, 0x33, 0x99, 0x4C, 0x18, 0xCD, 0x67, 0xB2,
  0x39, 0xEC, 0x46, 0x93, 0xC7, 0x12, 0xB8, 0x6D, 0x10, 0xC5, 0x6F, 0xBA, 0xEE, 0x3B, 0x91, 0x44,
  0x6B, 0xBE, 0x14, 0xC1, 0x95, 0x40, 0xEA, 0x3F, 0x42, 0x97, 0x3D, 0xE8, 0xBC, 0x69, 0xC3, 0x16,
  0xEF, 0x3A, 0x90, 0x45, 0x11, 0xC4, 0x6E, 0xBB, 0xC6, 0x13, 0xB9, 0x6C, 0x38, 0xED, 0x47, 0x92,
  0xBD, 0x68, 0xC2, 0x17, 0x43, 0x96, 0x3C, 0xE9, 0x94, 0x41, 0xEB, 0x3E, 0x6A, 0xBF, 0x15, 0xC0,
  0x4B, 0x9E, 0x34, 0xE1, 0xB5, 0x60, 0xCA, 0x1F, 0x62, 0xB7, 0x1D, 0xC8, 0x9C, 0x49, 0xE3, 0x36,
  0x19, 0xCC, 0x66, 0xB3, 0xE7, 0x32, 0x98, 0x4D, 0x30, 0xE5, 0x4F, 0x9A, 0xCE, 0x1B, 0xB1, 0x64,
  0x72, 0xA7, 0x0D, 0xD8, 0x8C, 0x59, 0xF3, 0x26, 0x5B, 0x8E, 0x24, 0xF1, 0xA5, 0x70, 0xDA, 0x0F,
  0x20, 0xF5, 0x5F, 0x8A, 0xDE, 0x0B, 0xA1, 0x74, 0x09, 0xDC, 0x76, 0xA3, 0xF7, 0x22, 0x88, 0x5D,
  0xD6, 0x03, 0xA9, 0x7C, 0x28, 0xFD, 0x57, 0x82, 0xFF, 0x2A, 0x80, 0x55, 0x01, 0xD4, 0x7E, 0xAB,
  0x84, 0x51, 0xFB, 0x2E, 0x7A, 0xAF, 0x05, 0xD0, 0xAD, 0x78, 0xD2, 0x07, 0x53, 0x86, 0x2C, 0xF9,
};

uint8_t crsf_crc8(const uint8_t *ptr, uint8_t len)
{
// crc implementation from CRSF protocol document rev7
  uint8_t crc = 0;
  uint_farptr_t crctab = pgm_get_far_address(ZZcrsf_crc8tab);
  for (uint8_t i=0; i < len; i++)
    {
      crc = pgm_read_byte_far(crctab + (crc ^ *ptr--));
    }
  return crc;
}

// The old menu-driven ELRS scanner is removed: all radio parameter reads
// are handled by CRSF_PowerMain()/CRSF_PowerRxFrame(). Keep ONE CRSF UART
// frame buffer for both module replies and sensor telemetry.
#if defined(CPUM2560) || defined(CPUXMEGA)
#define crsf_menu_rx_count (pulses2MHz.crsf_st.elrs_reuse.uart_rx_count)
#define crsf_menu_rx_size (pulses2MHz.crsf_st.elrs_reuse.uart_rx_size)
static uint8_t crsf_menu_rxbuf[64];
// Used only by the simulator's legacy RF-band text.
#if defined(SIMU)
static uint8_t crsf_menu_band = 0;
#endif

// CRSF -> FrSky S.Port bridge.  This uses OpenAVRc's EXISTING S.Port
// packet queue/decoder, not a second telemetry engine.  Receive callbacks
// run in the UART ISR; avoid any display/telemetry calculations here.
//
// Link 0x14 : Rx=LQ uplink (%), Tx=LQ downlink (%) using the FrSky
//             RSSI (0xF101) and SWR (0xF105) slots.
// Battery 0x08: optional battery ID is authoritative when present.
// ID 0 -> receiver/ELRS RxBt; ID 1 (and other non-zero IDs) -> external
// battery VFAS + Curr + Capa. Legacy frames without the optional ID keep
// the previous zero-current/zero-capacity heuristic for compatibility.
// Voltage Group 0x0E source_id 0..127 supplies Cells (first 6 used); 128 supplies RxBt.
// oXs 0x0D: temperature 1/2 (signed 0.1 deg C -> integer deg C S.Port).
// oXs 0x0C: source 0 RPM; compensated for OpenAVRc's blades divider.
// oXs 0x03: optional GPS date/time. The current oXs firmware may not send it.
// Link SNR and dBm are NOT written into the oXs T1/T2 fields.
//
// FrSky engine (telemetryInterrupt10ms) consumes the queued frames and
// updates frskyStreaming itself.  This also works with the original
// frsky.cpp, without requiring the special CRSF_SportTelemetryTick hook.
static void CRSF_PowerMain(void);

#if defined(FRSKY)
// Extra sensor frames are short but must NOT do math inside the UART ISR.
// This is a SINGLE-SLOT mailbox: if a frame arrives while another is pending,
// the new one is dropped (not overwritten) and a later periodic measurement
// will retry. Queue size is bounded to protect the ATmega2560's RAM.
// Header and CRC are checked by CRSF_MenuRxByte before data are stored.
static volatile uint8_t crsf_extra_pending = 0;
static uint8_t crsf_extra_type = 0;
static uint8_t crsf_extra_size = 0;
static uint8_t crsf_extra_payload[9];
// ELRS 0x14/0x08 sensor mailboxes. UART ISR only copies raw values;
// S.Port queue operations execute later in perMain(). This avoids nesting
// LoadFrskyTelemBuffer() into the UART RX interrupt while menus are drawn.
static volatile uint8_t crsf_link_pending = 0;
static uint8_t crsf_link_rx_lq = 0;
static uint8_t crsf_link_tx_lq = 0;
static volatile uint8_t crsf_battery_pending = 0;
// Written by UART RX ISR, consumed by CRSF_LinkBatteryMain().
static volatile uint16_t crsf_battery_volts_dv = 0;
static volatile uint16_t crsf_battery_current_da = 0;
// Limit bridge output to one S.Port sensor per 10ms: the OpenAVRc FrSky
// queue has only 3 slots. VFAS has priority; the other three fields
// (Curr, RSSI, SWR) are sent by round-robin without flooding the queue.
// This is main-loop state, never touched by the UART RX interrupt.
#define crsf_sport_to_send (pulses2MHz.crsf_st.elrs_reuse.sport_to_send)
#define crsf_sport_next_sensor (pulses2MHz.crsf_st.elrs_reuse.sport_next_sensor)
static uint8_t crsf_sport_rx_lq = 0;
#define crsf_sport_tx_lq (pulses2MHz.crsf_st.elrs_reuse.sport_tx_lq)
// G: snapshots emitted by perMain only; ISR mailboxes remain independent.
#define crsf_sport_battery_dv (pulses2MHz.crsf_st.elrs_reuse.sport_battery_dv)
#define crsf_sport_current_da (pulses2MHz.crsf_st.elrs_reuse.sport_current_da)

// RX telemetry rate limiter (CPU relief), without changing UART framing
// or radio -> module requests. The AIR telemetry flow is controlled separately
// by the TX's TLM Ratio parameter. All limits are in 10ms ticks and are easy
// to disable by setting CRSF_RX_SENSOR_THROTTLE to 0.
#ifndef CRSF_RX_SENSOR_THROTTLE
#define CRSF_RX_SENSOR_THROTTLE 1
#endif
#define CRSF_RX_LINK_TICKS   10U  // link 0x14: max 10 samples/s
#define CRSF_RX_BATT_TICKS  100U  // legacy definition; Battery IDs must not share one throttle slot
#define CRSF_RX_GPS_TICKS   100U  // GPS 0x02: max 1 sample/s
#define CRSF_RX_OTHER_TICKS 100U  // temperature/RPM/time: max 1 sample/s/type

#if CRSF_RX_SENSOR_THROTTLE
// 8-bit tick snapshots are safe inside UART RX ISR. The modulo arithmetic
// handles timer wrap; each interval must stay below 256 ticks (2.56s).
#define crsf_rx_sensor_last (pulses2MHz.crsf_st.elrs_reuse.rx_sensor_last)
#define crsf_rx_sensor_seen (pulses2MHz.crsf_st.elrs_reuse.rx_sensor_seen)
static bool CRSF_RxAllowSensor(uint8_t type)
{
  uint8_t index, interval;
  switch (type)
  {
    case CRSF_TYPE_LINK:        index = 0; interval = CRSF_RX_LINK_TICKS; break;
    case CRSF_TYPE_BATTERY:     return true; // ID 0 and ID 1 are independent sources
    case CRSF_TYPE_GPS:         index = 2; interval = CRSF_RX_GPS_TICKS; break;
    case CRSF_TYPE_TEMPERATURE: index = 3; interval = CRSF_RX_OTHER_TICKS; break;
    case CRSF_TYPE_RPM:         index = 4; interval = CRSF_RX_OTHER_TICKS; break;
    case CRSF_TYPE_GPS_TIME:    index = 5; interval = CRSF_RX_OTHER_TICKS; break;
    default: return true; // CRSF settings 0x28..0x2D NEVER filtered
  }
  const uint8_t now = (uint8_t)getTmr10ms();
  const uint8_t mask = (uint8_t)(1U << index);
  if ((crsf_rx_sensor_seen & mask) &&
      (uint8_t)(now - crsf_rx_sensor_last[index]) < interval)
    return false;
  crsf_rx_sensor_last[index] = now;
  crsf_rx_sensor_seen |= mask;
  return true;
}
#endif

static void CRSF_SportMakeFrame(uint8_t *out, uint16_t appId, uint32_t value)
{
  out[0] = 0x1B;  // internal S.Port sensor physical ID
  out[1] = DATA_FRAME;
  out[2] = (uint8_t)appId;
  out[3] = (uint8_t)(appId >> 8);
  for (uint8_t i=4; i<8; ++i)
  {
    out[i] = (uint8_t)value;
    value >>= 8;
  }
  // S.Port checksum (0xFF after adding bytes [1..8] with carry).
  uint16_t sum = 0;
  for (uint8_t i=1; i<8; ++i)
  {
    sum += out[i];
    sum += sum >> 8;
    sum &= 0xFF;
  }
  out[8] = (uint8_t)(0xFF - sum);
}

// This function is called only after a valid CRSF CRC, from UART RX ISR.
// LoadFrskyTelemBuffer writes the small 9-byte packet into OpenAVRc's
// 3-slot queue. The main-loop telemetry task calls processSportPacket().
static void CRSF_SportQueue(uint16_t appId, uint32_t value)
{
  // OFF stops displaying CRSF sensors; RX stays enabled so the module menu
  // and normal channel transmission continue to work without restarting.
  if (CRSF_LOCAL_TELEM_OFF) return;
  uint8_t sport[FRSKY_TLM_PKT_SIZE];
  CRSF_SportMakeFrame(sport, appId, value);
  // A telemetry timer ISR can consume frsky_telem_buffers[] at any moment.
  // Lock only the tiny enqueue (9 bytes, max 3 slots) so it never observes
  // a half-written S.Port packet. UART ISR callers preserve disabled IRQs.
  const uint8_t savedSreg = SREG;
  cli();
  LoadFrskyTelemBuffer(sport);
  SREG = savedSreg;
}

// Collect sensor samples from ISR mailboxes, then emit at most ONE FrSky
// S.Port frame per 10ms tick.  If new samples arrive meanwhile, the latest
// value replaces the pending value, not a partially written packet.
static void CRSF_LinkBatteryMain()
{
  uint8_t newBattery = 0;
  uint8_t rxBattFromInternalVbat = 0;
  uint8_t batteryRawMillivolts = 0;
  const uint8_t saved = SREG;
  cli();
  if (crsf_link_pending)
  {
    crsf_sport_rx_lq = crsf_link_rx_lq;
    crsf_sport_tx_lq = crsf_link_tx_lq;
    crsf_link_pending = 0;
    crsf_sport_to_send |= (1U << 2) | (1U << 3);  // RSSI / SWR
  }
  if (crsf_battery_pending)
  {
    crsf_sport_battery_dv = crsf_battery_volts_dv;
    crsf_sport_current_da = crsf_battery_current_da;
    rxBattFromInternalVbat = (crsf_battery_pending & 0x02U) ? 1U : 0U;
    batteryRawMillivolts = (crsf_battery_pending & 0x04U) ? 1U : 0U;
    crsf_battery_pending = 0;

    // External CRSF battery sensor (oXs/FC): VFAS + Curr.
    // Internal ELRS AnalogVbat: RxBt only.
    if (!rxBattFromInternalVbat)
      crsf_sport_to_send |= (1U << 0) | (1U << 1);  // VFAS / Curr

    newBattery = 1;
  }
  SREG = saved;

  // 0x0E/128 provides millivolts whereas CRSF 0x08 already uses 0.1V.
  // Do this division outside the IRQ-disabled section.
  if (newBattery && batteryRawMillivolts)
    crsf_sport_battery_dv = (crsf_sport_battery_dv + 50U) / 100U;

  if (newBattery && crsf_sport_battery_dv != 0)
  {
    if (rxBattFromInternalVbat)
    {
      // ELRS receiver's own supply voltage.
      if (crsf_sport_battery_dv <= 255U)
        telemetryData.rxBatt = (uint8_t)crsf_sport_battery_dv;
    }
    else
    {
      // External battery sensor (oXs/FC).
      telemetryData.value.vfas = crsf_sport_battery_dv;
    }

    frskyStreaming = frskyStreaming ? FRSKY_TIMEOUT10ms : FRSKY_TIMEOUT_FIRST;
  }

  // One packet per 10ms tick, VFAS first. CRSF raw and local radio
  // battery values are NOT mixed. 0x0210 carries centivolts for S.Port.
  for (uint8_t k = 0; k < 4; ++k)
  {
    const uint8_t which = (uint8_t)((crsf_sport_next_sensor + k) % 4);
    const uint8_t bit = (uint8_t)(1U << which);
    if (!(crsf_sport_to_send & bit)) continue;
    crsf_sport_to_send &= (uint8_t)~bit;
    crsf_sport_next_sensor = (uint8_t)((which + 1) % 4);
    switch (which)
    {
      case 0: CRSF_SportQueue(0x0210, (uint32_t)crsf_sport_battery_dv * 10UL); break;
      case 1: CRSF_SportQueue(0x0200, crsf_sport_current_da); break;
      case 2: CRSF_SportQueue(X_RSSI_ID, crsf_sport_rx_lq); break;
      case 3: CRSF_SportQueue(X_SWR_ID, crsf_sport_tx_lq); break;
    }
    return;
  }
}

#if defined(GPS)
// CRSF GPS (0x02) -> five standard FrSky S.Port fields.
// Keep only a 15-byte mailbox in the UART callback. Convert/emit ONE field
// per >=20ms on subsequent valid CRSF frames to avoid filling OpenAVRc's
// three-frame S.Port queue and to preserve latitude/longitude before altitude.
// UART callback stores 15 bytes only. Coordinate conversion is deferred
// to perMain() (NEVER the UART ISR or the interrupt-driven 10ms handler).
// One 15-byte GPS snapshot. The ISR cannot overwrite it until all five
// FrSky S.Port fields have been emitted by perMain().
static uint8_t crsf_gps_active[15];
static volatile uint8_t crsf_gps_new = 0;
static uint8_t crsf_gps_next_field = 5;
// G: GPS S.Port sends one field at least 2 * 10ms apart. Modulo-256
// arithmetic works across the 255->0 wrap; no 16-bit clock needed here.
// The scanner uses a separate 16-bit clock for its 12-second timeout.
static uint8_t crsf_gps_last_tick;

static uint16_t CRSF_GpsReadU16(uint8_t offset)
{
  return ((uint16_t)crsf_gps_active[offset] << 8) | crsf_gps_active[offset+1];
}

static int32_t CRSF_GpsReadS32(uint8_t offset)
{
  uint32_t raw = ((uint32_t)crsf_gps_active[offset] << 24) |
                 ((uint32_t)crsf_gps_active[offset+1] << 16) |
                 ((uint32_t)crsf_gps_active[offset+2] << 8) |
                 (uint32_t)crsf_gps_active[offset+3];
  return (int32_t)raw;
}

// CRSF signed degrees * 1e7 -> S.Port minutes * 10000, with hemisphere
// encoded in bits 31:30 (0=N, 1=S, 2=E, 3=W).
static uint32_t CRSF_GpsToSportCoord(int32_t coord, uint8_t longitude)
{
  uint32_t degrees1e7 = coord < 0 ? (uint32_t)(0UL - (uint32_t)coord) : (uint32_t)coord;
  const uint32_t degrees = degrees1e7 / 10000000UL;
  const uint32_t fractional = degrees1e7 % 10000000UL;
  uint32_t minutes10000 = degrees * 600000UL + (fractional * 3UL + 25UL) / 50UL;
  const uint8_t hemisphere = longitude ? (coord < 0 ? 3 : 2) : (coord < 0 ? 1 : 0);
  return minutes10000 | ((uint32_t)hemisphere << 30);
}

// Called only on a COMPLETE, CRC-valid incoming CRSF frame.
// Data are first sent as coordinates, then altitude (which triggers
// OpenAVRc's manageGpsFix), speed and course.
static void CRSF_GpsSendOne(void)
{
  if (crsf_gps_next_field >= 5) return;
  const uint16_t now = getTmr10ms();
  if ((uint8_t)((uint8_t)now - crsf_gps_last_tick) < 2U) return;
  crsf_gps_last_tick = (uint8_t)now;
  switch (crsf_gps_next_field++)
  {
    case 0: // latitude
      CRSF_SportQueue(0x0800, CRSF_GpsToSportCoord(CRSF_GpsReadS32(0), 0));
      break;
    case 1: // longitude
      CRSF_SportQueue(0x0800, CRSF_GpsToSportCoord(CRSF_GpsReadS32(4), 1));
      break;
    case 2: // CRSF altitude in meters with +1000m offset; S.Port in cm
      CRSF_SportQueue(0x0820, (uint32_t)((int32_t)CRSF_GpsReadU16(12) - 1000L) * 100L);
      break;
    case 3: // CRSF 0.1 km/h -> S.Port speed as 0.001 knots.
      // OpenAVRc's FrSky decoder divides by 100 to obtain 0.1 knots,
      // then its metric display converts 0.1 knots back to 0.1 km/h.
      // 1 knot = 1.852 km/h. Round to 0.1 knots before encoding.
      // Multiply by 27/50 (= 0.54), with 32-bit intermediates on AVR.
      {
        const uint32_t speed_tenths_knots =
            ((uint32_t)CRSF_GpsReadU16(8) * 27UL + 25UL) / 50UL;
        CRSF_SportQueue(0x0830, speed_tenths_knots * 100UL);
      }
      break;
    case 4: // course 0.01 degree, OpenAVRc divides by 100
      CRSF_SportQueue(0x0840, CRSF_GpsReadU16(10));
      break;
  }
}
#endif // GPS
#endif // FRSKY

// One bounded mailbox is drained by perMain(), not the UART ISR or
// the nested 10ms timer ISR. Sensor payload layouts follow oXs/CRSF (big endian).
#if defined(FRSKY)
static void CRSF_ExtraTelemetryTick()
{
  if (!crsf_extra_pending) return;
  uint8_t type, n, payload[9];
  // Atomic copy: the UART ISR may run at any byte boundary. Keep the
  // interrupts-disabled section short (max 9 payload bytes).
  const uint8_t savedSreg = SREG;
  cli();
  type = crsf_extra_type;
  n = crsf_extra_size;
  if (n > sizeof(payload)) n = sizeof(payload);
  for (uint8_t i=0; i<n; ++i) payload[i] = crsf_extra_payload[i];
  crsf_extra_pending = 0;
  SREG = savedSreg;

  if (type == CRSF_TYPE_BATTERY && n == 9 && payload[8] != 0U)
  {
    // Standard CRSF Battery Sensor with optional ID. Any non-zero ID is an
    // external battery source; oXs uses ID 1. Units already match OpenAVRc:
    // voltage/current are 0.1 V / 0.1 A and capacity is mAh.
    const uint16_t voltage_dv = ((uint16_t)payload[0] << 8) | payload[1];
    const uint16_t current_da = ((uint16_t)payload[2] << 8) | payload[3];
    const uint32_t capacity_mah = ((uint32_t)payload[4] << 16) |
                                  ((uint32_t)payload[5] << 8) | payload[6];

    if (voltage_dv != 0U)
      processHubPacket(VFAS_ID, voltage_dv);
    processHubPacket(CURRENT_ID, current_da);
    processHubPacket(OXS_MAH_ID,
                     capacity_mah > 65535UL ? 65535U : (uint16_t)capacity_mah);
    frskyStreaming = frskyStreaming ? FRSKY_TIMEOUT10ms : FRSKY_TIMEOUT_FIRST;
  }
  else if (type == CRSF_TYPE_VOLTAGES && n == 7 && payload[0] < 128U)
  {
    // CRSF 0x0E Cells. The UART ISR already compressed the first six cell
    // voltages from millivolts to OpenAVRc's native 0.02 V units so this
    // remains cheap in main context. CRSF/EdgeTX may carry 8 cells; this
    // transmitter intentionally keeps only C1..C6 because NUMCELLS == 6.
    uint8_t minCell = 255U;
    uint8_t minIdx = 0U;
    telemetryData.value.cellsCount = 6U;
    for (uint8_t i=0; i<6U; ++i)
    {
      const uint8_t cell = payload[1+i];
      telemetryData.value.cellVolts[i] = cell;
      if (cell < minCell)
      {
        minCell = cell;
        minIdx = i;
      }
    }
    telemetryData.value.minCellVolts = minCell;
    telemetryData.value.minCellIdx = minIdx;
    frskyStreaming = frskyStreaming ? FRSKY_TIMEOUT10ms : FRSKY_TIMEOUT_FIRST;
  }
  else if (type == CRSF_TYPE_VOLTAGES && n >= 3 && payload[0] == 128)
  {
    // ExpressLRS AnalogVbat voltage group: source_id 128, first value in mV.
    // This remains available even when an external 0x08 battery sensor is used.
    const uint16_t mv = ((uint16_t)payload[1] << 8) | payload[2];
    if (mv != 0U && mv != 0xFFFFU)
    {
      const uint16_t dv = (mv + 50U) / 100U;
      if (dv <= 255U)
      {
        telemetryData.rxBatt = (uint8_t)dv;
        frskyStreaming = frskyStreaming ? FRSKY_TIMEOUT10ms : FRSKY_TIMEOUT_FIRST;
      }
    }
  }
  else if (type == CRSF_TYPE_TEMPERATURE && (n == 3 || n == 5))
  {
    // source_id=0 : T1, optionally T2. source_id=1 : T2 alone.
    int16_t a = (int16_t)(((uint16_t)payload[1] << 8) | payload[2]);
    int16_t t1 = (a >= 0) ? (a + 5)/10 : (a - 5)/10;
    if (payload[0] == 0)
    {
      CRSF_SportQueue(0x0400, (uint32_t)(int32_t)t1);
      if (n == 5)
      {
        int16_t b = (int16_t)(((uint16_t)payload[3] << 8) | payload[4]);
        int16_t t2 = (b >= 0) ? (b + 5)/10 : (b - 5)/10;
        CRSF_SportQueue(0x0410, (uint32_t)(int32_t)t2);
      }
    }
    else if (payload[0] == 1)
      CRSF_SportQueue(0x0410, (uint32_t)(int32_t)t1);
  }
  else if (type == CRSF_TYPE_RPM && n == 4 && payload[0] == 0)
  {
    // Three-byte signed RPM. Negative RPM is clamped to zero; OpenAVRc
    // expects S.Port 0x0500 scaled by the configured number of blades+2.
    int32_t rpm = ((int32_t)payload[1] << 16) |
                  ((int32_t)payload[2] << 8) | (int32_t)payload[3];
    if (rpm & 0x800000L) rpm |= (int32_t)0xFF000000UL;
    if (rpm < 0) rpm = 0;
    CRSF_SportQueue(0x0500, (uint32_t)rpm *
                    (uint32_t)(g_model.telemetry.blades + 2));
  }
#if defined(GPS)
  else if (type == CRSF_TYPE_GPS_TIME && n == 9)
  {
    // UTC date/time from CRSF 0x03, not the transmitter's clock.
    // Date: [YY MM DD FF]; Time: [HH MM SS 00] in S.Port 0x0850.
    const uint16_t year = ((uint16_t)payload[0] << 8) | payload[1];
    const uint8_t mon=payload[2], day=payload[3], hour=payload[4];
    const uint8_t minute=payload[5], sec=payload[6];
    if (year>=2000 && year<=2099 && mon>=1 && mon<=12 && day>=1 && day<=31 &&
        hour<24 && minute<60 && sec<60)
    {
      CRSF_SportQueue(0x0850, ((uint32_t)(year-2000) << 24) |
                      ((uint32_t)mon << 16) | ((uint32_t)day << 8) | 0xFFUL);
      CRSF_SportQueue(0x0850, ((uint32_t)hour << 24) |
                      ((uint32_t)minute << 16) | ((uint32_t)sec << 8));
    }
  }
#endif
}
#endif // FRSKY

// Historical compatibility: some locally modified frsky.cpp versions call
// this function from telemetryInterrupt10ms(). That routine is itself called
// by TIMER_10MS_VECT with sei() enabled: it is NOT a normal main-loop task!
// IMPORTANT: do not decode GPS or call LoadFrskyTelemBuffer from that ISR.
// These operations are deliberately done by CRSF_SportTelemetryMain below.
void CRSF_SportTelemetryTick()
{
  // Intentionally empty. No long computations in the nested 10ms ISR.
}

// Called from perMain() in main_avr.cpp, never from the UART or 10ms ISRs.
// Do work at most once per 10ms tick. This also handles frsky.cpp versions
// without the optional CRSF_SportTelemetryTick() hook, fixing lost GPS data.
void CRSF_SportTelemetryMain()
{
  static uint16_t lastTick = 0xFFFF;
  const uint16_t now = getTmr10ms();
  if (lastTick == now) return;
  lastTick = now;

  // ELRS parameter parsing must run regardless of the local telemetry switch.
  // The menu still needs DEVICE_INFO/parameter responses while telemetry is OFF.
  CRSF_PowerMain();
#if defined(FRSKY)
  // Clear any queued sensor payload on OFF. Stale FrSky display values will
  // expire normally; never reset CRSF or disturb RF packet timing here.
  if (CRSF_LOCAL_TELEM_OFF)
  {
    crsf_extra_pending = 0;
    crsf_link_pending = 0;
    crsf_battery_pending = 0;
    crsf_sport_to_send = 0;
#if defined(GPS)
    crsf_gps_new = 0;
    crsf_gps_next_field = 5;
#endif
    return;
  }
  // Run conversion and S.Port queue writes only in normal main context.
  CRSF_LinkBatteryMain();
  CRSF_ExtraTelemetryTick();
#endif
#if defined(FRSKY) && defined(GPS)
  // Atomic GPS snapshot handover (no second 15-byte copy).
  // Keep RX blocked until all five S.Port fields have been emitted.
  if (crsf_gps_new && crsf_gps_next_field >= 5)
  {
    const uint8_t savedSreg = SREG;
    cli();
    crsf_gps_next_field = 0; // Lock ISR writer BEFORE clearing pending.
    crsf_gps_new = 0;
    SREG = savedSreg;
  }
  // At most one S.Port GPS field every 20ms; keeps buffer use bounded.
  CRSF_GpsSendOne();
#endif
}

// Compact ELRS options cache: the indices are the actual CRSF enumeration indices.
// Blank or >250Hz options are not selectable at our tested 400K UART speed.
#define CRSF_MENU_MAX_RATES 16
// Each CRSF frame has <= 56 bytes of parameter data after its 0x2B header.
// Stream fragments through one 56-byte mailbox, not a 256-byte assembly array.
#define CRSF_PWR_FRAGMENT_MAX 56U
// Each ELRS Packet Rate label follows [D|F|K|DK] + number + [Hz] + [ Full].
// Preserve the FULL index ordering while storing only a 16-bit packed label
// instead of 11 bytes for EVERY candidate (16*2 vs 16*11 bytes).
// Format: 0..10 frequency, 11..13 prefix, 14 Hz suffix, 15 Full suffix.
static uint16_t crsf_menu_rate_codes[CRSF_MENU_MAX_RATES];
#define crsf_menu_rate_text (pulses2MHz.crsf_st.elrs_reuse.rate_text) // 11 bytes reused

static uint16_t CRSF_RatePackLabel(const char *label)
{
  uint8_t i=0, prefix=0, hzSuffix=0, fullSuffix=0;
  if (label[0]=='D' && label[1]=='K') { prefix=4; i=2; }
  else if (label[0]=='D') { prefix=1; i=1; }
  else if (label[0]=='F') { prefix=2; i=1; }
  else if (label[0]=='K') { prefix=3; i=1; }
  uint16_t hz=0;
  if (label[i]<'0' || label[i]>'9') return 0;
  while (label[i]>='0' && label[i]<='9')
  {
    hz=(uint16_t)(hz*10U+(uint8_t)(label[i]-'0'));
    if (hz>2047U) return 0;
    ++i;
  }
  if (label[i]=='H' && label[i+1]=='z') { hzSuffix=1; i+=2; }
  if (label[i]==' ' && label[i+1]=='F' && label[i+2]=='u' &&
      label[i+3]=='l' && label[i+4]=='l') { fullSuffix=1; i+=5; }
  if (label[i] || !hz) return 0; // Unrecognized variant, never emit malformed text.
  return (uint16_t)(hz | ((uint16_t)prefix<<11) |
                 (hzSuffix?0x4000U:0U) | (fullSuffix?0x8000U:0U));
}

// Called synchronously by the model menu; never from UART/RF ISR.
static const char *CRSF_RateLabel(uint8_t index)
{
  char *p=crsf_menu_rate_text;
  if (index>=CRSF_MENU_MAX_RATES) { *p=0; return p; }
  const uint16_t code=crsf_menu_rate_codes[index];
  uint16_t hz=code & 0x07FFU;
  uint8_t prefix=(uint8_t)((code>>11)&7U);
  if (!hz) { *p=0; return p; }
  if (prefix==1 || prefix==4) *p++='D';
  if (prefix==2) *p++='F';
  if (prefix==3 || prefix==4) *p++='K';
  char decimal[4]; uint8_t len=0;
  do { decimal[len++]=(char)('0'+(hz%10U)); hz/=10U; }
  while (hz && len<sizeof(decimal));
  while (len) *p++=decimal[--len];
  if (code&0x4000U) { *p++='H'; *p++='z'; }
  if (code&0x8000U) { *p++=' '; *p++='F'; *p++='u'; *p++='l'; *p++='l'; }
  *p=0;
  return crsf_menu_rate_text;
}
// No 16-entry SRAM array: the period is calculated from the packed rate
// ONLY when a menu selection is read or edited (never in the UART ISR).
static uint16_t CRSF_RatePeriod(uint8_t index)
{
  if (index>=CRSF_MENU_MAX_RATES) return 0;
  const uint16_t hz=(uint16_t)(crsf_menu_rate_codes[index] & 0x07FFU);
  if (hz<25U || hz>250U || (!CRSF_MENU_FAST_BAUD && hz>100U)) return 0;
  return (uint16_t)((1000000UL+hz/2U)/hz);
}
#define crsf_menu_rate_count (pulses2MHz.crsf_st.elrs_reuse.rate_count)
#define crsf_menu_rate_selected (pulses2MHz.crsf_st.elrs_reuse.rate_selected)
#define crsf_menu_rate_field (pulses2MHz.crsf_st.elrs_reuse.rate_field)

// Switch Mode does not consume extra bytes in elrs_reuse.
// boot_pending and refresh_requested are boolean values and only use bit 0;
// their upper seven bits hold a 13-bit Switch Mode snapshot:
//   bits 0..7  = CRSF parameter field id
//   bits 8..9  = selected enum index (0..2)
//   bits 10..12 = selectable option mask (bits 0..2)
// The low boolean bits remain independent. requested_tlm/rate/power are kept.
static uint16_t CRSF_SwitchStateRead(void)
{
  const uint16_t lo=(uint16_t)(pulses2MHz.crsf_st.elrs_reuse.boot_pending>>1);
  const uint16_t hi=(uint16_t)(pulses2MHz.crsf_st.elrs_reuse.refresh_requested>>1);
  return (uint16_t)(lo | (hi<<7));
}

static void CRSF_SwitchStateWrite(uint8_t field, uint8_t selected, uint8_t mask)
{
  const uint16_t packed=(uint16_t)field |
                        ((uint16_t)(selected&0x03U)<<8) |
                        ((uint16_t)(mask&0x07U)<<10);
  const uint8_t saved=SREG;
  cli();
  const uint8_t boot=(uint8_t)(pulses2MHz.crsf_st.elrs_reuse.boot_pending&0x01U);
  const uint8_t refresh=(uint8_t)(pulses2MHz.crsf_st.elrs_reuse.refresh_requested&0x01U);
  pulses2MHz.crsf_st.elrs_reuse.boot_pending=(uint8_t)(boot | ((packed&0x007FU)<<1));
  pulses2MHz.crsf_st.elrs_reuse.refresh_requested=(uint8_t)(refresh | (((packed>>7)&0x007FU)<<1));
  SREG=saved;
}

static uint8_t CRSF_SwitchField(void)
{
  return (uint8_t)(CRSF_SwitchStateRead()&0x00FFU);
}

static uint8_t CRSF_SwitchSelected(void)
{
  return (uint8_t)((CRSF_SwitchStateRead()>>8)&0x03U);
}

static uint8_t CRSF_SwitchMask(void)
{
  return (uint8_t)((CRSF_SwitchStateRead()>>10)&0x07U);
}

#define crsf_menu_switch_field (CRSF_SwitchField())
#define crsf_menu_switch_selected (CRSF_SwitchSelected())
#define crsf_menu_switch_mask (CRSF_SwitchMask())

#define CRSF_PENDING_RATE   0x01U
#define CRSF_PENDING_SWITCH 0x02U
#define CRSF_PENDING_MODEL  0x04U
#define CRSF_PENDING_BIND   0x08U

// Upper nibble of pending_rate is free while the four low bits above are
// pending flags. During Range test it stores (normal RF power selection + 1).
// This keeps the complete NumRx/Bind/Range feature inside the existing
// 99-byte p2m reuse area: no new persistent SRAM is allocated.
#define CRSF_RANGE_SAVED_MASK 0xF0U

// requested_rate needs only its low nibble (CRSF_MENU_MAX_RATES == 16).
// Bits 4..5 retain a pending Switch Mode enum without removing the original
// requested_rate byte or changing the 99-byte p2m layout.
static uint8_t CRSF_RequestedRate(void)
{
  return (uint8_t)(pulses2MHz.crsf_st.elrs_reuse.requested_rate&0x0FU);
}

static void CRSF_SetRequestedRate(uint8_t value)
{
  pulses2MHz.crsf_st.elrs_reuse.requested_rate=
    (uint8_t)((pulses2MHz.crsf_st.elrs_reuse.requested_rate&0xF0U) | (value&0x0FU));
}

static uint8_t CRSF_RequestedSwitch(void)
{
  return (uint8_t)((pulses2MHz.crsf_st.elrs_reuse.requested_rate>>4)&0x03U);
}

static void CRSF_SetRequestedSwitch(uint8_t value)
{
  pulses2MHz.crsf_st.elrs_reuse.requested_rate=
    (uint8_t)((pulses2MHz.crsf_st.elrs_reuse.requested_rate&0xCFU) | ((value&0x03U)<<4));
}

static uint8_t CRSF_RangeActive(void)
{
  return (pulses2MHz.crsf_st.elrs_reuse.pending_rate & CRSF_RANGE_SAVED_MASK) ? 1U : 0U;
}

static uint8_t CRSF_RangeSavedPower(void)
{
  const uint8_t packed=(uint8_t)((pulses2MHz.crsf_st.elrs_reuse.pending_rate & CRSF_RANGE_SAVED_MASK)>>4);
  return packed ? (uint8_t)(packed-1U) : 0xFFU;
}

static uint8_t CRSF_SwitchFullRate(void)
{
  if (crsf_menu_rate_selected>=CRSF_MENU_MAX_RATES) return 0;
  return (crsf_menu_rate_codes[crsf_menu_rate_selected]&0x8000U) ? 1U : 0U;
}

// ExpressLRS Switch Mode labels, kept in flash.
static const pm_char *CRSF_SwitchLabel(uint8_t index)
{
  if (CRSF_SwitchFullRate())
  {
    if (index==0) return PSTR("8ch");
    if (index==1) return PSTR("16ch Rate/2");
    if (index==2) return PSTR("12ch Mixed");
  }
  else
  {
    if (index==0) return PSTR("Wide");
    if (index==1) return PSTR("Hybrid");
  }
  return PSTR("---");
}

static uint8_t CRSF_SwitchSelectable(uint8_t index)
{
  return (index<3U && (crsf_menu_switch_mask&(uint8_t)(1U<<index))) ? 1U : 0U;
}

static uint8_t CRSF_MenuNextSwitch(uint8_t current, int8_t direction)
{
  const uint8_t count=CRSF_SwitchFullRate()?3U:2U;
  for (uint8_t i=0;i<count;++i)
  {
    current=(direction>0) ? (uint8_t)((current+1U)%count) :
                            (uint8_t)((current+count-1U)%count);
    if (CRSF_SwitchSelectable(current)) return current;
  }
  return crsf_menu_switch_selected;
}
// ISR owns the mailbox only until CRSF_PWR_FRAGMENT is published.
// perMain() parses it and requests the next chunk after consumption.
static volatile uint8_t crsf_pwr_fragment[CRSF_PWR_FRAGMENT_MAX];
#define crsf_pwr_fragment_len (pulses2MHz.crsf_st.elrs_reuse.fragment_len)
#define crsf_pwr_fragment_more (pulses2MHz.crsf_st.elrs_reuse.fragment_more)

// Small state machine for CRSF_TEXT_SELECTION and version/domain CRSF_INFO.
// All parsing executes in perMain(), never inside UART/RF interrupts.
#define crsf_pwr_parse_stage (pulses2MHz.crsf_st.elrs_reuse.parse_stage)
#define crsf_pwr_parse_type (pulses2MHz.crsf_st.elrs_reuse.parse_type)
#define crsf_pwr_parse_kind (pulses2MHz.crsf_st.elrs_reuse.parse_kind)
#define crsf_pwr_parse_name (pulses2MHz.crsf_st.elrs_reuse.parse_scratch.parse_name)
#define crsf_pwr_parse_name_len (pulses2MHz.crsf_st.elrs_reuse.parse_name_len)
#define crsf_pwr_parse_opt (pulses2MHz.crsf_st.elrs_reuse.parse_scratch.parse_opt)
#define crsf_pwr_parse_opt_len (pulses2MHz.crsf_st.elrs_reuse.parse_opt_len)
#define crsf_pwr_parse_opt_skip (pulses2MHz.crsf_st.elrs_reuse.parse_opt_skip)
#define crsf_pwr_parse_opt_index (pulses2MHz.crsf_st.elrs_reuse.parse_opt_index)
#define crsf_pwr_parse_recognized (pulses2MHz.crsf_st.elrs_reuse.parse_recognized)
#define crsf_pwr_parse_off (pulses2MHz.crsf_st.elrs_reuse.parse_off)
#define crsf_pwr_parse_dyn (pulses2MHz.crsf_st.elrs_reuse.parse_dyn)
#define crsf_pwr_parse_win (pulses2MHz.crsf_st.elrs_reuse.parse_scratch.parse_win)
#define crsf_pwr_parse_band_sub (pulses2MHz.crsf_st.elrs_reuse.parse_band_sub)
#define crsf_pwr_parse_band_24 (pulses2MHz.crsf_st.elrs_reuse.parse_band_24)

// Lightweight, independent ELRS module discovery at startup.
// The historical menu-driven scanner has been removed.
// Reuse a single 56-byte mailbox for successive 0x2B chunks;
// no second buffer is allocated. The ISR only copies bytes.
// IMPORTANT: these indices are PARAMETER ENUM choices and not power in mW.
enum {
  CRSF_PWR_IDLE, CRSF_PWR_PING_TX, CRSF_PWR_PING_RX,
  CRSF_PWR_READ_TX, CRSF_PWR_READ_RX, CRSF_PWR_FRAGMENT, CRSF_PWR_DECODE,
  CRSF_PWR_READY, CRSF_PWR_FAILED, CRSF_PWR_REFRESH_DELAY
};
#define CRSF_PWR_MAX_FIELDS 48
#define CRSF_PWR_WAIT_TICKS 40U  // 400 ms per attempt
#define crsf_pwr_state (pulses2MHz.crsf_st.elrs_reuse.pwr_state)
// Display-only status: 0 INIT,1 PING,2 TX-OK,3 READ,4 OK,
// 5 NO-ACK,6 NO-PAR,7 CRCERR,8 ADDR?,9 WAIT.
// Written as one byte, so it is safe to read from the LCD on ATmega2560.
#define crsf_pwr_diag (pulses2MHz.crsf_st.elrs_reuse.pwr_diag)
// Only updated from a validated, TX-addressed CRSF_INFO parameter reply.
#define crsf_pwr_band (pulses2MHz.crsf_st.elrs_reuse.pwr_band)
// DEVICE_INFO provides the actual last field ID, possibly > 48.
#define crsf_pwr_last_field (pulses2MHz.crsf_st.elrs_reuse.last_field)
#define crsf_pwr_last_queried (pulses2MHz.crsf_st.elrs_reuse.last_queried)
#define crsf_pwr_info_seen (pulses2MHz.crsf_st.elrs_reuse.info_seen)
#define crsf_pwr_params_seen (pulses2MHz.crsf_st.elrs_reuse.params_seen)
#define crsf_pwr_crc_err (pulses2MHz.crsf_st.elrs_reuse.crc_err)
#define crsf_pwr_wrong_addr (pulses2MHz.crsf_st.elrs_reuse.wrong_addr)
// Boot-time module discovery: no dependency on the model menu.
// The 16-bit timestamps are ONLY accessed from main context, not the RX ISR.
#define crsf_pwr_init_tick (pulses2MHz.crsf_st.elrs_reuse.phase_tick) // G shared phase time
#define crsf_pwr_scan_tick (pulses2MHz.crsf_st.elrs_reuse.phase_tick) // G shared phase time
#define crsf_pwr_refresh_tick (pulses2MHz.crsf_st.elrs_reuse.phase_tick) // G shared phase time
#define crsf_pwr_boot_pending (pulses2MHz.crsf_st.elrs_reuse.boot_pending)
#define crsf_pwr_last_tx_tick (pulses2MHz.crsf_st.elrs_reuse.last_tx_tick)
#define crsf_pwr_refresh_requested (pulses2MHz.crsf_st.elrs_reuse.refresh_requested)
#define CRSF_PWR_START_DELAY_TICKS 120U  // 1.2 s: leave startup/mixer/UART alone
#define CRSF_PWR_SCAN_LIMIT_TICKS 1200U  // 12 s bounded overall discovery
#define CRSF_PWR_REFRESH_DELAY_TICKS 20U // 200 ms after module write
#define CRSF_PWR_COMMAND_GAP_TICKS 5U    // at most one request per 50 ms

#define crsf_pwr_field (pulses2MHz.crsf_st.elrs_reuse.field)
#define crsf_pwr_fields_total (pulses2MHz.crsf_st.elrs_reuse.fields_total)
#define crsf_pwr_chunk (pulses2MHz.crsf_st.elrs_reuse.chunk)
#define crsf_pwr_retries (pulses2MHz.crsf_st.elrs_reuse.retries)
#define crsf_pwr_sent_tick (pulses2MHz.crsf_st.elrs_reuse.sent_tick)
#define crsf_pwr_max_id (pulses2MHz.crsf_st.elrs_reuse.max_id)
#define crsf_pwr_dyn_id (pulses2MHz.crsf_st.elrs_reuse.dyn_id)
#define crsf_pwr_dyn_off (pulses2MHz.crsf_st.elrs_reuse.dyn_off)
#define crsf_pwr_dyn_on (pulses2MHz.crsf_st.elrs_reuse.dyn_on)
#define crsf_pwr_enum (pulses2MHz.crsf_st.elrs_reuse.enum_levels)
#define crsf_pwr_pending (pulses2MHz.crsf_st.elrs_reuse.pending)
#define crsf_pwr_write_step (pulses2MHz.crsf_st.elrs_reuse.write_step)
#define crsf_pwr_power_raw (pulses2MHz.crsf_st.elrs_reuse.power_raw)
#define crsf_pwr_dynamic_raw (pulses2MHz.crsf_st.elrs_reuse.dynamic_raw)
#define crsf_pwr_display_power (pulses2MHz.crsf_st.elrs_reuse.display_power)
#define crsf_pwr_tlm_id (pulses2MHz.crsf_st.elrs_reuse.tlm_id)
#define crsf_pwr_tlm_display (pulses2MHz.crsf_st.elrs_reuse.tlm_display)
#define crsf_pwr_pending_rate (pulses2MHz.crsf_st.elrs_reuse.pending_rate)
#define crsf_pwr_pending_tlm (pulses2MHz.crsf_st.elrs_reuse.pending_tlm)
#define crsf_pwr_requested_tlm (pulses2MHz.crsf_st.elrs_reuse.requested_tlm)
#define crsf_pwr_requested_rate (pulses2MHz.crsf_st.elrs_reuse.requested_rate)
#define crsf_pwr_requested_power (pulses2MHz.crsf_st.elrs_reuse.requested_power)
static void CRSF_MenuApplyPeriod(uint16_t us);


// Match domain tokens in a bounded, NUL-terminated CRSF_INFO field name.
// This describes the RF band(s), not the instantaneous FHSS frequency.
static uint8_t CRSF_PwrContains(const uint8_t *data, uint8_t len,
                                 const char *target, uint8_t targetLen)
{
  if (targetLen > len) return 0;
  for (uint8_t j=0; j<=(uint8_t)(len-targetLen); ++j)
    if (!memcmp_P(data+j,target,targetLen)) return 1;
  return 0;
}

static uint8_t CRSF_PwrBandFromInfo(const uint8_t *data, uint8_t len)
{
  const uint8_t band24=CRSF_PwrContains(data,len,PSTR("ISM2G4"),6) ||
                       CRSF_PwrContains(data,len,PSTR("CE_LBT"),6);
  uint8_t sub=0;
  if (CRSF_PwrContains(data,len,PSTR("EU868"),5)) sub=1;
  else if (CRSF_PwrContains(data,len,PSTR("FCC915"),6) ||
           CRSF_PwrContains(data,len,PSTR("AU915"),5)) sub=2;
  else if (CRSF_PwrContains(data,len,PSTR("IN866"),5)) sub=5;
  else if (CRSF_PwrContains(data,len,PSTR("EU433"),5) ||
           CRSF_PwrContains(data,len,PSTR("AU433"),5) ||
           CRSF_PwrContains(data,len,PSTR("US433"),5) ||
           CRSF_PwrContains(data,len,PSTR("US433W"),6)) sub=4;
  if (!band24) return sub;
  if (!sub) return 3;
  if (sub==1) return 6;
  if (sub==2) return 7;
  if (sub==4) return 8;
  return 9;
}

// Parser helpers work on one bounded parameter snapshot in the UI context.
static uint16_t CRSF_PwrParseNumber(const uint8_t *p, uint16_t len)
{
  uint16_t value=0;
  uint8_t found=0;
  // Refuse multi-band '25/100' choices: a single power cannot represent both.
  for (uint16_t i=0; i<len; ++i)
  {
    if (p[i]=='/') return 0;
    if (p[i]>='0' && p[i]<='9')
    {
      if (value>2000U) return 0;
      value=(uint16_t)(value*10U + p[i]-'0');
      found=1;
    }
    else if (found) break;
    else if (p[i]!=' ') return 0;
  }
  return found ? value : 0;
}

static uint8_t CRSF_PwrSlot(uint16_t mw)
{
  switch (mw)
  {
    case 10: return 0;
    case 25: return 1;
    case 50: return 2;
    case 100: return 3;
    case 250: return 4;
    case 500: return 5;
    case 1000: return 6;
    case 2000: return 7;
    default: return 0xFF;
  }
}

static uint8_t CRSF_PwrTlmUi(uint8_t raw)
{
  if (raw==0) return 8;    // Std
  if (raw==1) return 0;    // Off
  if (raw>=2 && raw<=8) return raw-1;
  if (raw==9) return 9;    // Race
  return 0xFF;
}

static uint8_t CRSF_PwrTlmRaw(uint8_t ui)
{
  if (ui==8) return 0;
  if (ui==0) return 1;
  if (ui==9) return 9;
  return (ui<=7) ? ui+1 : 0xFF;
}

static uint8_t CRSF_PwrSelectable(uint8_t selection)
{
  if (selection==1) return crsf_pwr_dyn_id && crsf_pwr_dyn_on!=0xFF;
  if (selection>=2 && selection<=9)
    return crsf_pwr_max_id && crsf_pwr_enum[selection-2]!=0xFF;
  return 0;
}

static uint8_t CRSF_PwrNextSelection(uint8_t current, int8_t direction)
{
  for (uint8_t tries=0;tries<10;++tries)
  {
    current = (direction>0) ? (uint8_t)((current+1)%10) :
                                (uint8_t)((current+9)%10);
    if (CRSF_PwrSelectable(current)) return current;
  }
  return 0;
}

// The TX field stream begins [parent, type, field-name, NUL, options, NUL,
// selected-value, ...]. The name/options/selected-value can span 0x2B chunks.
static void CRSF_PwrParserReset()
{
  crsf_pwr_parse_stage=0;
  crsf_pwr_parse_type=0;
  crsf_pwr_parse_kind=0;
  crsf_pwr_parse_name_len=0;
  crsf_pwr_parse_name[0]=0;
  crsf_pwr_parse_opt_len=0;
  crsf_pwr_parse_opt_skip=0;
  crsf_pwr_parse_opt_index=0;
  crsf_pwr_parse_recognized=0;
  crsf_pwr_parse_off=0xFF;
  crsf_pwr_parse_dyn=0xFF;
  for (uint8_t j=0;j<6;++j) crsf_pwr_parse_win[j]=0;
  crsf_pwr_parse_band_sub=0;
  crsf_pwr_parse_band_24=0;
}

static void CRSF_PwrParserNameDone()
{
  const uint8_t n=crsf_pwr_parse_name_len;
  const char *p=crsf_pwr_parse_name;
  if (crsf_pwr_parse_type==12U)
  {
    crsf_pwr_parse_kind=5; // CRSF_INFO, version/domain already scanned
    uint8_t sub=crsf_pwr_parse_band_sub;
    const uint8_t b24=crsf_pwr_parse_band_24;
    if (b24)
    {
      if (sub==1) sub=6;
      else if (sub==2) sub=7;
      else if (sub==4) sub=8;
      else if (sub==5) sub=9;
      else sub=3;
    }
    if (sub) crsf_pwr_band=sub;
  }
  else if (crsf_pwr_parse_type==9U)
  {
    if (n==9 && memcmp_P(p,PSTR("Max Power"),9)==0) crsf_pwr_parse_kind=1;
    else if (n==7 && memcmp_P(p,PSTR("Dynamic"),7)==0) crsf_pwr_parse_kind=2;
    else if ((n==11 && memcmp_P(p,PSTR("Telem Ratio"),11)==0) ||
             (n==9 && memcmp_P(p,PSTR("TLM Ratio"),9)==0)) crsf_pwr_parse_kind=3;
    else if (n==11 && memcmp_P(p,PSTR("Packet Rate"),11)==0) crsf_pwr_parse_kind=4;
    else if (n==11 && memcmp_P(p,PSTR("Switch Mode"),11)==0) crsf_pwr_parse_kind=6;
  }
}

static void CRSF_PwrParserOptionDone()
{
  const uint8_t raw=crsf_pwr_parse_opt_index;
  crsf_pwr_parse_opt[crsf_pwr_parse_opt_len]=0;
  if (crsf_pwr_parse_kind==1 && raw<32)
  {
    const uint8_t slot=CRSF_PwrSlot(CRSF_PwrParseNumber(
      (const uint8_t *)crsf_pwr_parse_opt,crsf_pwr_parse_opt_len));
    if (slot<8) { crsf_pwr_enum[slot]=raw; ++crsf_pwr_parse_recognized; }
  }
  else if (crsf_pwr_parse_kind==2 && raw<16)
  {
    if (crsf_pwr_parse_opt_len==3 && !memcmp_P(crsf_pwr_parse_opt,PSTR("Off"),3))
      crsf_pwr_parse_off=raw;
    if (crsf_pwr_parse_opt_len==3 && !memcmp_P(crsf_pwr_parse_opt,PSTR("Dyn"),3))
      crsf_pwr_parse_dyn=raw;
  }
  else if (crsf_pwr_parse_kind==4 && raw<CRSF_MENU_MAX_RATES)
  {
    crsf_menu_rate_codes[raw]=CRSF_RatePackLabel(crsf_pwr_parse_opt);
  }
  else if (crsf_pwr_parse_kind==6 && raw<3U && crsf_pwr_parse_opt_len)
  {
    // Empty entries are disabled choices (e.g. MAVLink-specific lists).
    crsf_pwr_parse_recognized|=(uint8_t)(1U<<raw);
  }
  crsf_pwr_parse_opt_len=0;
  crsf_pwr_parse_opt_skip=0;
  if (crsf_pwr_parse_opt_index<255) ++crsf_pwr_parse_opt_index;
}

static void CRSF_PwrParserValue(uint8_t selected)
{
  if (crsf_pwr_parse_kind==1 && crsf_pwr_parse_recognized)
  {
    crsf_pwr_max_id=crsf_pwr_field;
    crsf_pwr_power_raw=selected;
  }
  else if (crsf_pwr_parse_kind==2 && crsf_pwr_parse_off!=0xFF &&
           crsf_pwr_parse_dyn!=0xFF)
  {
    crsf_pwr_dyn_id=crsf_pwr_field;
    crsf_pwr_dyn_off=crsf_pwr_parse_off;
    crsf_pwr_dyn_on=crsf_pwr_parse_dyn;
    crsf_pwr_dynamic_raw=selected;
  }
  else if (crsf_pwr_parse_kind==3)
  {
    const uint8_t tlm=CRSF_PwrTlmUi(selected);
    if (tlm!=0xFF)
    {
      crsf_pwr_tlm_id=crsf_pwr_field;
      crsf_pwr_tlm_display=tlm;
    }
  }
  else if (crsf_pwr_parse_kind==4 &&
           selected<crsf_pwr_parse_opt_index &&
           selected<CRSF_MENU_MAX_RATES)
  {
    crsf_menu_rate_field=crsf_pwr_field;
    crsf_menu_rate_count=(crsf_pwr_parse_opt_index>CRSF_MENU_MAX_RATES) ?
                          CRSF_MENU_MAX_RATES : crsf_pwr_parse_opt_index;
    crsf_menu_rate_selected=selected;
    const uint16_t period=CRSF_RatePeriod(selected);
    if (period) CRSF_MenuApplyPeriod(period);
  }
  else if (crsf_pwr_parse_kind==6 && selected<3U &&
           (crsf_pwr_parse_recognized&(uint8_t)(1U<<selected)))
  {
    CRSF_SwitchStateWrite(crsf_pwr_field,selected,crsf_pwr_parse_recognized);
  }
}

// Process ONLY in the main loop. Each character is considered once, with
// bounded fixed-size state. The RX ISR never searches strings or divides.
static void CRSF_PwrParserFeed(uint8_t c)
{
  switch (crsf_pwr_parse_stage)
  {
    case 0: crsf_pwr_parse_stage=1; break; // parent
    case 1: crsf_pwr_parse_type=(uint8_t)(c&0x7FU);
            crsf_pwr_parse_stage=2; break;
    case 2:
      if (!c)
      {
        CRSF_PwrParserNameDone();
        crsf_pwr_parse_stage=3;
        break;
      }
      if (crsf_pwr_parse_name_len<255) ++crsf_pwr_parse_name_len;
      // CRSF_INFO (type 12) uses only the sliding domain window;
      // do not overwrite it with a simultaneous name string.
      if (crsf_pwr_parse_type!=12U &&
          crsf_pwr_parse_name_len<sizeof(crsf_pwr_parse_name))
      {
        crsf_pwr_parse_name[crsf_pwr_parse_name_len-1]=(char)c;
        crsf_pwr_parse_name[crsf_pwr_parse_name_len]=0;
      }
      if (crsf_pwr_parse_type==12U)
      {
        for (uint8_t i=0;i<5;++i) crsf_pwr_parse_win[i]=crsf_pwr_parse_win[i+1];
        crsf_pwr_parse_win[5]=c;
        const uint8_t b=CRSF_PwrBandFromInfo(crsf_pwr_parse_win,6);
        if (b==3) crsf_pwr_parse_band_24=1;
        else if (b) crsf_pwr_parse_band_sub=b;
      }
      break;
    case 3:
      if (!c)
      {
        // Do not invent a final empty option after a trailing semicolon.
        if (crsf_pwr_parse_opt_len || crsf_pwr_parse_opt_skip)
          CRSF_PwrParserOptionDone();
        crsf_pwr_parse_stage=4;
      }
      else if (c==';') CRSF_PwrParserOptionDone();
      else if (c=='(') crsf_pwr_parse_opt_skip=1;
      else if (!crsf_pwr_parse_opt_skip &&
               crsf_pwr_parse_opt_len<sizeof(crsf_pwr_parse_opt)-1)
        crsf_pwr_parse_opt[crsf_pwr_parse_opt_len++]=(char)c;
      break;
    case 4: CRSF_PwrParserValue(c);
            crsf_pwr_parse_stage=5; break;
    default: break;
  }
}

// Called only once all requested chunks for this parameter have been consumed.
static void CRSF_PwrDecode()
{
  // Display power ONLY when both radio parameters were successfully parsed.
  if (crsf_pwr_max_id && crsf_pwr_dyn_id &&
      crsf_pwr_dynamic_raw!=0xFF && crsf_pwr_power_raw!=0xFF)
  {
    crsf_pwr_display_power=0;
    if (crsf_pwr_dynamic_raw!=crsf_pwr_dyn_off)
      crsf_pwr_display_power=1;
    else
      for (uint8_t slot=0;slot<8;++slot)
        if (crsf_pwr_enum[slot]==crsf_pwr_power_raw)
        {
          crsf_pwr_display_power=(uint8_t)(slot+2);
          RF_POWER_P2M=slot;
          RF_POWER_MEM_P2M=slot;
          break;
        }
  }
  crsf_pwr_chunk=0;
  const uint8_t haveSettings=crsf_pwr_max_id && crsf_pwr_dyn_id &&
                              crsf_pwr_tlm_id && crsf_menu_rate_field &&
                              crsf_menu_switch_field;
  if (haveSettings && !crsf_pwr_band && crsf_pwr_last_field &&
      !crsf_pwr_last_queried && crsf_pwr_field!=crsf_pwr_last_field)
  {
    crsf_pwr_last_queried=1;
    crsf_pwr_field=crsf_pwr_last_field;
    CRSF_PwrParserReset();
    crsf_pwr_state=CRSF_PWR_READ_TX;
  }
  else if (haveSettings || crsf_pwr_field>=crsf_pwr_fields_total ||
           crsf_pwr_field>=CRSF_PWR_MAX_FIELDS)
    crsf_pwr_state=CRSF_PWR_READY;
  else
  {
    ++crsf_pwr_field;
    CRSF_PwrParserReset();
    crsf_pwr_state=CRSF_PWR_READ_TX;
  }
}

static void CRSF_PwrProcessFragment()
{
  // RX has stopped publishing while this state is active.
  const uint8_t n=crsf_pwr_fragment_len;
  for (uint8_t i=0;i<n;++i)
    CRSF_PwrParserFeed(crsf_pwr_fragment[i]);
  if (crsf_pwr_fragment_more)
  {
    ++crsf_pwr_chunk;
    crsf_pwr_state=CRSF_PWR_READ_TX;
  }
  else crsf_pwr_state=CRSF_PWR_DECODE;
}

// One scan for each CRSF protocol initialization. The module is authoritative:
// never restore RF values from the model EEPROM. This runs OUTSIDE interrupts.
static void CRSF_PowerStartScan(void)
{
  crsf_pwr_field=1;
  crsf_pwr_fields_total=CRSF_PWR_MAX_FIELDS;
  crsf_pwr_info_seen=0;
  crsf_pwr_params_seen=0;
  crsf_pwr_crc_err=0;
  crsf_pwr_wrong_addr=0;
  crsf_pwr_diag=1; // PING
  crsf_pwr_band=0;
  crsf_pwr_last_field=0;
  crsf_pwr_last_queried=0;
  crsf_pwr_chunk=0;
  crsf_pwr_retries=0;
  crsf_pwr_max_id=0;
  crsf_pwr_dyn_id=0;
  crsf_pwr_dyn_off=0xFF;
  crsf_pwr_dyn_on=0xFF;
  crsf_pwr_pending=0;
  crsf_pwr_write_step=0;
  crsf_pwr_power_raw=0xFF;
  crsf_pwr_dynamic_raw=0xFF;
  crsf_pwr_display_power=0;
  crsf_pwr_tlm_id=0;
  crsf_pwr_tlm_display=0xFF;
  crsf_menu_rate_count=0;
  crsf_menu_rate_field=0;
  CRSF_SwitchStateWrite(0,0,0);
  crsf_pwr_pending_rate=0;
  crsf_pwr_pending_tlm=0;
  CRSF_PwrParserReset();
  for (uint8_t i=0;i<8;++i) crsf_pwr_enum[i]=0xFF;
  crsf_pwr_scan_tick=getTmr10ms();
  crsf_pwr_last_tx_tick=(uint8_t)getTmr10ms()-CRSF_PWR_COMMAND_GAP_TICKS;
  crsf_pwr_state=CRSF_PWR_PING_TX;
}

static void CRSF_PowerInit(void)
{
  // Called by PROTOCMD_INIT, also on model/protocol changes.
  // Never query the module synchronously during OpenAVRc startup.
  crsf_pwr_state=CRSF_PWR_IDLE;
  crsf_pwr_diag=0;
  crsf_pwr_band=0;
  crsf_pwr_last_field=0;
  crsf_pwr_last_queried=0;
  // Clear both the Switch Mode packed snapshot and the boolean low bits.
  pulses2MHz.crsf_st.elrs_reuse.boot_pending=0;
  pulses2MHz.crsf_st.elrs_reuse.refresh_requested=0;
  crsf_pwr_boot_pending=1;
  crsf_pwr_init_tick=getTmr10ms();
  crsf_pwr_display_power=0;
  crsf_pwr_tlm_display=0xFF;
  crsf_menu_rate_count=0;
  crsf_menu_rate_field=0;
  crsf_menu_rate_selected=0;
  // The reused union is shared by protocols: do not depend on its old bytes
  // being zero when switching models/protocols. Boot scan resets more fields.
  crsf_pwr_sent_tick=0;
  crsf_pwr_fragment_len=0;
  crsf_pwr_fragment_more=0;
  crsf_pwr_max_id=0;
  crsf_pwr_dyn_id=0;
  crsf_pwr_dyn_off=0xFF;
  crsf_pwr_dyn_on=0xFF;
  crsf_pwr_pending=0;
  crsf_pwr_write_step=0;
  crsf_pwr_power_raw=0xFF;
  crsf_pwr_dynamic_raw=0xFF;
  crsf_pwr_tlm_id=0;
  crsf_pwr_pending_rate=0;
  crsf_pwr_pending_tlm=0;
  crsf_pwr_requested_tlm=0;
  crsf_pwr_requested_rate=0;
  crsf_pwr_requested_power=0;
}

// Called from perMain() every 10ms, independently of menu visibility.
// No delay(), serial read loops, malloc, or LCD operations here.
// G: phase_tick is an alias for mutually exclusive init/scan/refresh states.
// Do not reuse it concurrently in a future state of the scanner.
static void CRSF_PowerMain(void)
{
  const uint16_t now=getTmr10ms();
  if (crsf_pwr_boot_pending&0x01U)
  {
    if ((uint16_t)(now-crsf_pwr_init_tick)<CRSF_PWR_START_DELAY_TICKS) return;
    crsf_pwr_boot_pending&=(uint8_t)~0x01U;
    CRSF_PowerStartScan();
  }
  if (crsf_pwr_refresh_requested&0x01U)
  {
    crsf_pwr_refresh_requested&=(uint8_t)~0x01U;
    crsf_pwr_refresh_tick=now;
  }
  if (crsf_pwr_state==CRSF_PWR_REFRESH_DELAY)
  {
    if ((uint16_t)(now-crsf_pwr_refresh_tick)>=CRSF_PWR_REFRESH_DELAY_TICKS)
      CRSF_PowerStartScan();
    return;
  }
  if (crsf_pwr_state==CRSF_PWR_FRAGMENT) CRSF_PwrProcessFragment();
  if (crsf_pwr_state==CRSF_PWR_DECODE) CRSF_PwrDecode();
  // Keep radio responsive if the module is absent or misbehaving.
  if (crsf_pwr_state!=CRSF_PWR_IDLE && crsf_pwr_state!=CRSF_PWR_READY &&
      crsf_pwr_state!=CRSF_PWR_FAILED &&
      (uint16_t)(now-crsf_pwr_scan_tick)>=CRSF_PWR_SCAN_LIMIT_TICKS)
  {
    crsf_pwr_state=(crsf_pwr_max_id || crsf_pwr_tlm_id || crsf_menu_rate_field ||
                    crsf_menu_switch_field) ? CRSF_PWR_READY : CRSF_PWR_FAILED;
  }
  if (crsf_pwr_state==CRSF_PWR_READY)
    crsf_pwr_diag=(crsf_pwr_max_id || crsf_pwr_tlm_id || crsf_menu_rate_field ||
                   crsf_menu_switch_field) ? 4 : 6;
  else if (crsf_pwr_state==CRSF_PWR_FAILED)
    crsf_pwr_diag=(crsf_pwr_crc_err && !crsf_pwr_params_seen && !crsf_pwr_info_seen) ? 7 :
                  (crsf_pwr_wrong_addr && !crsf_pwr_params_seen && !crsf_pwr_info_seen) ? 8 :
                  (crsf_pwr_info_seen || crsf_pwr_params_seen) ? 6 : 5;
}

static void CRSF_PowerScheduleRefresh(void)
{
  // Bit 0 is the refresh flag; upper bits belong to the Switch Mode snapshot.
  crsf_pwr_refresh_requested|=0x01U;
  crsf_pwr_state=CRSF_PWR_REFRESH_DELAY;
}

static void CRSF_PowerEdited(uint8_t uiChoice)
{
  if (crsf_pwr_state!=CRSF_PWR_READY || !CRSF_PwrSelectable(uiChoice)) return;
  crsf_pwr_requested_power=uiChoice;
  crsf_pwr_pending=1;
  crsf_pwr_write_step=0;
}

static void CRSF_RateEdited(uint8_t selected)
{
  if (crsf_pwr_state!=CRSF_PWR_READY || !crsf_menu_rate_field ||
      selected>=crsf_menu_rate_count || !CRSF_RatePeriod(selected)) return;
  CRSF_SetRequestedRate(selected);
  crsf_pwr_pending_rate|=CRSF_PENDING_RATE;
}

static void CRSF_SwitchEdited(uint8_t selected)
{
  if (crsf_pwr_state!=CRSF_PWR_READY || !crsf_menu_switch_field ||
      !CRSF_SwitchSelectable(selected)) return;
  CRSF_SetRequestedSwitch(selected);
  crsf_pwr_pending_rate|=CRSF_PENDING_SWITCH;
}

static void CRSF_TlmEdited(uint8_t selected)
{
  if (crsf_pwr_state!=CRSF_PWR_READY || !crsf_pwr_tlm_id) return;
  if (CRSF_PwrTlmRaw(selected)==0xFF) return;
  crsf_pwr_requested_tlm=selected;
  crsf_pwr_pending_tlm=1;
}

// Model ID and Bind are standard CRSF COMMAND 0x32 operations. They are
// queued in unused bits of pending_rate and emitted by the normal RF timer,
// so no extra RAM mailbox or blocking serial write is required.
static void CRSF_ModelEdited(void)
{
  crsf_pwr_pending_rate|=CRSF_PENDING_MODEL;
}

static void CRSF_BindEdited(void)
{
  crsf_pwr_pending_rate|=CRSF_PENDING_BIND;
}

// OpenAVRc Range mode is active only while the Range field is being edited.
// CRSF has no dedicated Range command, so temporarily force 10mW. The current
// module Max Power/Dynamic snapshots are deliberately NOT refreshed while in
// Range mode, allowing them to be restored exactly when leaving the field.
static void CRSF_RangeSet(uint8_t enable)
{
  if (crsf_pwr_state!=CRSF_PWR_READY || !crsf_pwr_max_id || !crsf_pwr_dyn_id) return;

  if (enable)
  {
    if (CRSF_RangeActive()) return;
    const uint8_t saved=crsf_pwr_display_power;
    if (!CRSF_PwrSelectable(saved) || saved>9U) return;
    crsf_pwr_pending_rate=(uint8_t)((crsf_pwr_pending_rate&0x0FU) | ((saved+1U)<<4));
    crsf_pwr_requested_power=2U; // UI slot 2 = 10mW
    crsf_pwr_pending=1U;
    crsf_pwr_write_step=0U;
  }
  else if (CRSF_RangeActive())
  {
    const uint8_t saved=CRSF_RangeSavedPower();
    crsf_pwr_pending_rate&=0x0FU;
    crsf_pwr_requested_power=saved;
    crsf_pwr_pending=1U;
    // If Dynamic Power was active, first restore the original Max Power raw
    // value, then re-enable Dynamic. Bit7 marks that two-stage restore.
    crsf_pwr_write_step=(saved==1U) ? 0x80U : 0U;
  }
}

// Called ONLY after frame CRC validation by the existing RX collector.
static void CRSF_PowerRxFrame(uint8_t pos)
{
  if (crsf_pwr_state==CRSF_PWR_PING_RX &&
      crsf_menu_rxbuf[2]==CRSF_TYPE_DEVICE_INFO && pos>=20 &&
      crsf_menu_rxbuf[3]==CRSF_ADDR_RADIO &&
      crsf_menu_rxbuf[4]==CRSF_ADDR_MODULE)
  {
    crsf_pwr_info_seen=1;
    crsf_pwr_diag=2;  // module has replied (regardless of subsequent options)
    uint8_t k=5;
    while (k<pos-1 && crsf_menu_rxbuf[k]) ++k;
    if (k+13>=pos-1) return;
    uint8_t n=crsf_menu_rxbuf[k+13];
    // Real TX field count is kept separately: the version/domain info is
    // typically the LAST ELRS parameter and may have an ID above 48.
    crsf_pwr_last_field=n;
    // A zero field count is not proof that the receiver is offline.
    // Still try bounded direct parameter reads in that situation.
    crsf_pwr_fields_total = (n && n<CRSF_PWR_MAX_FIELDS) ? n : CRSF_PWR_MAX_FIELDS;
    crsf_pwr_retries=0;
    crsf_pwr_chunk=0;
    CRSF_PwrParserReset();
    crsf_pwr_state=CRSF_PWR_READ_TX;
  }
  else if (crsf_pwr_state==CRSF_PWR_READ_RX &&
           crsf_menu_rxbuf[2]==CRSF_TYPE_SETTINGS_ENTRY && pos>=8 &&
           crsf_menu_rxbuf[3]==CRSF_ADDR_RADIO &&
           crsf_menu_rxbuf[4]==CRSF_ADDR_MODULE &&
           crsf_menu_rxbuf[5]==crsf_pwr_field)
  {
    crsf_pwr_params_seen=1;
    crsf_pwr_diag=2; // 0x2B received from the correct TX module
    const uint8_t len=pos-8; // indexes 7..pos-2 inclusive, CRC excluded
    if (len>CRSF_PWR_FRAGMENT_MAX) return;
    // One fragment mailbox; publish state LAST so perMain sees a whole chunk.
    for (uint8_t j=0;j<len;++j) crsf_pwr_fragment[j]=crsf_menu_rxbuf[7+j];
    crsf_pwr_fragment_len=len;
    crsf_pwr_fragment_more=(crsf_menu_rxbuf[6] && crsf_pwr_chunk<14);
    crsf_pwr_retries=0;
    crsf_pwr_state=CRSF_PWR_FRAGMENT;
  }
}

// Writes a reversed UART packet in the same convention as RC channels.
// The actual CRSF PARAMETER_WRITE frame on the wire is:
// C8 06 2D EE EA [parameter-field-id] [selection-index] [CRC8-D5].
// DEVICE_PING uses C8 06 28 EE EA 00 00 [CRC8-D5], allowing
// ELRS endpoint parsers to inspect index/chunk safely.
static void CRSF_PwrPacket(uint8_t type, uint8_t field, uint8_t value)
{
  // ExpressLRS's endpoint parses the two parameter bytes for all extended
  // frames, including DEVICE_PING. Send zero index/chunk on ping too.
  // CRC stays standard CRSF CRC8-D5; RF RC frames are unchanged.
  const uint8_t count=8;
  Usart0TxBufferCount=count;
  uint8_t i=count;
  Usart0TxBuffer_p2M[--i]=CRSF_ADDR_FC;
  Usart0TxBuffer_p2M[--i]=count-2;
  Usart0TxBuffer_p2M[--i]=type;
  Usart0TxBuffer_p2M[--i]=CRSF_ADDR_MODULE;
  Usart0TxBuffer_p2M[--i]=CRSF_ADDR_RADIO;
  Usart0TxBuffer_p2M[--i]=(type==CRSF_TYPE_PING_DEVICES)?0:field;
  Usart0TxBuffer_p2M[--i]=(type==CRSF_TYPE_PING_DEVICES)?0:value;
  Usart0TxBuffer_p2M[0]=crsf_crc8(&Usart0TxBuffer_p2M[count-3],count-3);
  crsf_pwr_last_tx_tick=(uint8_t)getTmr10ms();
}

// CRSF COMMAND frames use a second CRC8 with polynomial 0xBA before the
// normal D5 frame CRC. Bitwise implementation saves the 256-byte lookup table.
static uint8_t CRSF_CommandCrcBA(const uint8_t *ptr, uint8_t len)
{
  uint8_t crc=0;
  while (len--)
  {
    crc^=*ptr--;
    for (uint8_t b=0;b<8U;++b)
      crc=(crc&0x80U) ? (uint8_t)((crc<<1)^0xBAU) : (uint8_t)(crc<<1);
  }
  return crc;
}

// Exact EdgeTX/CRSF command layout on wire:
// Bind:    C8 07 32 EE EA 10 01 CRC_BA CRC_D5
// ModelID: C8 08 32 EE EA 10 05 ID CRC_BA CRC_D5
static void CRSF_CommandPacket(uint8_t subcommand, uint8_t withValue, uint8_t value)
{
  const uint8_t count=withValue ? 10U : 9U;
  Usart0TxBufferCount=count;
  uint8_t i=count;
  Usart0TxBuffer_p2M[--i]=CRSF_ADDR_FC;
  Usart0TxBuffer_p2M[--i]=count-2U;
  const uint8_t typeIndex=--i;
  Usart0TxBuffer_p2M[typeIndex]=XF_TYPE_COMMAND_ID;
  Usart0TxBuffer_p2M[--i]=CRSF_ADDR_MODULE;
  Usart0TxBuffer_p2M[--i]=CRSF_ADDR_RADIO;
  Usart0TxBuffer_p2M[--i]=CRSF_SUBCOMMAND;
  Usart0TxBuffer_p2M[--i]=subcommand;
  if (withValue) Usart0TxBuffer_p2M[--i]=value;
  const uint8_t innerIndex=--i;
  Usart0TxBuffer_p2M[innerIndex]=CRSF_CommandCrcBA(&Usart0TxBuffer_p2M[typeIndex],
                                                   withValue?6U:5U);
  Usart0TxBuffer_p2M[0]=crsf_crc8(&Usart0TxBuffer_p2M[typeIndex],withValue?7U:6U);
  crsf_pwr_last_tx_tick=(uint8_t)getTmr10ms();
}

// Called by RF timer. Returns 1 when a config frame replaces an RC frame.
static uint8_t CRSF_PowerSendRequest()
{
  const uint8_t now=(uint8_t)getTmr10ms();
  uint8_t state=crsf_pwr_state;
  // Limit the number of config frames: RC packets have priority by default.
  // This does not delay the callback or stall main firmware startup.
  if ((uint8_t)(now-crsf_pwr_last_tx_tick)<CRSF_PWR_COMMAND_GAP_TICKS)
    return 0;

  // Model selection must be sent before the initial parameter scan so the
  // module exposes the RF profile belonging to this OpenAVRc model. It is
  // also resent immediately after an in-menu NumRx change.
  if (crsf_pwr_pending_rate&CRSF_PENDING_MODEL)
  {
    crsf_pwr_pending_rate&=(uint8_t)~CRSF_PENDING_MODEL;
    CRSF_CommandPacket(CRSF_COMMAND_MODEL_SELECT_ID,1U,g_model.modelId);
    if (state==CRSF_PWR_READY) CRSF_PowerScheduleRefresh();
    return 1;
  }
  if (crsf_pwr_pending_rate&CRSF_PENDING_BIND)
  {
    crsf_pwr_pending_rate&=(uint8_t)~CRSF_PENDING_BIND;
    CRSF_CommandPacket(CRSF_COMMAND_SUBCMD_RX_BIND,0U,0U);
    return 1;
  }

  if (state==CRSF_PWR_IDLE || state==CRSF_PWR_DECODE ||
      state==CRSF_PWR_FAILED || state==CRSF_PWR_REFRESH_DELAY)
    return 0;
  if (state==CRSF_PWR_READY)
  {
    // On explicit editing only. Module remains the source of truth.
    if ((crsf_pwr_pending_rate&CRSF_PENDING_RATE) && crsf_menu_rate_field)
    {
      const uint8_t selected=CRSF_RequestedRate();
      crsf_pwr_pending_rate&=(uint8_t)~CRSF_PENDING_RATE;
      if (selected<crsf_menu_rate_count && CRSF_RatePeriod(selected))
      {
        CRSF_PwrPacket(CRSF_TYPE_SETTINGS_WRITE,crsf_menu_rate_field,selected);
        CRSF_MenuApplyPeriod(CRSF_RatePeriod(selected));
        CRSF_PowerScheduleRefresh(); // re-read the actual module value
        return 1;
      }
    }
    if ((crsf_pwr_pending_rate&CRSF_PENDING_SWITCH) && crsf_menu_switch_field)
    {
      const uint8_t selected=CRSF_RequestedSwitch();
      crsf_pwr_pending_rate&=(uint8_t)~CRSF_PENDING_SWITCH;
      if (CRSF_SwitchSelectable(selected))
      {
        CRSF_PwrPacket(CRSF_TYPE_SETTINGS_WRITE,crsf_menu_switch_field,selected);
        CRSF_PowerScheduleRefresh(); // module remains authoritative
        return 1;
      }
    }
    if (crsf_pwr_pending_tlm && crsf_pwr_tlm_id)
    {
      const uint8_t raw=CRSF_PwrTlmRaw(crsf_pwr_requested_tlm);
      crsf_pwr_pending_tlm=0;
      if (raw!=0xFF)
      {
        CRSF_PwrPacket(CRSF_TYPE_SETTINGS_WRITE,crsf_pwr_tlm_id,raw);
        CRSF_PowerScheduleRefresh();
        return 1;
      }
    }
    if (!crsf_pwr_pending || !crsf_pwr_max_id || !crsf_pwr_dyn_id) return 0;
    const uint8_t selection=crsf_pwr_requested_power;
    if (!CRSF_PwrSelectable(selection)) {crsf_pwr_pending=0; return 0;}
    if (selection==1)
    {
      // Dynamic requires active RF telemetry, not a local EEPROM TLM value.
      if (crsf_pwr_tlm_display==0 || crsf_pwr_tlm_display==9 ||
          crsf_pwr_tlm_display==0xFF) return 0;
      if (crsf_pwr_write_step&0x80U)
      {
        // Leaving Range mode after DYN: restore the original Max Power first.
        if (!(crsf_pwr_write_step&0x01U))
        {
          CRSF_PwrPacket(CRSF_TYPE_SETTINGS_WRITE,crsf_pwr_max_id,crsf_pwr_power_raw);
          crsf_pwr_write_step|=0x01U;
          return 1;
        }
        crsf_pwr_write_step=0U;
      }
      CRSF_PwrPacket(CRSF_TYPE_SETTINGS_WRITE,crsf_pwr_dyn_id,crsf_pwr_dyn_on);
      crsf_pwr_pending=0;
      CRSF_PowerScheduleRefresh();
      return 1;
    }
    const uint8_t slot=selection-2;
    if (crsf_pwr_write_step==0)
    {
      CRSF_PwrPacket(CRSF_TYPE_SETTINGS_WRITE,crsf_pwr_dyn_id,crsf_pwr_dyn_off);
      crsf_pwr_write_step=1;
    }
    else
    {
      CRSF_PwrPacket(CRSF_TYPE_SETTINGS_WRITE,crsf_pwr_max_id,crsf_pwr_enum[slot]);
      crsf_pwr_write_step=0;
      crsf_pwr_pending=0;
      // While Range is active keep the original decoded RF snapshots in RAM;
      // they are needed to restore the exact Max Power/Dynamic configuration.
      if (!CRSF_RangeActive()) CRSF_PowerScheduleRefresh();
    }
    return 1;
  }
  if (state==CRSF_PWR_PING_RX || state==CRSF_PWR_READ_RX)
  {
    if ((uint8_t)(now-crsf_pwr_sent_tick)<CRSF_PWR_WAIT_TICKS) return 0;
    if (++crsf_pwr_retries>=2)
    {
      if (state==CRSF_PWR_PING_RX)
      {
        // Some TX versions do not answer our DEVICE_PING, but still accept
        // PARAMETER_READ. Try that read-only route once before declaring failure.
        crsf_pwr_diag=3; // READ (without 0x29)
        crsf_pwr_field=1;
        crsf_pwr_fields_total=CRSF_PWR_MAX_FIELDS;
        crsf_pwr_chunk=0;
        CRSF_PwrParserReset();
        crsf_pwr_retries=0;
        crsf_pwr_state=CRSF_PWR_READ_TX;
        return 0;
      }
      // Field unavailable: move on, leaving normal RC traffic untouched.
      CRSF_PwrParserReset();
      crsf_pwr_chunk=0;
      if (crsf_pwr_field>=crsf_pwr_fields_total)
        crsf_pwr_state=(crsf_pwr_max_id || crsf_pwr_tlm_id || crsf_menu_rate_field ||
                        crsf_menu_switch_field) ? CRSF_PWR_READY : CRSF_PWR_FAILED;
      else
      {
        ++crsf_pwr_field;
        crsf_pwr_retries=0;
        crsf_pwr_state=CRSF_PWR_READ_TX;
      }
      return 0;
    }
    crsf_pwr_state=(state==CRSF_PWR_PING_RX)?CRSF_PWR_PING_TX:CRSF_PWR_READ_TX;
  }
  state=crsf_pwr_state;
  if (state!=CRSF_PWR_PING_TX && state!=CRSF_PWR_READ_TX) return 0;
  const uint8_t ping=(state==CRSF_PWR_PING_TX);
  CRSF_PwrPacket(ping?CRSF_TYPE_PING_DEVICES:CRSF_TYPE_SETTINGS_READ,
                 crsf_pwr_field,crsf_pwr_chunk);
  crsf_pwr_sent_tick=now;
  crsf_pwr_state=ping?CRSF_PWR_PING_RX:CRSF_PWR_READ_RX;
  return 1;
}

static void CRSF_MenuApplyPeriod(uint16_t us)
{
  // RC frame timing is an independent 16-bit value read by RF_TIMER.
  // Save/restore SREG, not blindly sei(): this function may be invoked with
  // interrupts already disabled. A 25Hz rate uses alternating 20ms halves.
  // OpenAVRc mixer scheduling uses two 20ms halves for a 25Hz RF mode.
  if (!us) return;
  const uint8_t saved = SREG;
  cli();
  CRSF_40_MS_Flag = (us == 40000U);
  CRSF_40_MS_Flipflop = 0;
  CRSF_RATE_PERIOD = CRSF_40_MS_Flag ? 20000U : us;
  SREG = saved;
}



static uint8_t CRSF_MenuNextRate(uint8_t current, int8_t direction)
{
  const uint8_t count = crsf_menu_rate_count;
  if (!count) return current;
  for (uint8_t i = 0; i < count; ++i)
  {
    current = (direction > 0) ? ((current + 1) % count) : ((current + count - 1) % count);
    if (CRSF_RatePeriod(current)) return current;
  }
  return crsf_menu_rate_selected;
}


// Called in the protocol callback (not the UART receive ISR).
// The parameter bytes start at [parent, type, NUL-terminated field name].
static const void CRSF_MenuRxByte(uint8_t value, uint8_t error);

// Forward CRC for received CRSF frames. TX uses the reverse-order existing buffer.
static uint8_t CRSF_MenuCRC(const uint8_t *bytes, uint8_t count)
{
  uint8_t crc = 0;
  uint_farptr_t table = pgm_get_far_address(ZZcrsf_crc8tab);
  while (count--)
    crc = pgm_read_byte_far(table + (crc ^ *bytes++));
  return crc;
}

// CRSF RX collector: UART RX remains enabled in normal operation and in menu.
static const void CRSF_MenuRxByte(uint8_t value, uint8_t error)
{
  // USART0 RX callback. This code runs at IRQ priority on an 8-bit MCU.
  // The ISR collects one complete CRSF frame (length <= 64) and checks CRC8.
  // Invalid UART bytes reset the collector; sensor conversions are minimal.
  // NB: UART at 400K receives each byte ~25 microseconds apart.
  if (error)
  {
    crsf_menu_rx_count = 0;
    return;
  }
  uint8_t pos = crsf_menu_rx_count;
  if (!pos)
  {
    // Radio is the destination for ELRS answers; ignore echoed RC frames.
    // ExpressLRS module responses normally use CRSF_SYNC_BYTE (0xC8);
    // some CRSF frames may use the radio address (0xEA) as sync.
    if (value != CRSF_ADDR_FC && value != CRSF_ADDR_RADIO &&
        value != CRSF_ADDR_MODULE) return;
    crsf_menu_rxbuf[0] = value;
    crsf_menu_rx_count = 1;
    return;
  }
  if (pos == 1)
  {
    if (value < 2 || value > 62) { crsf_menu_rx_count = 0; return; }
    crsf_menu_rxbuf[1] = value;
    crsf_menu_rx_size = value + 2;
    crsf_menu_rx_count = 2;
    return;
  }
  crsf_menu_rxbuf[pos] = value;
  ++pos;
  if (pos != crsf_menu_rx_size) { crsf_menu_rx_count = pos; return; }
  crsf_menu_rx_count = 0;
  const uint8_t crcOK = (CRSF_MenuCRC(&crsf_menu_rxbuf[2], pos - 3) == crsf_menu_rxbuf[pos-1]);
  // Diagnostics only; no ASCII/LCD/parameter parsing in the UART ISR.
  // A1/VFAS/CRSF telemetry and RC decoding are left untouched.
  if (crsf_pwr_state>=CRSF_PWR_PING_TX && crsf_pwr_state<=CRSF_PWR_READ_RX &&
      (crsf_menu_rxbuf[2]==CRSF_TYPE_DEVICE_INFO ||
       crsf_menu_rxbuf[2]==CRSF_TYPE_SETTINGS_ENTRY))
  {
    if (!crcOK) { if (crsf_pwr_crc_err<255) ++crsf_pwr_crc_err; }
    else if (pos>=7 && (crsf_menu_rxbuf[3]!=CRSF_ADDR_RADIO ||
                         crsf_menu_rxbuf[4]!=CRSF_ADDR_MODULE))
    { if (crsf_pwr_wrong_addr<255) ++crsf_pwr_wrong_addr; }
  }
  if (!crcOK) return;
  CRSF_PowerRxFrame(pos); // bounded 0x29/0x2B collector, no parsing in ISR
#if defined(FRSKY) && CRSF_RX_SENSOR_THROTTLE
  // A completed, CRC-valid sensor frame may be discarded here. This reduces
  // mailbox updates and downstream S.Port work; it does NOT reduce the number
  // of CRSF bytes received on the UART (use the ELRS TLM Ratio for that).
  // Called AFTER CRSF_PowerRxFrame: parameter discovery is never throttled.
  if (!CRSF_LOCAL_TELEM_OFF && !CRSF_RxAllowSensor(crsf_menu_rxbuf[2])) return;
#endif
#if defined(FRSKY) && defined(GPS)
  // Standard CRSF GPS: 15 payload bytes, 19-byte complete packet.
  // The oXs RP2040 also emits this 0x02 format when gpsInstalled or
  // forced fields are enabled.  Do not synthesize a fix from 0,0.
  if (!CRSF_LOCAL_TELEM_OFF && crsf_menu_rxbuf[2] == CRSF_TYPE_GPS && pos == 19)
  {
    uint8_t nonzero_position = 0;
    for (uint8_t i=3; i<11; ++i) nonzero_position |= crsf_menu_rxbuf[i];
    if (nonzero_position && crsf_menu_rxbuf[17])
    {
      // One-slot GPS mailbox. Do not overwrite a waiting location or
      // restart the 5-field S.Port sequence with every fresh GPS frame.
      // crsf_gps_next_field is 8-bit (atomic on AVR).
      if (!crsf_gps_new && crsf_gps_next_field >= 5)
      {
        for (uint8_t i=0; i<15; ++i) crsf_gps_active[i] = crsf_menu_rxbuf[3+i];
        crsf_gps_new = 1;  // set LAST: consumer sees complete payload
      }
    }
  }
  // GPS output is drained by CRSF_SportTelemetryMain(), outside all ISRs.
#endif
  // Ordinary CRSF sensor frames are not extended frames. Always receive
  // them, even when the model setup menu is closed. No radio parameter
  // queries are needed to receive the telemetry frames.
#if defined(FRSKY)
  // On top of the 0x14 link stats and 0x08 battery, oXs can supply T1/T2,
  // RPM and GPS time. Copy ONLY the short payload now and return. The main
  // main loop performs conversions with interrupts enabled.
  const uint8_t extraType = crsf_menu_rxbuf[2];

  // CRSF 0x0E Cells: source_id 0..127. EdgeTX supports up to 8 cells,
  // but OpenAVRc has NUMCELLS == 6. Keep C1..C6 and deliberately ignore C7/C8.
  // Compress mV to OpenAVRc's native 0.02 V byte here so the existing 9-byte
  // extra mailbox is sufficient and no additional static SRAM is allocated.
  if (!CRSF_LOCAL_TELEM_OFF &&
      extraType == CRSF_TYPE_VOLTAGES &&
      pos >= 17 &&
      crsf_menu_rxbuf[3] < 128U)
  {
    if (!crsf_extra_pending)
    {
      crsf_extra_type = extraType;
      crsf_extra_size = 7; // source_id + six cells in 0.02 V units
      crsf_extra_payload[0] = crsf_menu_rxbuf[3];
      for (uint8_t i=0; i<6U; ++i)
      {
        const uint8_t p = (uint8_t)(4U + i*2U);
        const uint16_t mv = ((uint16_t)crsf_menu_rxbuf[p] << 8) |
                            crsf_menu_rxbuf[p+1U];
        uint16_t cell = (mv + 10U) / 20U; // 20 mV per OpenAVRc cell unit
        if (cell > 255U) cell = 255U;
        crsf_extra_payload[1U+i] = (uint8_t)cell;
      }
      crsf_extra_pending = 1; // publish LAST
    }
    return;
  }

  // ExpressLRS AnalogVbat 0x0E: source_id 128.
  // Keep RxBt OUT of the shared extra mailbox: oXs Battery ID 1 also uses
  // that mailbox for VFAS/Curr/Capa and could otherwise starve receiver Vbat.
  // Reuse the existing battery mailbox instead. Bit 2 of pending marks that
  // crsf_battery_volts_dv temporarily contains raw millivolts (not 0.1V).
  if (!CRSF_LOCAL_TELEM_OFF &&
      extraType == CRSF_TYPE_VOLTAGES &&
      pos >= 7 &&
      crsf_menu_rxbuf[3] == 128)
  {
    // RxBt is receiver-local and must not be starved by another pending
    // battery sample.  ID1 oXs battery frames use crsf_extra_pending, so it
    // is safe to give source_id 128 priority in this existing mailbox.
    const uint16_t mv = ((uint16_t)crsf_menu_rxbuf[4] << 8) | crsf_menu_rxbuf[5];
    if (mv != 0U && mv != 0xFFFFU)
    {
      crsf_battery_volts_dv = mv;       // raw mV; converted in perMain()
      crsf_battery_current_da = 0;
      crsf_battery_pending = 0x07U;     // valid + internal RxBt + raw mV
    }
    return;
  }

  if (!CRSF_LOCAL_TELEM_OFF &&
      ((extraType == CRSF_TYPE_TEMPERATURE && (pos == 7 || pos == 9)) ||
       (extraType == CRSF_TYPE_RPM && pos == 8) ||
       (extraType == CRSF_TYPE_GPS_TIME && pos == 13)))
  {
    const uint8_t n = pos-4;  // excludes address, length, type, and CRC
    if (!crsf_extra_pending && n <= sizeof(crsf_extra_payload))
    {
      crsf_extra_type = extraType;
      crsf_extra_size = n;
      for (uint8_t i=0; i<n; ++i) crsf_extra_payload[i] = crsf_menu_rxbuf[3+i];
      crsf_extra_pending = 1; // set LAST so consumer never sees partial bytes
    }
    return;
  }
  if (!CRSF_LOCAL_TELEM_OFF && crsf_menu_rxbuf[2] == CRSF_TYPE_LINK && pos == 14)
  {
    // UART IRQ: copy only. perMain() will enqueue FrSky S.Port packets.
    if (!crsf_link_pending)
    {
      crsf_link_rx_lq = crsf_menu_rxbuf[5] <= 100 ? crsf_menu_rxbuf[5] : 100;
      crsf_link_tx_lq = crsf_menu_rxbuf[11] <= 100 ? crsf_menu_rxbuf[11] : 100;
      crsf_link_pending = 1; // Publish last; prevents torn reads.
    }
    return;
  }
  // Standard CRSF Battery Sensor 0x08.
  // Legacy frame: 8-byte payload, no battery ID -> complete packet pos==12.
  // Current CRSF frame: optional ID follows Remaining -> pos==13, ID at [11].
  // Explicit ID 0 is receiver/ELRS RxBt. Any non-zero ID is an external
  // battery source; oXs uses ID 1 and is deferred through the existing
  // 9-byte extra mailbox so VFAS + Curr + Capa are handled in main context.
  if (!CRSF_LOCAL_TELEM_OFF && crsf_menu_rxbuf[2] == CRSF_TYPE_BATTERY &&
      (pos == 12 || pos == 13))
  {
    const uint8_t hasBatteryId = (pos == 13) ? 1U : 0U;
    const uint8_t batteryId = hasBatteryId ? crsf_menu_rxbuf[11] : 0U;

    if (hasBatteryId && batteryId != 0U)
    {
      if (!crsf_extra_pending)
      {
        crsf_extra_type = CRSF_TYPE_BATTERY;
        crsf_extra_size = 9;
        for (uint8_t i=0; i<9; ++i)
          crsf_extra_payload[i] = crsf_menu_rxbuf[3+i];
        crsf_extra_pending = 1; // publish LAST
      }
      return;
    }

    if (!crsf_battery_pending)
    {
      crsf_battery_volts_dv =
          ((uint16_t)crsf_menu_rxbuf[3] << 8) | crsf_menu_rxbuf[4];
      crsf_battery_current_da =
          ((uint16_t)crsf_menu_rxbuf[5] << 8) | crsf_menu_rxbuf[6];

      // Explicit ID 0 is always receiver voltage. For old frames without ID,
      // preserve the validated heuristic so existing external sensors still work.
      uint8_t internalVbat = hasBatteryId;
      if (!hasBatteryId)
      {
        // Legacy/no-ID ELRS receiver battery frames may carry a Remaining
        // byte even though Current and Capacity are zero.  oXs now uses
        // explicit Battery ID 1, so classify zero-current/zero-capacity
        // no-ID frames as receiver RxBt regardless of Remaining.
        internalVbat =
            (crsf_menu_rxbuf[5] == 0U && crsf_menu_rxbuf[6] == 0U &&
             crsf_menu_rxbuf[7] == 0U && crsf_menu_rxbuf[8] == 0U &&
             crsf_menu_rxbuf[9] == 0U) ? 1U : 0U;
      }

      crsf_battery_pending = internalVbat ? 0x03U : 0x01U;
    }
    return;
  }
#endif
}

#else
static uint8_t crsf_menu_band = 0;
#endif

/* from CRSF document
Center (1500us) = 992
TICKS_TO_US(x) ((x - 992) * 5 / 8 + 1500)
US_TO_TICKS(x) ((x - 1500) * 8 / 5 + 992)
*/

#define CSRF_MIN 172
#define CSRF_MAX 1811
// Keep CH5 (AUX1, array index 4) fully proportional in the CRSF frame,
// exactly like the other 15 channels. Do NOT quantize CH5 here:
// ExpressLRS decides whether it is a 2-position ARM channel or a full-res
// proportional channel depending on its arming method and switch mode.
// WARNING: "Arm using Switch" (ELRS v4+) also requires a separate arming
// state in the CRSF channels frame; this firmware does not emit it yet.
// With the currently supported "Arm using CH5", assign a 2-pos mix to CH5
// (low = disarmed, high = armed); an unset mix outputs the midpoint.
static void build_CRSF_data_pkt()
{
  #if defined(X_ANY)
  Xany_scheduleTx_AllInstance();
  #endif

  Usart0TxBufferCount = CRSF_CHAN_PACKET_SIZE;
  uint8_t crsfTxBufferCount = Usart0TxBufferCount;

  Usart0TxBuffer_p2M[--crsfTxBufferCount] = CRSF_ADDR_MODULE;
  Usart0TxBuffer_p2M[--crsfTxBufferCount] = CRSF_CHAN_PACKET_SIZE - 2;   // length of type + payload + crc
  Usart0TxBuffer_p2M[--crsfTxBufferCount] = CRSF_TYPE_CHANNELS;

  int16_t value;
  uint32_t bits = 0;
  uint8_t bitsavailable = 0;

  for (uint8_t i=0; i < CRSF_CHANNELS; i++)
    {
      if (i < 16/*todoModel.num_channels*/)
        {
          value = calcRESXto1000(FULL_CHANNEL_OUTPUTS(i))*8/10;
          value += 992; // Add midpoint value
        }
      else
        value = 992;  // midpoint

      bits |= (uint32_t)value << bitsavailable;
      bitsavailable += 11; // 11 bits per channel
      while (bitsavailable >= 8)
        {
          Usart0TxBuffer_p2M[--crsfTxBufferCount] = ((uint8_t) (bits & 0xff));
          bits >>= 8;
          bitsavailable -= 8;
        }
    }
  Usart0TxBuffer_p2M[0] = crsf_crc8(&Usart0TxBuffer_p2M[CRSF_CHAN_PACKET_SIZE-3], CRSF_CHAN_PACKET_SIZE-3);
}

// Legacy ELRS rate-change API: on ATmega2560 replaced by parameter writes
// using the actual IDs reported in the menu (0x2D). On other targets the
// older path is preserved. Do not compile dead code on M2560: -Wunused-function.
#if !defined(CPUM2560) && !defined(CPUXMEGA)
static void buildElrsPacket(uint8_t command, uint8_t value)
{
  Usart0TxBufferCount = CRSF_SET_PACKET_SIZE;
  uint8_t crsfTxBufferCount = Usart0TxBufferCount;

  Usart0TxBuffer_p2M[--crsfTxBufferCount] = CRSF_ADDR_MODULE;
  Usart0TxBuffer_p2M[--crsfTxBufferCount] = CRSF_SET_PACKET_SIZE-2;
  Usart0TxBuffer_p2M[--crsfTxBufferCount] = CRSF_TYPE_SETTINGS_WRITE;
  Usart0TxBuffer_p2M[--crsfTxBufferCount] = ELRS_ADDRESS;
  Usart0TxBuffer_p2M[--crsfTxBufferCount] = CRSF_ADDR_RADIO;
  Usart0TxBuffer_p2M[--crsfTxBufferCount] = command;
  Usart0TxBuffer_p2M[--crsfTxBufferCount] = value;
  Usart0TxBuffer_p2M[--crsfTxBufferCount] = crsf_crc8(&Usart0TxBuffer_p2M[CRSF_CHAN_PACKET_SIZE-3], CRSF_CHAN_PACKET_SIZE-3);
}
#endif // !CPUM2560: legacy packet-rate command only

uint16_t convertPktRateToPeriod()
{
  uint8_t rate = READ_CRSF_RATE;
  uint16_t freq = IS_CRSF_24_FREQ ? pgm_read_byte_near(&CRSF_RATE24[rate]) : pgm_read_byte_near(&CRSF_RATE900[rate]);
  if (freq == 0xFF) freq = 500;
  freq = 1000/freq;
  return freq;
}

// Same compatibility guard: M2560 takes the dynamic list from ELRS itself;
// this fixed lookup table is unused and must not be linked on M2560.
#if !defined(CPUM2560) && !defined(CPUXMEGA)
static uint8_t convertPktRateToElrs(uint8_t rfFreqRate)
{
  uint8_t rate = (rfFreqRate & 0x0F);
  rfFreqRate >>= 4; // keep rfFreq value

  switch (rate) // rate
    {
    case 0:
      if (rfFreqRate == 5) return 0;
      return 2;
    case 1:
      if (rfFreqRate == 5) return 1;
      return 4;
    case 2:
      if (rfFreqRate == 5) return 3;
      return 5;
    case 3:
      if (rfFreqRate == 5) return 5;
      return 6;
    case 4:
      return 6;
    }
  return 6;
}
#endif // !CPUM2560: old fixed RF-rate enumeration

static uint8_t check_CRSF_ParamChange()
{
  if (g_model.rfOptionValue1 != CRSF_FREQ_RATE_MEM) // freq and/or rate change ?
    {
#if defined(CPUM2560) || defined(CPUXMEGA)
      // The menu sends a proper CRSF PARAMETER_WRITE using the actual field ID.
      // Do not issue the old hard-coded ELRS command (wrong for ELRS 4.x).
      CRSF_FREQ_RATE_MEM = g_model.rfOptionValue1;
      return 0;
#else
      uint8_t rate = convertPktRateToElrs(g_model.rfOptionValue1);
      buildElrsPacket(ELRS_PKT_RATE_COMMAND, rate);
      CRSF_FREQ_RATE_MEM = g_model.rfOptionValue1;
      uint16_t period = convertPktRateToPeriod();
      if (period == 40)
        {
          CRSF_40_MS_Flag = 1;
          period /= 2; // use CRSF_40_MS_Flipflop
        }
      else
        {
          CRSF_40_MS_Flag = 0;
        }
      CRSF_RATE_PERIOD = period * 1000U;
      SCHEDULE_MIXER_END_IN_US(CRSF_RATE_PERIOD); // Schedule new next Mixer calculations.
      return 1;
#endif
    }
  return 0;
}

static uint16_t CRSF_SERIAL_cb()
{
  SCHEDULE_MIXER_END_IN_US(CRSF_RATE_PERIOD); // Schedule next Mixer calculations.
  if (!(CRSF_40_MS_Flag && (CRSF_40_MS_Flipflop ^= 0x1)))
    {
      if (!check_CRSF_ParamChange())
      {
#if defined(CPUM2560) || defined(CPUXMEGA)
        // NEVER read menuHandlers[menuLevel] from the timer interrupt:
        // menuHandlers contains function pointers (16-bit on AVR) and the UI
        // changes them while navigating with the keys. A torn read can give
        // an inconsistent result. An 8-bit UI heartbeat avoids this race.
        // 75 ticks = 750ms, ample room for a normal LCD refresh.
        if (!CRSF_PowerSendRequest())
#endif
          build_CRSF_data_pkt();
      }
      #if !defined(SIMU)
#if defined(CPUXMEGA)
      // EVO V2 half-duplex: drive TXD only while a CRSF frame is sent.
      USART_DISABLE_RX(CRSF_USART);
      CRSF_USART.STATUS = USART_TXCIF_bm; // Clear stale TX-complete flag.
      RF_PORT.DIRSET = USART_TXD_PIN_bm;
#endif
      USART_TRANSMIT_BUFFER(CRSF_USART);
      #endif
      heartbeat |= HEART_TIMER_PULSES;
    }
  CALCULATE_LAT_JIT(); // Calculate latency and jitter.
  return CRSF_RATE_PERIOD *2;
}


static void CRSF_initialize(uint8_t bind)
{
#if defined(CPUM2560) || defined(CPUXMEGA)
  crsf_menu_rx_count = 0;
  crsf_menu_rx_size = 0;
  CRSF_PowerInit();
  // Tell ExpressLRS which OpenAVRc model is active before its RF parameters
  // are scanned. Same concept as SPI NumRx; this does not initiate a bind.
  crsf_pwr_pending_rate|=CRSF_PENDING_MODEL;
  if (bind) crsf_pwr_pending_rate|=CRSF_PENDING_BIND;
#endif
  // CRSF 115200 8N1 - first HALF interface test.
  // External ELRS on single-wire BAT45 interface. UART RX stays active.
  // Selectable UART baudrate on ATmega2560.
#if defined(CPUXMEGA)
  // 32 MHz / (16 * (4+1)) = 400000 baud (BSCALE=0, BSEL=4).
  // Use the existing tuned 115200 baud setting for slower ELRS modules.
  // USART macros expand to compound statements; explicit braces keep else valid.
  if (CRSF_MENU_FAST_BAUD) {
    USART_SET_BAUD_400K(CRSF_USART);
  }
  else {
    USART_SET_BAUD_115K2(CRSF_USART);
  }
#elif defined(CPUM2560)
  // One model bit chooses UART speed. Keep 115200 for existing models.
  if (CRSF_MENU_FAST_BAUD)
    {
      USART_SET_BAUD(CRSF_USART, 400000UL); // F_CPU=16MHz, U2X=1, UBRR=4
    }
  else
    {
      USART_SET_BAUD_115K2(CRSF_USART);
    }
#else
  // Keep the established USART setup for other targets.
  USART_SET_BAUD_115K2(CRSF_USART);
#endif
  USART_SET_MODE_8N1(CRSF_USART);
  USART_ENABLE_TX(CRSF_USART);
#if defined(CPUXMEGA)
  // Release the half-duplex signal line until the next frame.
  RF_PORT.DIRCLR = USART_TXD_PIN_bm;
  CRSF_USART.CTRLA &= ~USART_TXCINTLVL_gm;
#endif
  Usart0TxBufferCount = 0;
#if defined(CPUM2560) || defined(CPUXMEGA)
#if defined(FRSKY)
  // Always select the FrSky S.Port backend for CRSF, even when the local
  // telemetry switch is OFF. OFF suppresses sensor conversion only; RX is
  // needed for the ELRS Packet Rate / TLM Ratio parameter menu.
  SET_USR_PROTO_SMART_PORT();
#endif
#if defined(FRSKY) && defined(GPS)
  crsf_gps_next_field = 5;
  crsf_gps_last_tick = 0;
  crsf_gps_new = 0;
#endif
#if defined(FRSKY)
  // Discard leftover temperature/RPM/time mailbox from another model.
  crsf_extra_pending = 0;
  crsf_link_pending = 0;
  crsf_battery_pending = 0;
  crsf_sport_to_send = 0;
  crsf_sport_tx_lq = 0;     // G: always reset reused union state at init.
  crsf_sport_battery_dv = 0;
  crsf_sport_current_da = 0;
  crsf_sport_next_sensor = 0;
#if CRSF_RX_SENSOR_THROTTLE
  crsf_rx_sensor_seen = 0;
#endif
#endif
  parseSerialTelemFunction = CRSF_MenuRxByte;
  USART_ENABLE_RX(CRSF_USART);
#endif
#if defined(CPUM2560) || defined(CPUXMEGA)
  // 50Hz SAFE startup until Packet Rate is actually read from the TX module.
  // EEPROM is not authoritative for ExpressLRS RF settings.
  CRSF_MenuApplyPeriod(20000U);
#else
  CRSF_RATE_PERIOD = convertPktRateToPeriod() * 1000U;
#endif
  CRSF_FREQ_RATE_MEM = g_model.rfOptionValue1;
  PROTO_Start_Callback( CRSF_SERIAL_cb);
}

// SIMU ONLY: preserve the same power-index convention as the SPI protocols.
// rfOptionValue3 is the MENU selection: 0=--, 1=DYN, 2..9=fixed power.
// RF_POWER_P2M is only a FIXED power level (0..7), NEVER a menu mode.
// DYN/-- leave the last fixed power index untouched; real ELRS dynamic power
// needs a separate CRSF parameter command, not yet implemented here.
// RF_POWER_MEM_P2M mirrors a simulated APPLIED power, not the menu choice.
#if defined(SIMU)
void CRSF_ManagePower(void)
{
  uint8_t requested;

  if (systemBolls.rangeModeIsOn)
  {
    requested = TXPOWER_1;   // Range mode: same power-index convention as SPI.
  }
  else
  {
    const uint8_t selection = g_model.rfOptionValue3;
    if (selection < 2 || selection > 9)
      return; // -- or DYN (or invalid): not a fixed power level.
    requested = selection - 2; // 10mW..2000mW map to indexes 0..7.
  }

  RF_POWER_P2M = requested;
  if (RF_POWER_P2M != RF_POWER_MEM_P2M)
  {
    // Simulator-only virtual application. The real module is NOT written.
    RF_POWER_MEM_P2M = RF_POWER_P2M;
  }
}
#endif

const void *CRSF_Cmds(enum ProtoCmds cmd)
{
  switch(cmd)
    {
    case PROTOCMD_INIT:
      CRSF_initialize(0);
      return 0;
    case PROTOCMD_RESET:
      PROTO_Stop_Callback();
#if defined(CPUM2560) || defined(CPUXMEGA)
      crsf_menu_rx_count=0;
      crsf_pwr_state=CRSF_PWR_IDLE;
      crsf_pwr_boot_pending=0;
      crsf_pwr_refresh_requested=0;
#endif
      CRSF_Reset();
      return 0;
    case PROTOCMD_BIND:
      CRSF_initialize(1);
      return 0;
    case PROTOCMD_GETOPTIONS:
      SetRfOptionSettings(pgm_get_far_address(RfOpt_CRSF_Ser),
                          STR_DUMMY,      //Sub proto
                          STR_DUMMY,      //Option 1 (int)
                          STR_CRSF_TLM_LABEL, //Option 2 (int) - TLM Ratio
                          STR_RFPOWER,    //Option 3: RF Power (hardware CRSF write; local SIMU handling)
                          STR_TELEMETRY, //OptionBool 1: 0=ON, 1=OFF (inverted storage)
                          STR_DUMMY, //OptionBool 2: Half/Full removed
                          STR_CRSF_BAUD_LABEL //OptionBool 3: 0=115.2K, 1=400K
                         );
      return 0;
    default:
      break;
    }
  return 0;
}
