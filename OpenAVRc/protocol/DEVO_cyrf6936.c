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


#include "../OpenAVRc.h"

#define PKTS_PER_CHANNEL 4
#define DEVO_BIND_COUNT 0x1388 // 12 seconds.
#define TELEMETRY_ENABLE 0x30
#define DEVO_NUM_WAIT_LOOPS (100 / 5) // Each loop is ~5us.  Do not wait more than 100us

static const char * const DEVO_opts[] = {
  _tr_noop("Telemetry"),  _tr_noop("On"), _tr_noop("Off"), NULL,
  _tr_noop("Bind_type"),  _tr_noop("Temp"), _tr_noop("Perm"), NULL,
  NULL
};

enum {
  DEVO_OPT_TELEMETRY = 0,
  DEVO_OPT_BIND_TYPE,
  DEVO_OPT_LAST,
};
//ctassert(LAST_PROTO_OPT <= NUM_PROTO_OPTS, too_many_protocol_opts);


enum PktState {
  DEVO_BIND,
  DEVO_BIND_SENDCH,
  DEVO_BOUND,
  DEVO_BOUND_1,
  DEVO_BOUND_2,
  DEVO_BOUND_3,
  DEVO_BOUND_4,
  DEVO_BOUND_5,
  DEVO_BOUND_6,
  DEVO_BOUND_7,
  DEVO_BOUND_8,
  DEVO_BOUND_9,
  DEVO_BOUND_10,
};

static int16_t bind_counter;
static uint8_t DEVO_state;
static uint8_t txState;
static uint32_t DEVO_fixed_id;
static uint8_t radio_ch[5];
static uint8_t *radio_ch_ptr;
static uint8_t pkt_num;
//static uint8_t cyrfmfg_id[6];
static uint8_t DEVO_num_channels;
static uint8_t failsafe_pkt;


static void DEVO_scramble_pkt()
{
#ifdef NO_SCRAMBLE
  return;
#else
  for(uint8_t i = 0; i < 15; i++) {
    packet[i + 1] ^= cyrfmfg_id[i % 4];
  }
#endif
}

static void DEVO_add_pkt_suffix()
{
  uint8_t bind_state;

#define TEMPORARY_BIND 1 // For Ladybird Quad.

  if (!TEMPORARY_BIND) {
    if (bind_counter > 0) bind_state = 0xC0;
    else bind_state = 0x80;
  }
  else bind_state = 0x00;

  packet[10] = bind_state | (PKTS_PER_CHANNEL - pkt_num - 1);
  packet[11] = *(radio_ch_ptr + 1);
  packet[12] = *(radio_ch_ptr + 2);
  packet[13] = DEVO_fixed_id  & 0xff;
  packet[14] = (DEVO_fixed_id >> 8) & 0xff;
  packet[15] = (DEVO_fixed_id >> 16) & 0xff;
}


static void DEVO_build_beacon_pkt(uint8_t upper)
{
    packet[0] = ((DEVO_num_channels << 4) | 0x07);
    uint8_t enable = 0;
    int max = 8;
    int offset = 0;
    if (upper) {
        packet[0] += 1;
        max = 4;
        offset = 8;
    }
    for(uint8_t i = 0; i < max; i++) {
//        if (i + offset < Model.num_channels && Model.limits[i+offset].flags & CH_FAILSAFE_EN) {
//            enable |= 0x80 >> i;
//            packet[i+1] = Model.limits[i+offset].failsafe;
//        } else {
            packet[i+1] = 0;
//        }
    }
    packet[9] = enable;
    DEVO_add_pkt_suffix();
}

static void DEVO_build_bind_pkt()
{
  packet[0] = (DEVO_num_channels << 4) | 0x0a;
  packet[1] = bind_counter & 0xff;
  packet[2] = (bind_counter >> 8);
  packet[3] = *radio_ch_ptr;
  packet[4] = *(radio_ch_ptr + 1);
  packet[5] = *(radio_ch_ptr + 2);
  packet[6] = cyrfmfg_id[0];
  packet[7] = cyrfmfg_id[1];
  packet[8] = cyrfmfg_id[2];
  packet[9] = cyrfmfg_id[3];
  DEVO_add_pkt_suffix();
  // The fixed-id portion is scrambled in the bind packet
  // I assume it is ignored
  packet[13] ^= cyrfmfg_id[0];
  packet[14] ^= cyrfmfg_id[1];
  packet[15] ^= cyrfmfg_id[2];
}

