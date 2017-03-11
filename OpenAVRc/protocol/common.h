//

#ifndef _COMMON_H_
#define _COMMON_H_

#include "../OpenAVRc.h"

uint8_t read_bind_sw(void);


#define PROTO_TELEM_UNSUPPORTED -1
#define MODULE_CALLTYPE
//#define UNIMOD 1 // MPX Serial input.
//#define UNIMOD 2 // PPM input capture.

enum PROTO_MODE {
BIND_MODE,
NORMAL_MODE,
RANGE_MODE,
};


enum TxPower {
    TXPOWER_0,
    TXPOWER_1,
    TXPOWER_2,
    TXPOWER_3,
    TXPOWER_4,
    TXPOWER_5,
    TXPOWER_6,
    TXPOWER_7,
};

enum {
    CYRF6936,
    A7105,
    CC2500,
    NRF24L01,
    MULTIMOD,
    TX_MODULE_LAST,
};

/* Protocol */
#define PROTODEF(proto, module, map, init, name) proto,
enum DevoProtocols {
    PROTOCOL_NONE,
    #include "protocol.h"
    PROTOCOL_COUNT,
};
#undef PROTODEF

//extern const uint8_t *ProtocolChannelMap[PROTOCOL_COUNT];
//extern const char * const ProtocolNames[PROTOCOL_COUNT];

struct Model {
//    char name[24];
//    char icon[24];
//    enum ModelType type;
	enum DevoProtocols protocol;
#define NUM_PROTO_OPTS 4
	int16_t proto_opts[NUM_PROTO_OPTS];
    uint8_t num_channels;
//    uint8_t num_ppmin;
//   uint16_t ppmin_centerpw;
//    uint16_t ppmin_deltapw;
//    uint8_t train_sw;
//    int8_t ppm_map[MAX_PPM_IN_CHANNELS];
    uint32_t fixed_id;
    enum TxPower tx_power;
//    enum SwashType swash_type;
//    uint8_t swash_invert;
//    uint8_t swashmix[3];
//    struct Trim trims[NUM_TRIMS];
//    struct Mixer mixers[NUM_MIXERS];
//    struct Limit limits[NUM_OUT_CHANNELS];
//    char virtname[NUM_VIRT_CHANNELS][VIRT_NAME_LEN];
//    struct Timer timer[NUM_TIMERS];
//    uint8_t templates[NUM_CHANNELS];
//    struct PageCfg2 pagecfg2;
//    uint8_t safety[NUM_SOURCES+1];
//    uint8_t telem_alarm[TELEM_NUM_ALARMS];
//    uint16_t telem_alarm_val[TELEM_NUM_ALARMS];
//    uint8_t telem_flags;
//    MixerMode mixer_mode;
//    uint32_t permanent_timer;
//#if HAS_DATALOG
//    struct datalog datalog;
//#endif
};


extern struct Model Model;

extern volatile uint8_t g_initializing;

struct Transmitter {
    uint8_t dummy;
};

extern struct Transmitter Transmitter;

// #define NULL ((void*) 0)

#define NUM_OUT_CHANNELS 7

//MAX = +10000
//MIN = -10000
// #define CHAN_MULTIPLIER 100

// For Multiplex M-Link & PPM
#define CHAN_MAX_VALUE (1520L)
#define CHAN_MIN_VALUE (-CHAN_MAX_VALUE)
#define DELTA_PPM_IN 550 // 500 for Most systems or 550 for Multiplex. e.g. 1500 +/- 550 us
//#define CHAN_MAX_VALUE (+1520 * 7)
//#define CHAN_MIN_VALUE (-1520 * 7) // Range of -1521 to +1520.

extern volatile int16_t Channels[16];

/* Temporary definition until we have real translation */
#define _tr_noop(x) x
#ifdef NO_LANGUAGE_SUPPORT
#define _tr(x) x
#else
const char *_tr(const char *str);
#endif

#if   F_CPU == 16000000
#define HALF_MICRO_SEC_COUNTS(half_us) (half_us)
#elif F_CPU == 32000000
#define HALF_MICRO_SEC_COUNTS(half_us) (half_us *2)
#else
#define HALF_MICRO_SEC_COUNTS(half_us) (((F_CPU/800)*(half_us))/20000)
#endif


#define COUNTS_PER_MILLI_SEC (F_CPU/128000L)-1 // value should == 1ms based on F_CPU.

#define gpio_set(sfr, bit)   (sfr) |=   1<<(bit)
#define gpio_clear(sfr, bit) (sfr) &= ~(1<<(bit))

//Port C
#define SPI_CS		3
#define CH_ORD          4
#define	BIND_SW		5

//Port B
#define PPM_IN	  0
#define DEBUG_1	  1
#define DEBUG_2   2

#define CS_HI() 		gpio_set(PORTC, SPI_CS)
#define CS_LO() 		gpio_clear(PORTC, SPI_CS)

//Port D
#define RF_EN 7  // MPX RF Enable signal.

#define RF_EN_STATE()	gpio_get(GPIOD, RF_EN)

#endif // _COMMON_H_
