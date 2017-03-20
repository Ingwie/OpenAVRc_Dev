//

#ifndef _COMMON_H_
#define _COMMON_H_

#include "../OpenAVRc.h"

//uint8_t read_bind_sw(void);


#define PROTO_TELEM_UNSUPPORTED -1
#define MODULE_CALLTYPE
//#define UNIMOD 1 // MPX Serial input.
//#define UNIMOD 2 // PPM input capture.

//extern struct Model Model;

//extern volatile uint8_t g_initializing;

struct Transmitter {
    uint8_t dummy;
};

//extern struct Transmitter Transmitter;

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

//extern volatile int16_t Channels[16];

/* Temporary definition until we have real translation */
#define _tr_noop(x) x
#ifdef NO_LANGUAGE_SUPPORT
#define _tr(x) x
#else
const char *_tr(const char *str);
#endif


#define COUNTS_PER_MILLI_SEC (F_CPU/128000L)-1 // value should == 1ms based on F_CPU.


#endif // _COMMON_H_