static void DEVO_build_data_pkt()
{
  static uint8_t ch_idx =0;

  packet[0] = (DEVO_num_channels << 4) | (0x0b + ch_idx);
    uint8_t sign = 0x0b;
    for (uint8_t i = 0; i < 4; i++) {

    	int16_t value = channelOutputs[i + (4* ch_idx)];
    	value = value + (value >> 1) + (value >> 4); // Range +/- 1600.

        if(value < 0) {
            value = -value;
            sign |= 1 << (7 - i);
        }
        packet[2 * i + 1] = value & 0xff;
        packet[2 * i + 2] = (value >> 8) & 0xff;
    }
    packet[9] = sign;
    ch_idx++;
    if (ch_idx * 4 >= DEVO_num_channels)
        ch_idx = 0;
    DEVO_add_pkt_suffix();
}

#if 0
static int32_t float_to_int(uint8_t *ptr)
{
    int32_t value = 0;
    int seen_decimal = 0;
    for(int i = 0; i < 7; i++) {
        if(ptr[i] == '.') {
            value *= 1000;
            seen_decimal = 100;
            continue;
        }
        if(ptr[i] == 0)
            break;
        if(seen_decimal) {
            value += (ptr[i] - '0') * seen_decimal;
            seen_decimal /= 10;
            if(! seen_decimal)
                break;
        } else {
            value = value * 10 + (ptr[i] - '0');
        }
    }
    return value;
}

static uint32_t text_to_int(uint8_t *ptr, uint8_t len)
{
    uint32_t value = 0;
    for(uint8_t i = 0; i < len; i++) {
        value = value * 10 + (ptr[i] - '0');
    }
    return value;
}


