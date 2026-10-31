/*
**************************************************************************
*                                                                        *
*                 OpenAVRc -- CRSF serial interface                       *
*                                                                        *
*   Original project: OpenAVRc (GPL-2.0-or-later).                        *
*   CRSF buffer reuse patch: experimental, ATmega2560.                    *
**************************************************************************
*/
#ifndef CRSF_SERIAL_H
#define CRSF_SERIAL_H

#define CRSF_CHAN_PACKET_SIZE     26

typedef struct
{
  uint8_t crsf_tx_buffer[CRSF_CHAN_PACKET_SIZE]; // Reserved aka Usart0TxBuffer_p2M
#if (defined(CPUM2560) || defined(CPUXMEGA))
  // ATmega2560: the historical CRSF scratch fields from the original layout were unused
  // by the current protocol code. Retain the 26-byte active TX frame ONLY.
  // No RX ISR buffer, telemetry mailbox, or RF-timer scratch lives here.
  // A compile error from any other user's source referencing an old field
  // is intentional: do not silently alias live state.
#else
  // Historical layout unchanged for other targets.
  volatile uint8_t crsf_rx_state;
  uint8_t crsf_freq_rate;
  uint8_t crsf_40ms_flag;
  uint8_t crsf_40ms_flipflop;
  uint16_t crsf_rate_period;
  uint8_t crsf_rx_buffer[15];
  int32_t crsf_timing_correct;
  int16_t crsf_timing_offset;
  uint8_t crsf_module_responding;
#endif
#if (defined(CPUM2560) || defined(CPUXMEGA))
  // Reuse the spare bytes already allocated by pulses2MHz.pbyte[144].
  // Exactly bytes 0..25 are the active CRSF TX frame.
  // The shared bytes 125..143 remain completely outside this structure.
  struct {
    // Put 16-bit fields first to avoid padding on non-AVR builds.
    // G: One 16-bit timer reused by NON-OVERLAPPING phases of TX discovery.
    // Boot: init_tick; scan: scan_tick; delay: refresh_tick.
    // Do not use simultaneously. No use of this counter in the UART/RF ISRs.
    uint16_t phase_tick;
    uint8_t parse_stage;
    uint8_t parse_type;
    uint8_t parse_kind;
    // Name, option and domain search are never needed at the same time.
    // A union avoids reserving 12+11+6 bytes for disjoint parser stages.
    union {
      char parse_name[12];
      char parse_opt[11];
      uint8_t parse_win[6];
    } parse_scratch;
    uint8_t parse_name_len;
    uint8_t parse_opt_len;
    uint8_t parse_opt_skip;
    uint8_t parse_opt_index;
    uint8_t parse_recognized;
    uint8_t parse_off;
    uint8_t parse_dyn;
    uint8_t parse_band_sub;
    uint8_t parse_band_24;
    uint8_t last_field;
    uint8_t last_queried;
    volatile uint8_t info_seen;
    volatile uint8_t params_seen;
    volatile uint8_t crc_err;
    volatile uint8_t wrong_addr;
    // bit0: boolean; bits1..7 are reused by packed Switch Mode state.
    uint8_t boot_pending;
    uint8_t last_tx_tick;
    // bit0: boolean; bits1..7 are reused by packed Switch Mode state.
    volatile uint8_t refresh_requested;
    uint8_t enum_levels[8];
    uint8_t field;
    uint8_t fields_total;
    uint8_t chunk;
    uint8_t retries;
    // D: 30 more bytes move out of .bss into the protocol's 144-byte union.
    // Text is only formatted/read by the model-menu task (not UART ISR).
    char rate_text[11];
    uint8_t sent_tick;
    uint8_t max_id;
    uint8_t dyn_id;
    uint8_t dyn_off;
    uint8_t dyn_on;
    uint8_t pending;
    uint8_t write_step;
    uint8_t power_raw;
    uint8_t dynamic_raw;
    uint8_t display_power;
    uint8_t tlm_id;
    uint8_t tlm_display;
    uint8_t pending_rate;
    uint8_t pending_tlm;
    uint8_t requested_tlm;
    uint8_t requested_rate;
    uint8_t requested_power;
    uint8_t rate_count;
    uint8_t rate_field;
    // E: move 17 bytes of previously static state into the 17-byte
    // overlap recovered above. These fields remain simultaneously live.
    volatile uint8_t uart_rx_count;
    uint8_t uart_rx_size;
    volatile uint8_t fragment_len;
    volatile uint8_t fragment_more;
    volatile uint8_t pwr_state;
    volatile uint8_t pwr_diag;
    volatile uint8_t pwr_band;
    uint8_t rate_selected;
    uint8_t sport_to_send;
    uint8_t sport_next_sensor;
    uint8_t rx_sensor_last[6];
    uint8_t rx_sensor_seen;
    // G: available 5 bytes: four reclaimed from timestamps + E's spare.
    // Main-loop-only S.Port snapshots; the UART RX copies remain separate.
    uint16_t sport_battery_dv;
    uint16_t sport_current_da;
    uint8_t sport_tx_lq;
#if defined(SIMU)
  // Host compilers round this 99-byte state up to 100 bytes because of
  // 16-bit alignment. Pack only the simulator's nested state to preserve
  // the exact AVR layout and keep the original overlap checks enabled.
  } __attribute__((packed)) elrs_reuse;
#else
  } elrs_reuse;
#endif
#endif
} crsfSt_t;

// Frame Type (original OpenAVRc definitions retained)
#define XF_TYPE_GPS              0x02
#define XF_TYPE_VARIO            0x07
#define XF_TYPE_BATTERY          0x08
#define XF_TYPE_BARO_ALT         0x09
#define XF_TYPE_HEARTBEAT        0x0b
#define XF_TYPE_VTX              0x0F
#define XF_TYPE_VTX_TELEM        0x10
#define XF_TYPE_LINK             0x14
#define XF_TYPE_CHANNELS         0x16
#define XF_TYPE_RX_ID            0x1C
#define XF_TYPE_TX_ID            0x1D
#define XF_TYPE_ATTITUDE         0x1E
#define XF_TYPE_FLIGHT_MODE      0x21
#define XF_TYPE_PING_DEVICES     0x28
#define XF_TYPE_DEVICE_INFO      0x29
#define XF_TYPE_REQUEST_SETTINGS 0x2A
#define XF_TYPE_SETTINGS_ENTRY   0x2B
#define XF_TYPE_SETTINGS_READ    0x2C
#define XF_TYPE_SETTINGS_WRITE   0x2D
#define XF_TYPE_ELRS_INFO        0x2E
#define XF_TYPE_COMMAND_ID       0x32
#define XF_TYPE_RADIO_ID         0x3A

#define CRSF_UART_SYNC                      0xC8
#define CRSF_SUBCOMMAND                     0x10
#define CRSF_COMMAND_SUBCMD_RX_BIND         0x01
#define CRSF_COMMAND_MODEL_SELECT_ID        0x05
#endif // CRSF_SERIAL_H
