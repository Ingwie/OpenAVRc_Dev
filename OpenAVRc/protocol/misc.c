/*
*/

#include "../OpenAVRc.h"
#include "common.h"
#include "interface.h"

extern volatile uint8_t g_sync_count;
//extern volatile uint16_t g_entropy;
extern volatile uint32_t bind_press_time;

uint16_t (*timer_callback)(void);
const void * (*PROTO_Cmds)(enum ProtoCmds);

static volatile uint32_t msecs = 0;



void CLOCK_StartTimer(uint16_t half_us, uint16_t (*cb)(void))
{
  if(! cb) return;
  if(! half_us) return;
  timer_callback = cb; // timer_callback = pointer to function.

  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
	OCR1A = TCNT1 + half_us;
  }
  // Setup Timer 1.
  // Normal mode (0), OVF @ TOP (0xFFFF), F_CPU/8.
  TCCR1A = 0; // Disconnect Commutation Outputs.
  TCCR1B = (0b10 << CS10);

  TIFR1 |= 1<<OCF1A; // Reset Flag.
  TIMSK1 |= 1<<OCIE1A; // Enable Output Compare interrupt.
}


void CLOCK_StopTimer()
{
  TCCR1B &= ~(0b111 << CS10); // Stop Counter.
  timer_callback = NULL;
  TIMSK1 &= ~(1<<OCIE1A); // Disable Output Compare interrupt.
  TIFR1 |= 1<<OCF1A; // Reset Flag.
}


#if 0
ISR(TIMER1_CAPT_vect)
{
// One timer count ~723 nS @ 11.0592MHz / 8.
uint16_t icr1_diff;
uint16_t icr1_current;

static uint16_t icr1_previous = 0;
static uint8_t servo_count = 0;
static uint8_t need_to_sync = 1;

icr1_current = ICR1;

//icr1_diff = icr1_current - icr1_previous;
if(icr1_current >= icr1_previous) icr1_diff = icr1_current - icr1_previous;
else icr1_diff = (0xffff - icr1_previous) + icr1_current + 1 ;

icr1_previous = icr1_current;

	if (icr1_diff > MICRO_SEC_CONVERT(2300)) // > 2.3ms pulse seen as frame sync.
	{
	need_to_sync =0;
	g_sync_count ++;
	servo_count =0;
   	}
	else if (icr1_diff < MICRO_SEC_CONVERT(700)) // < 0.7ms pulse seen as glitch.
	{
	// Do nothing with glitch.
	}
	else if (! need_to_sync) // Pulse within limits and we don't need to sync.
	{
		if (servo_count < NUM_OUT_CHANNELS)
		{
		if (icr1_diff > MICRO_SEC_CONVERT(1500 + DELTA_PPM_IN)) icr1_diff = MICRO_SEC_CONVERT(1500 + DELTA_PPM_IN);
   		else if (icr1_diff < MICRO_SEC_CONVERT(1500 - DELTA_PPM_IN)) icr1_diff = MICRO_SEC_CONVERT(1500 - DELTA_PPM_IN);

		// Subtract 1.5 ms centre offset.
		// Multiply by 2 to get max-min counter value difference to be +-1520
		// (same scaling as M-Link Packet for MPX ppm (+-550us) range !).

   		Channels[servo_count] = (icr1_diff - MICRO_SEC_CONVERT(1500)) * 2;
   		servo_count++;
		}
		else need_to_sync = 1; // More servo pulses than we can handle ... need to sync.
	}
// if (g_entropy==0) g_entropy = TCNT1;
}
#endif


uint32_t CLOCK_getms()
{
  uint32_t ms;

  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    ms = msecs;
  }
  return ms;
}


void CLOCK_delayms(uint32_t delay_ms)
{
  uint32_t start_ms;

  start_ms = msecs;
  while(msecs < (start_ms + delay_ms));
}


void PROTOCOL_SetBindState(uint32_t msec)
{
  uint32_t bind_time;

  if(msec) {
    SpiRFModule.mode = BIND_MODE;
    if (msec == 0xFFFFFFFF) bind_time = msec;
    else bind_time = CLOCK_getms() + msec;
  }
  else SpiRFModule.mode = NORMAL_MODE; // unimod rick added. Can't go from bind to range test.
}