static void parse_telemetry_packet()
{
    static const uint8_t voltpkt[] = {
            TELEM_DEVO_VOLT1, TELEM_DEVO_VOLT2, TELEM_DEVO_VOLT3,
            TELEM_DEVO_RPM1, TELEM_DEVO_RPM2, 0
        };
    static const uint8_t temppkt[] = {
            TELEM_DEVO_TEMP1, TELEM_DEVO_TEMP2, TELEM_DEVO_TEMP3, TELEM_DEVO_TEMP4, 0
        };
    static const uint8_t gpslongpkt[] = { TELEM_GPS_LONG, 0};
    static const uint8_t gpslatpkt[] = { TELEM_GPS_LAT, 0};
    static const uint8_t gpsaltpkt[] = { TELEM_GPS_ALT, 0};
    static const uint8_t gpsspeedpkt[] = { TELEM_GPS_SPEED, 0};
    static const uint8_t gpstimepkt[] = { TELEM_GPS_TIME, 0};

    scramble_pkt(); //This will unscramble the packet
    if (((packet[0] & 0xF0) != TELEMETRY_ENABLE) ||
        ((((uint32_t)packet[15] << 16) | ((uint32_t)packet[14] << 8) | packet[13]) != (DEVO_fixed_id & 0x00ffffff)))
    {
        return;
    }
    const uint8_t *update = &gpstimepkt[1];
    uint32_t step = 1;
    uint32_t idx = 1;
    //if (packet[0] < 0x37) {
    //    memcpy(Telemetry.line[packet[0]-0x30], packet+1, 12);
    //}
    if (packet[0] == TELEMETRY_ENABLE) {
        update = voltpkt;
        step = 2;
        Telemetry.value[TELEM_DEVO_VOLT1] = packet[1]; //In 1/10 of Volts
        Telemetry.value[TELEM_DEVO_VOLT2] = packet[3]; //In 1/10 of Volts
        Telemetry.value[TELEM_DEVO_VOLT3] = packet[5]; //In 1/10 of Volts
        Telemetry.value[TELEM_DEVO_RPM1]  = packet[7] * 120; //In RPM
        Telemetry.value[TELEM_DEVO_RPM2]  = packet[9] * 120; //In RPM
    }
    if (packet[0] == 0x31) {
        update = temppkt;
        while (*update) {
            Telemetry.value[*update] = packet[idx];
            if (packet[idx++] != 0xff)
                TELEMETRY_SetUpdated(*update);
            update++;
        }
        return;
    }
    /* GPS Data
       32: 30333032302e3832373045fb  = 030°20.8270E
       33: 353935342e373737364e0700  = 59°54.776N
       34: 31322e380000004d4d4e45fb  = 12.8 MMNE (altitude maybe)?
       35: 000000000000302e30300000  = 0.00 (probably speed)
       36: 313832353532313531303132  = 2012-10-15 18:25:52 (UTC)
    */
    if (packet[0] == 0x32) {
        update = gpslongpkt;
        Telemetry.gps.longitude = text_to_int(&packet[1], 3) * 3600000
                                + text_to_int(&packet[4], 2) * 60000
                                + text_to_int(&packet[7], 4) * 6;
        if (packet[11] == 'W')
            Telemetry.gps.longitude *= -1;
    }
    if (packet[0] == 0x33) {
        update = gpslatpkt;
        Telemetry.gps.latitude = text_to_int(&packet[1], 2) * 3600000
                               + text_to_int(&packet[3], 2) * 60000
                               + text_to_int(&packet[6], 4) * 6;
        if (packet[10] == 'S')
            Telemetry.gps.latitude *= -1;
    }
    if (packet[0] == 0x34) {
        update = gpsaltpkt;
        Telemetry.gps.altitude = float_to_int(&packet[1]);
    }
    if (packet[0] == 0x35) {
        update = gpsspeedpkt;
        Telemetry.gps.velocity = float_to_int(&packet[7]);
        idx = 7;
    }
    if (packet[0] == 0x36) {
        update = gpstimepkt;
        uint8_t hour  = text_to_int(&packet[1], 2);
        uint8_t min   = text_to_int(&packet[3], 2);
        uint8_t sec   = text_to_int(&packet[5], 2);
        uint8_t day   = text_to_int(&packet[7], 2);
        uint8_t month = text_to_int(&packet[9], 2);
        uint8_t year  = text_to_int(&packet[11], 2); // + 2000
        Telemetry.gps.time = ((year & 0x3F) << 26)
                           | ((month & 0x0F) << 22)
                           | ((day & 0x1F) << 17)
                           | ((hour & 0x1F) << 12)
                           | ((min & 0x3F) << 6)
                           | ((sec & 0x3F) << 0);
    }
    while (*update) {
        if (packet[idx])
            TELEMETRY_SetUpdated(*update);
        update++;
        idx += step;
    }
}
#endif

static void DEVO_set_bound_sop_code()
{
  // crc == 0 isn't allowed, so use 1 if the math results in 0.
  uint8_t crc = (cyrfmfg_id[0] + (cyrfmfg_id[1] >> 6) + cyrfmfg_id[2]);

  if(! crc) crc = 1;
  uint8_t sopidx = (0xff &((cyrfmfg_id[0] << 2) + cyrfmfg_id[1] + cyrfmfg_id[2])) % 10;
  CYRF_SetTxRxMode(TX_EN);
  CYRF_ConfigCRCSeed((crc << 8) + crc);
  CYRF_PROGMEM_Config_DEVO_J6PRO_sopcodes(sopidx);
//  CYRF_ConfigSOPCode(sopcodes[sopidx]);
}


