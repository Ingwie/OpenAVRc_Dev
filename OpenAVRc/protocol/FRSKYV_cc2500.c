

#include "../OpenAVRc.h"

static const char * const frsky_opts[] = {
  _tr_noop("Freq-Fine"),  "-127", "+127", NULL,
  NULL
};

enum {
  PROTO_OPTS_FREQFINE =0,
  LAST_PROTO_OPT,
};
//ctassert(LAST_PROTO_OPT <= NUM_PROTO_OPTS, too_many_protocol_opts);

static uint8_t Frs_packet[16];
static uint32_t seed;
static uint8_t dp_crc_init;
static uint16_t frsky_id;
static uint8_t channels_used[50];
static uint8_t channel_offset;


static uint8_t FRSKYV_crc8(uint8_t result, uint8_t *data, uint8_t len)
{
  static const uint8_t polynomial = 0x07;

  for(uint8_t i = 0; i < len; i++) {
    result = result ^ data[i];
    for(uint8_t j = 0; j < 8; j++) {
      if(result & 0x80) result = (result << 1) ^ polynomial;
      else result = result << 1;
    }
  }
  return result;
}


static uint8_t reflect8(uint8_t in)
{
  // Reflects the bits in a byte.
  uint8_t i, j, out=0;

  for(i = 0x80, j=0x01; i; i>>=1, j<<= 1) {
    if(in & i) out |= j;
  }
  return out;
}


static uint8_t calc_dp_crc_init(void)
{
// How TxId relates to data Frs_packet initial crc value.
// ID      crc start value
// 0x0000  0x0E
// 0x0001  0xD7
// 0x0002  0xBB
// 0x0003  0x62
// 0x1257  0xA6
// 0x1258  0x7D
// 0x1259  0xA4
// 0x1E2D  0x89
// 0x1E2E  0xE5
// 0x1E2F  0x3C
// 0x3210  0x1F
// 0x3FFF  0x45

// Apparently it's crc8 polynominal=0xc1 initial value=0x6b reflect in and out xor value=0.
// Fast bit by bit algorithm without augmented zero bytes.

  uint8_t c, bit;
  uint8_t crc = 0x6b; // Initial value.

  static const uint8_t poly = 0xC1;
  uint8_t * data = (uint8_t *) &frsky_id;

  for(int8_t i=1; i>-1; i--) {
    c = data[i];
    c = reflect8(c);
    for(uint8_t j=0x80; j; j>>=1) {
      bit = crc & 0x80;
      crc<<= 1;
      if (c & j) bit^= 0x80;
      if (bit) crc^= poly;
    }
  }
  return reflect8(crc);
}


