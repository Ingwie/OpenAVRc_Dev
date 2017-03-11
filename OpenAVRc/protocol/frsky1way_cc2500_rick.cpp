/*
 Rich
*/

#include "../OpenAVRc.h"
#include "interface.h"
#include "common.h"
#include "misc.h"

static const char * const frsky_opts[] = {
  _tr_noop("Freq-Fine"),  "-127", "+127", NULL, NULL
};

enum {
  PROTO_OPTS_FREQFINE =0,
  LAST_PROTO_OPT,
};
//ctassert(LAST_PROTO_OPT <= NUM_PROTO_OPTS, too_many_protocol_opts);


static uint8_t packet[16];
static uint32_t seed;
static uint8_t dp_crc_init;
static uint16_t frsky_id;
static uint8_t channels_used[50];
static uint8_t channel_offset;


static uint8_t crc8(uint8_t result, uint8_t * data, uint8_t len)
{
  static const uint8_t polynomial = 0x07;

  for(int i = 0; i < len; i++) {
    result = result ^ data[i];
    for(uint8_t j = 0; j < 8; j++) {
      if(result & 0x80)  result = (result << 1) ^ polynomial;
      else result = result << 1;
    }
  }
  return result;
}


static uint8_t reflect8(uint8_t in)
{
  // reflects the bits in the byte.
  uint8_t i, j, out=0;

  for(i = 0x80, j=0x01; i; i>>=1, j<<= 1) {
    if(in & i) out |= j;
  }
  return out;
}


static void calc_dp_crc_init(void)
{
// How TxId relates to data packet initial crc value.
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

// fast bit by bit algorithm without augmented zero bytes.

uint8_t c, bit;
uint8_t crc = 0x6B; // Initial value.
static const unsigned char poly = 0xC1;
uint8_t * data = (uint8_t *) frsky_id;

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
  dp_crc_init = reflect8(crc);
}


static void frsky1way_init(uint8_t bind)
{
  CC2500_SetTxRxMode(TX_EN);

  CC2500_WriteReg(CC2500_17_MCSM1, 0x0C);
  CC2500_WriteReg(CC2500_18_MCSM0, 0x08); // Manual calibration using SCAL. Was 0x18.
//  CC2500_WriteReg(CC2500_18_MCSM0, 0x18); // Auto calibrate when going from idle to tx/rx/fstxon
  CC2500_WriteReg(CC2500_06_PKTLEN, 0xFF);
  CC2500_WriteReg(CC2500_07_PKTCTRL1, 0x04);
  CC2500_WriteReg(CC2500_08_PKTCTRL0, 0x05);
  CC2500_WriteReg(CC2500_3E_PATABLE, bind ? 0x50 : 0xFE); // power level (0xFE) 0dBm * Power Amp (RDA T212).

  CC2500_WriteReg(CC2500_0B_FSCTRL1, 0x08);

  // static const int8_t fine = 0 ; // Frsky rf deck = 0, Skyartec rf module = -17.
  CC2500_WriteReg(CC2500_0C_FSCTRL0, (int8_t) Model.proto_opts[PROTO_OPTS_FREQFINE]);
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
  CC2500_WriteReg(CC2500_22_FREND0, 0x10);// power index 0
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

  CC2500_Strobe(CC2500_SIDLE);    // Go to idle...
  CC2500_Strobe(CC2500_SFTX); // 3b
  CC2500_Strobe(CC2500_SFRX); // 3a

#ifdef UNIMOD
  if(proto_mode == NORMAL_MODE) CC2500_WriteReg(CC2500_3E_PATABLE, 0xFE); // D8 uses PATABLE = 0xFE for normal transmission.
  else CC2500_WriteReg(CC2500_3E_PATABLE, 0x50); // D8 uses PATABLE = 0x50 for range testing and binding.
#endif

  CC2500_WriteReg(CC2500_0A_CHANNR, 0x00);
  CC2500_Strobe(CC2500_SCAL);    // Manual calibration
}


static void build_bind_packet_1way()
{
  static uint8_t bind_idx =0;

  packet[0] = 0x0E;                //Length
  packet[1] = 0x03;                //Packet type
  packet[2] = 0x01;                //Packet type
  packet[3] = frsky_id & 0xFF;
  packet[4] = frsky_id >> 8;
  packet[5] = bind_idx *5; // Index into channels_used array.
  packet[6] =  channels_used[ (packet[5]) +0];
  packet[7] =  channels_used[ (packet[5]) +1];
  packet[8] =  channels_used[ (packet[5]) +2];
  packet[9] =  channels_used[ (packet[5]) +3];
  packet[10] = channels_used[ (packet[5]) +4];
  packet[11] = 0x00;
  packet[12] = 0x00;
  packet[13] = 0x00;
  packet[14] = crc8(0x93, packet, 14);

  bind_idx ++;
  if(bind_idx > 9) bind_idx = 0;
}