static void DEVO_init()
{
  // Initialise CYRF chip.
  CYRF_WriteRegister(CYRF_1D_MODE_OVERRIDE, 0x38);  //FRC SEN (forces the synthesizer to start) + FRC AWAKE (force the oscillator to keep running at all times)
  CYRF_WriteRegister(CYRF_03_TX_CFG, 0x08 | 7);     //Data Code Length = 32 chip codes + Data Mode = 8DR Mode + max-power(+4 dBm)
  CYRF_WriteRegister(CYRF_06_RX_CFG, 0x4A);         //LNA + FAST TURN EN + RXOW EN, enable low noise amplifier, fast turning, overwrite enable
  CYRF_WriteRegister(CYRF_0B_PWR_CTRL, 0x00);       //Reset power control
  CYRF_WriteRegister(CYRF_10_FRAMING_CFG, 0xA4);    //SOP EN + SOP LEN = 32 chips + LEN EN + SOP TH = 04h
  CYRF_WriteRegister(CYRF_11_DATA32_THOLD, 0x05);   //TH32 = 0x05
  CYRF_WriteRegister(CYRF_12_DATA64_THOLD, 0x0E);   //TH64 = 0Eh, set pn correlation threshold
  CYRF_WriteRegister(CYRF_1B_TX_OFFSET_LSB, 0x55);  //STRIM LSB = 0x55, typical configuration
  CYRF_WriteRegister(CYRF_1C_TX_OFFSET_MSB, 0x05);  //STRIM MSB = 0x05, typical configuration
  CYRF_WriteRegister(CYRF_32_AUTO_CAL_TIME, 0x3C);  //AUTO_CAL_TIME = 3Ch, typical configuration
  CYRF_WriteRegister(CYRF_35_AUTOCAL_OFFSET, 0x14); //AUTO_CAL_OFFSET = 14h, typical configuration
  CYRF_WriteRegister(CYRF_39_ANALOG_CTRL, 0x01);    //ALL SLOW
  CYRF_WriteRegister(CYRF_1E_RX_OVERRIDE, 0x10);    //FRC RXDR (Force Receive Data Rate)
  CYRF_WriteRegister(CYRF_1F_TX_OVERRIDE, 0x00);    //Reset TX overrides
  CYRF_WriteRegister(CYRF_01_TX_LENGTH, 0x10);      //TX Length = 16 byte packet
  CYRF_WriteRegister(CYRF_27_CLK_OVERRIDE, 0x02);   //RXF, force receive clock
  CYRF_WriteRegister(CYRF_28_CLK_EN, 0x02);         //RXF, force receive clock enable
  CYRF_WriteRegister(CYRF_0F_XACT_CFG, 0x28);       // Pascallanger - Not in Deviation
}


static void set_radio_channels()
{
    CYRF_FindBestChannels(radio_ch, 3, 4, 4, 80);
    //printf("Radio Channels:");
    //for (int i = 0; i < 3; i++) {
    //    printf(" %02x", radio_ch[i]);
    //}
    //printf("\n");
    //Makes code a little easier to duplicate these here
    radio_ch[3] = radio_ch[0];
    radio_ch[4] = radio_ch[1];
}

void DEVO_BuildPacket()
{
    //static unsigned cnt = 0;
    //char foo[20];
    //snprintf(foo, 20, "%d: %d", cnt++, state);
    switch(DEVO_state) {
        case DEVO_BIND:
            bind_counter--;
            DEVO_build_bind_pkt();
            DEVO_state = DEVO_BIND_SENDCH;
        break;

        case DEVO_BIND_SENDCH:
            bind_counter--;
            DEVO_build_data_pkt();
            DEVO_scramble_pkt();
            if (bind_counter <= 0) {
                DEVO_state = DEVO_BOUND;
                PROTOCOL_SetBindState(0);
            } else {
                DEVO_state = DEVO_BIND;
            }
            break;
        case DEVO_BOUND:
        case DEVO_BOUND_1:
        case DEVO_BOUND_2:
        case DEVO_BOUND_3:
        case DEVO_BOUND_4:
        case DEVO_BOUND_5:
        case DEVO_BOUND_6:
        case DEVO_BOUND_7:
        case DEVO_BOUND_8:
        case DEVO_BOUND_9:
        	DEVO_build_data_pkt();
            DEVO_scramble_pkt();
            DEVO_state ++;
            if (bind_counter > 0) {
                bind_counter--;
                if (bind_counter == 0)
                    PROTOCOL_SetBindState(0);
            }
            break;
        case DEVO_BOUND_10:
            DEVO_build_beacon_pkt(DEVO_num_channels > 8 ? failsafe_pkt : 0);
            failsafe_pkt = failsafe_pkt ? 0 : 1;
            DEVO_scramble_pkt();
            DEVO_state = DEVO_BOUND_1;
            break;
    }
    pkt_num++;
    if(pkt_num == PKTS_PER_CHANNEL)
        pkt_num = 0;
}