static void FRSKYV_init(uint8_t bind)
{
  CC2500_SetTxRxMode(TX_EN);

  CC2500_WriteReg(CC2500_17_MCSM1, 0x0C);
//  CC2500_WriteReg(CC2500_18_MCSM0, 0x08); // Manual calibration using SCAL. Was 0x18.
  CC2500_WriteReg(CC2500_18_MCSM0, 0x18); // Auto calibrate when going from idle to tx/rx/fstxon
  CC2500_WriteReg(CC2500_06_PKTLEN, 0xFF);
  CC2500_WriteReg(CC2500_07_PKTCTRL1, 0x04);
  CC2500_WriteReg(CC2500_08_PKTCTRL0, 0x05);
//  CC2500_WriteReg(CC2500_3E_PATABLE, bind ? 0x50 : 0xFE); // power level (0xFE) 0dBm * Power Amp (RDA T212).

  CC2500_WriteReg(CC2500_0B_FSCTRL1, 0x08);

  // static const int8_t fine = 0 ; // Frsky rf deck = 0, Skyartec rf module = -17.
  CC2500_WriteReg(CC2500_0C_FSCTRL0, (int8_t) -20); // TODO Model.proto_opts[PROTO_OPTS_FREQFINE]);
  CC2500_WriteReg(CC2500_0D_FREQ2, 0x5C);
  CC2500_WriteReg(CC2500_0E_FREQ1, 0x58);
  CC2500_WriteReg(CC2500_0F_FREQ0, 0x9D);
  CC2500_WriteReg(CC2500_10_MDMCFG4, 0xAA);
  CC2500_WriteReg(CC2500_11_MDMCFG3, 0x10); // 26977 baud.
  CC2500_WriteReg(CC2500_12_MDMCFG2, 0x93);
  CC2500_WriteReg(CC2500_13_MDMCFG1, 0x23);
  CC2500_WriteReg(CC2500_14_MDMCFG0, 0x7A);
  CC2500_WriteReg(CC2500_15_DEVIATN, 0x41); // 0x41 (+-28564Hz) for 1way, 0x42 for 2way.
  CC2500_WriteReg(CC2500_19_FOCCFG, 0x16);
  CC2500_WriteReg(CC2500_1A_BSCFG, 0x6C);
  CC2500_WriteReg(CC2500_1B_AGCCTRL2, 0x43);
  CC2500_WriteReg(CC2500_1C_AGCCTRL1, 0x40);
  CC2500_WriteReg(CC2500_1D_AGCCTRL0, 0x91);
  CC2500_WriteReg(CC2500_21_FREND1, 0x56);
  CC2500_WriteReg(CC2500_22_FREND0, 0x10); // power index 0
  CC2500_WriteReg(CC2500_23_FSCAL3, 0xA9); // Enable charge pump calibration, calibrate for each hop.

  CC2500_WriteReg(CC2500_24_FSCAL2, 0x0A);
  CC2500_WriteReg(CC2500_25_FSCAL1, 0x00);
  CC2500_WriteReg(CC2500_26_FSCAL0, 0x11);
  CC2500_WriteReg(CC2500_29_FSTEST, 0x59);
  CC2500_WriteReg(CC2500_2C_TEST2, 0x88);
  CC2500_WriteReg(CC2500_2D_TEST1, 0x31);
  CC2500_WriteReg(CC2500_2E_TEST0, 0x0B);
  CC2500_WriteReg(CC2500_03_FIFOTHR, 0x07);
  CC2500_WriteReg(CC2500_09_ADDR, 0x00); // address 0

  CC2500_Strobe(CC2500_SIDLE); // Go to idle...
  CC2500_Strobe(CC2500_SFTX); // 3b
  CC2500_Strobe(CC2500_SFRX); // 3a

//TODO
//  if(proto_mode == NORMAL_MODE) CC2500_WriteReg(CC2500_3E_PATABLE, 0xFE); // D8 uses PATABLE = 0xFE for normal transmission.
//  else CC2500_WriteReg(CC2500_3E_PATABLE, 0x50); // D8 uses PATABLE = 0x50 for range testing and binding.
  CC2500_WriteReg(CC2500_3E_PATABLE, 0xF0);

  CC2500_WriteReg(CC2500_0A_CHANNR, 0x00);
  CC2500_Strobe(CC2500_SCAL); // Manual calibration
}


static void FRSKYV_build_bind_packet()
{
  static uint8_t bind_idx =0;

  Frs_packet[0] = 0x0E; //Length
  Frs_packet[1] = 0x03; //Packet type
  Frs_packet[2] = 0x01; //Packet type
  Frs_packet[3] = frsky_id & 0xFF;
  Frs_packet[4] = frsky_id >> 8;
  Frs_packet[5] = bind_idx *5; // Index into channels_used array.
  Frs_packet[6] =  channels_used[ (Frs_packet[5]) +0];
  Frs_packet[7] =  channels_used[ (Frs_packet[5]) +1];
  Frs_packet[8] =  channels_used[ (Frs_packet[5]) +2];
  Frs_packet[9] =  channels_used[ (Frs_packet[5]) +3];
  Frs_packet[10] = channels_used[ (Frs_packet[5]) +4];
  Frs_packet[11] = 0x00;
  Frs_packet[12] = 0x00;
  Frs_packet[13] = 0x00;
  Frs_packet[14] = FRSKYV_crc8(0x93, Frs_packet, 14);

  bind_idx ++;
  if(bind_idx > 9) bind_idx = 0;
}


static void FRSKYV_build_data_packet()
{
  static uint8_t data_idx =0;

  Frs_packet[0] = 0x0E;
  Frs_packet[1] = frsky_id & 0xFF;
  Frs_packet[2] = frsky_id >> 8;
  Frs_packet[3] = seed & 0xFF;
  Frs_packet[4] = seed >> 8;

  // Appears to be a bitmap relating to the number of channels sent e.g.
  // 0x0F -> first 4 channels, 0x70 -> channels 5,6,7, 0xF0 -> channels 5,6,7,8
  if(data_idx==0) Frs_packet[5] = 0x0F;
  else if(data_idx==1) Frs_packet[5] = 0xF0;
  else Frs_packet[5] = 0x00;

  uint8_t num_chan = 8 + (g_model.ppmNCH *2);
  if(num_chan > 8) num_chan = 8;
  // Potentially if we had only four channels we could send them every 9ms.

  for(uint8_t i = 0; i < 4; i++) {
    if( (i + (4* data_idx) ) < num_chan) {

    // 0x08CA / 1.5 = 1500 (us). Probably because they use 12MHz clocks.
    // 0x05DC -> 1000us
    // 0x0BB8 -> 2000us

    int16_t PPM_Range = g_model.extendedLimits ? (640 + (640>>1)): (512 + (512>>1)); // x+x/2

    int16_t value = channelOutputs[i + (4* data_idx)];
    value -= (value>>2); // x-x/4
    value = limit((int16_t)-PPM_Range, value, (int16_t)+PPM_Range);
    value += 0x08CA;

//    if(value < 0x546) value = 0x546; // 900 uS
//    else if(value > 0xC4E ) value = 0xC4E; // 2100 uS
    Frs_packet[6 + (i*2)] = value & 0xFF;
    Frs_packet[7 + (i*2)] = (value >> 8) & 0xFF;
    }
    else {
    Frs_packet[6 + (i*2)] = 0xC9;
    Frs_packet[7 + (i*2)] = 0x08;
    }
  }

  Frs_packet[14] = FRSKYV_crc8(dp_crc_init, Frs_packet, 14);
  data_idx ++;
  if(data_idx > 1) data_idx =0;
}