static void build_data_packet_1way()
{
static uint8_t data_idx =0;

packet[0] = 0x0E;
packet[1] = frsky_id & 0xFF;
packet[2] = frsky_id >> 8;
packet[3] = seed & 0xFF;
packet[4] = seed >> 8;

// Appears to be a bitmap relating to the number of channels sent e.g.
// 0x0F -> first 4 channels, 0x70 -> channels 5,6,7,  0xF0 -> channels 5,6,7,8
if(data_idx==0) packet[5] = 0x0F;
else if(data_idx==1) packet[5] = 0xF0;
else packet[5] = 0x00;

// 0x08CA / 1.5 = 1500 (us)
// 0x05DC -> 1000us
// 0x0BB8 -> 2000us

for(uint8_t i = 0; i < 4; i++) {
  if( (i + (4* data_idx) ) < Model.num_channels)
  {
  int32_t value = (int32_t) Channels[i + (4* data_idx)];

#define PPM_SCALING 500L // 500 for Most systems or 550 for Multiplex. e.g. 1500 + 500 us
value = (int32_t)((PPM_SCALING * 15L * value) / (CHAN_MAX_VALUE * 10L)) + 0x08CA;
#undef PPM_SCALING

  if(value < 0x546) value = 0x546; // 900 uS
  else if(value > 0xC4E ) value = 0xC4E; // 2100 uS
  packet[6 + (i*2)] = value & 0xFF;
  packet[7 + (i*2)] = (value >> 8) & 0xFF;
  }
  else
  {
  packet[6 + (i*2)] = 0xC9;
  packet[7 + (i*2)] = 0x08;
  }
}

packet[14] = crc8(dp_crc_init, packet, 14);
data_idx ++;
if(data_idx > 1) data_idx =0;
}


static uint16_t frsky1waydata_cb()
{
static uint8_t counter =0;

	if(! (counter & 0x01))
	{
		// Tx prep
		CC2500_Strobe(CC2500_SIDLE);
		CC2500_WriteReg(CC2500_0A_CHANNR, channels_used[ ( (seed & 0xFF) % 50) ] );
		CC2500_Strobe(CC2500_SCAL);    // Manual calibration
		counter ++;
		return 1180 *2;
	}
	else
	{
	// Tx data
	    CC2500_Strobe(CC2500_SFTX); // Flush Tx FIFO
		CC2500_WriteData(packet, 15);
		CC2500_Strobe(CC2500_STX);
//
		seed = (uint32_t) (seed * 0xAA) % 0x7673; // Prime number 30323
//
	    build_data_packet_1way();
		CC2500_Strobe(CC2500_SNOP); // just shows how long to build packet. AVR = 0.7ms

		// Wait for previous transmit to finish.
		// while( 0x0F != CC2500_Strobe(CC2500_SNOP)) { _delay_us(5); }

		counter ++;
		return 7820 *2;
	}
}


static uint16_t frsky_bind_cb()
{
  CC2500_Strobe(CC2500_SIDLE);
  CC2500_WriteReg(CC2500_0A_CHANNR, 0);
// CC2500_Strobe(CC2500_SCAL); // Manual calibration
  build_bind_packet_1way();
  CC2500_Strobe(CC2500_SFTX); // Flush Tx FIFO
  CC2500_WriteData(packet, 15);
  CC2500_Strobe(CC2500_STX); // Tx
  return 18000U *2;
}


static void initialize(uint8_t bind)
{
  CLOCK_StopTimer();

  frsky_id = Model.fixed_id % 0x4000; // 0x3210 pour moi

   // Build channel array.
  channel_offset = frsky_id % 5;
  for(uint8_t x = 0; x < 50; x ++)	channels_used[x] = (x*5) + 6 + channel_offset;

  calc_dp_crc_init();

// Setup SPI Port.
#if defined (REV_EVO_V1)
//USART3
//PORTJ0 RXD3
//PORTJ1 TXD3
//PORTJ2 XCK3
//PORTJ3 CSN

// Setup pin states and USART in MSPI mode.
// Initialisation of USART.
  UBRR3 = 0; // Reset is part of initialisation sequence.

  UCSR3C = 0xC3; // UMSEL21:0 = 3 DORD2=0 CPHA2=1 CPOL2=1  USART in Master SPI mode, MSB first, Clock phase=1 Polarity=1.
  UCSR3B = (1 << RXEN3) | (1 << TXEN3); // Transmit and Receive.
  UBRR3 = 0; // 8.0MHz clock ... 16MHz/(2*(UBRR+1))

  cs_inactive

  DDRJ |= (1<<PORTJ1) | (1<<PORTJ2) | (1<<PORTJ3);
  DDRJ &= ~(1<<PORTJ0);

#define WAIT_TX_FIN()           while(! (UCSR2A & (1 << TXC2)) )
#else
#endif

  CC2500_Reset();// 0x30

  if(bind) {
    frsky1way_init(1);
    PROTOCOL_SetBindState(0xFFFFFFFF);
    CLOCK_StartTimer(25000U *2, &frsky_bind_cb);
  }
  else {
    frsky1way_init(0);
    seed = 2UL;
    build_data_packet_1way();
    CLOCK_StartTimer(25000U *2, &frsky1waydata_cb);
  }
}


const void * FRSKY1WAY_Cmds(enum ProtoCmds cmd)
{
    switch(cmd) {
        case PROTOCMD_INIT:  initialize(0); return 0;
//        case PROTOCMD_DEINIT:
//        case PROTOCMD_RESET:
//            CLOCK_StopTimer();
//            return (void *)(CC2500_Reset() ? 1L : -1L);
        case PROTOCMD_CHECK_AUTOBIND: return 0; //Never Autobind.
        case PROTOCMD_BIND:  initialize(1); return 0;
        case PROTOCMD_NUMCHAN: return (void *)8L;
        case PROTOCMD_DEFAULT_NUMCHAN: return (void *)8L;
//        case PROTOCMD_CURRENT_ID: return Model.fixed_id ? (void *)((unsigned long)Model.fixed_id % 0x4000) : 0;
        case PROTOCMD_GETOPTIONS: return frsky_opts;
        case PROTOCMD_TELEMETRYSTATE: return (void *)(long) PROTO_TELEM_UNSUPPORTED;
        default: break;
    }
    return 0;
}