#if 0
static uint16_t DEVO_telemetry_cb()
{
    int delay;

    if (txState == 0) {
        DEVO_BuildPacket();
        CYRF_WriteDataPacket(packet);
        txState = 1;
        return 900;
    }
    if (txState == 1) {
        int i = 0;
        uint8_t reg;
        while (! ((reg = CYRF_ReadRegister(CYRF_04_TX_IRQ_STATUS)) & 0x02)) {
            if (++i >= DEVO_NUM_WAIT_LOOPS)
                break;
        }
        if (((reg & 0x22) == 0x20) || (CYRF_ReadRegister(CYRF_02_TX_CTRL) & 0x80)) {
            CYRF_Reset();
            cyrf_init();
            DEVO_set_bound_sop_code();
            CYRF_SetPower(TXPOWER_1); // ToDo
            CYRF_ConfigRFChannel(*radio_ch_ptr);
            //printf("Rst CYRF\n");
            delay = 1500;
            txState = 15;
        } else {
            if (DEVO_state == DEVO_BOUND) {
                /* exit binding state */
                DEVO_state = DEVO_BOUND_3;
                DEVO_set_bound_sop_code();
                CYRF_SetPower(TXPOWER_1); // ToDo
            }
            if((pkt_num != 0) && (bind_counter == 0)) {
                CYRF_SetTxRxMode(RX_EN); //Receive mode
                CYRF_WriteRegister(CYRF_05_RX_CTRL, 0x80); //Prepare to receive
                txState = 2;
                return 1300;
            }
        }
        if(pkt_num == 0) {
            //Keep tx power updated
            CYRF_SetPower(TXPOWER_1);
            radio_ch_ptr = radio_ch_ptr == &radio_ch[2] ? radio_ch : radio_ch_ptr + 1;
            CYRF_ConfigRFChannel(*radio_ch_ptr);
        }
        delay = 1500;
    }
    if(txState == 2) {  // this won't be true in emulator so we need to simulate it somehow
        uint8_t rx_state = CYRF_ReadRegister(CYRF_07_RX_IRQ_STATUS);
        if((rx_state & 0x03) == 0x02) {  // RXC=1, RXE=0 then 2nd check is required (debouncing)
            rx_state |= CYRF_ReadRegister(CYRF_07_RX_IRQ_STATUS);
        }
        if((rx_state & 0x07) == 0x02) { // good data (complete with no errors)
            CYRF_WriteRegister(CYRF_07_RX_IRQ_STATUS, 0x80); // need to set RXOW before data read
            CYRF_ReadDataPacketLen(packet, CYRF_ReadRegister(CYRF_09_RX_COUNT));
            parse_telemetry_packet();
        }
        CYRF_SetTxRxMode(TX_EN); //Write mode
#ifdef EMULATOR
        uint8_t telem_bit = rand32() % 7; // random number in [0, 7)
        packet[0] =  TELEMETRY_ENABLE + telem_bit; // allow emulator to simulate telemetry parsing to prevent future bugs in the telemetry monitor
        //printf("telem 1st packet: 0x%x\n", packet[0]);
        for(int i = 1; i < 13; i++)
            packet[i] = rand32() % 256;
        parse_telemetry_packet();
        for(int i = 0; i < TELEM_UPDATE_SIZE; i++)
            Telemetry.updated[i] = 0xff;
#endif
        delay = 200;
    }
    txState = 0;
    return delay;
}
#endif