static uint16_t FRSKYV_data_cb()
{
    // Wait for transmit to finish.
    // while( 0x0F != CC2500_Strobe(CC2500_SNOP)) { _delay_us(5); }
    CC2500_Strobe(CC2500_SIDLE);
    CC2500_WriteReg(CC2500_0A_CHANNR, channels_used[ ( (seed & 0xFF) % 50) ] );
    CC2500_Strobe(CC2500_SFTX); // Flush Tx FIFO.
    CC2500_WriteData(Frs_packet, 15);
    CC2500_Strobe(CC2500_STX);


    // Build next Frs_packet. Incurs a latency of 9ms. This can be done whilst previous Frs_packet is being emitted.
    seed = (uint32_t) (seed * 0xAA) % 0x7673; // Prime number 30323.
    FRSKYV_build_data_packet();
    // CC2500_Strobe(CC2500_SNOP); // Just shows how long to build Frs_packet. 16MHz AVR = 126us.

    // TODO Update power level and fine frequency tuning.
    // CC2500_WriteReg(CC2500_0C_FSCTRL0, option);
    // CC2500_SetPower();

    // Schedule next Mixer calculations.
    SCHEDULE_MIXER_END((uint16_t) (9 *2) *8);

    heartbeat |= HEART_TIMER_PULSES;
    return 9000 *2;
}


static uint16_t FRSKYV_bind_cb()
{
  FRSKYV_build_bind_packet();
  CC2500_Strobe(CC2500_SIDLE);
  CC2500_WriteReg(CC2500_0A_CHANNR, 0);
  CC2500_Strobe(CC2500_SFTX); // Flush Tx FIFO
  CC2500_WriteData(Frs_packet, 15);
  CC2500_Strobe(CC2500_STX); // Tx
  heartbeat |= HEART_TIMER_PULSES;
  return 18000U *2;
}


static void FRSKYV_initialise(uint8_t bind)
{
  CLOCK_StopTimer();

  //TODO  frsky_id = Model.fixed_id % 0x4000;
  frsky_id = 0xABCD % 0x4000; // = 0x2BCD pour Moi.

  // Build channel array.
  channel_offset = frsky_id % 5;
  for(uint8_t x = 0; x < 50; x ++) channels_used[x] = (x*5) + 6 + channel_offset;

  dp_crc_init = calc_dp_crc_init();

  CC2500_Reset(); // 0x30

  if(bind) {
    FRSKYV_init(1);
    PROTOCOL_SetBindState(0xFFFFFFFF);
    CLOCK_StartTimer(25000U *2, &FRSKYV_bind_cb);
  }
  else {
    FRSKYV_init(0);
    seed = 2UL;
    FRSKYV_build_data_packet();
    CLOCK_StartTimer(25000U *2, &FRSKYV_data_cb);
  }
}



const void * FRSKYV_Cmds(enum ProtoCmds cmd)
{
  switch(cmd) {
    case PROTOCMD_INIT: FRSKYV_initialise(0); return 0;
//        case PROTOCMD_DEINIT:
//        case PROTOCMD_RESET:
//            CLOCK_StopTimer();
//            return (void *)(CC2500_Reset() ? 1L : -1L);
    case PROTOCMD_CHECK_AUTOBIND: return 0; //Never Autobind.
    case PROTOCMD_BIND:
    	proto_mode = BIND_MODE; // TODO
    	FRSKYV_initialise(1); return 0;
    case PROTOCMD_NUMCHAN: return (void *)8L;
    case PROTOCMD_DEFAULT_NUMCHAN: return (void *)8L;
//        case PROTOCMD_CURRENT_ID: return Model.fixed_id ? (void *)((unsigned long)Model.fixed_id % 0x4000) : 0;
    case PROTOCMD_GETOPTIONS: return frsky_opts;
    case PROTOCMD_TELEMETRYSTATE: return (void *)(long) PROTO_TELEM_UNSUPPORTED;
    default: break;
  }
  return 0;
}