static uint16_t DEVO_cb()
{
    if (txState == 0) {
        txState = 1;
        DEVO_BuildPacket();
        CYRF_WriteDataPacket(packet);

        heartbeat |= HEART_TIMER_PULSES;
        dt = TCNT1 - OCR1A; // Calculate latency and jitter.
        return 1200 *2;
    }
    txState = 0;
//    uint8_t i = 0;
//    while (! (CYRF_ReadRegister(CYRF_04_TX_IRQ_STATUS) & 0x02)) {
//        if(++i > DEVO_NUM_WAIT_LOOPS)
//            return 1200;
//    }
    if (DEVO_state == DEVO_BOUND) {
        /* exit binding state */
        DEVO_state = DEVO_BOUND_3;
        DEVO_set_bound_sop_code();
        CYRF_SetPower(TXPOWER_1); // ToDo
    }
    if(pkt_num == 0) {
        //Keep tx power updated
        CYRF_SetPower(TXPOWER_1); // ToDo
        radio_ch_ptr = radio_ch_ptr == &radio_ch[2] ? radio_ch : radio_ch_ptr + 1;
        CYRF_ConfigRFChannel(*radio_ch_ptr);
    }

    heartbeat |= HEART_TIMER_PULSES;
    dt = TCNT1 - OCR1A; // Calculate latency and jitter.
    return 1200 *2;
}

static void DEVO_bind()
{
  bind_counter = DEVO_BIND_COUNT;
  PROTOCOL_SetBindState(0x1388 * 2400U / 1000); // 12 seconds.
}


static void DEVO_initialize(void)
{
  CLOCK_StopTimer();
  CYRF_Reset();
  DEVO_init();
  CYRF_GetMfgData(cyrfmfg_id);
  CYRF_SetTxRxMode(TX_EN);
  CYRF_ConfigCRCSeed(0x0000);

  CYRF_PROGMEM_Config_DEVO_J6PRO_sopcodes(0);
//CYRF_ConfigSOPCode(sopcodes[0]);

  set_radio_channels();

  failsafe_pkt = 0;
  radio_ch_ptr = radio_ch;
  CYRF_ConfigRFChannel(*radio_ch_ptr);

  DEVO_num_channels = 8 + (g_model.ppmNCH *2);
  if(DEVO_num_channels > 12) DEVO_num_channels = 12;
  DEVO_num_channels = ((DEVO_num_channels  +3) >> 2) * 4;

  pkt_num = 0;
  txState = 0;

  DEVO_fixed_id = SpiRFModule.fixed_id % 1000000;
  // use_fixed_id = 1;
  bind_counter = DEVO_BIND_COUNT;
  DEVO_state = DEVO_BIND;
  PROTOCOL_SetBindState(0x1388 * 2400U / 1000U); // 12 seconds.
  CYRF_SetPower(TXPOWER_1); // ToDo

//    if (Model.proto_opts[PROTOOPTS_TELEMETRY] == TELEM_ON) {
//        CLOCK_StartTimer(2400, DEVO_telemetry_cb);
//    } else {
        CLOCK_StartTimer(25000U *2, DEVO_cb);
//    }
}

const void *DEVO_Cmds(enum ProtoCmds cmd)
{
  switch(cmd) {
    case PROTOCMD_INIT:
      DEVO_initialize();
      return 0;
    case PROTOCMD_DEINIT:
    case PROTOCMD_RESET:
    CLOCK_StopTimer();
    return (void *)(CYRF_Reset() ? 1L : -1L);
//  case PROTOCMD_CHECK_AUTOBIND: return Model.fixed_id ? 0 : (void *)1L;
    case PROTOCMD_BIND:  DEVO_bind(); return 0;
    case PROTOCMD_NUMCHAN: return (void *)12L;
    case PROTOCMD_DEFAULT_NUMCHAN: return (void *)8L;
    case PROTOCMD_GETOPTIONS: return DEVO_opts;
// case PROTOCMD_SETOPTIONS:
// PROTOCOL_Init(0);  // only 1 prot_ops item, it is to enable/disable telemetry
    break;
// case PROTOCMD_TELEMETRYSTATE:
// return (void *)(long)(Model.proto_opts[PROTOOPTS_TELEMETRY] == TELEM_ON ? PROTO_TELEM_ON : PROTO_TELEM_OFF);
// case PROTOCMD_TELEMETRYTYPE: return (void *)(long) TELEM_DEVO;
    default: break;
  }
  return 0;
}

