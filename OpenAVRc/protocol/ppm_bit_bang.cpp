
#include "../OpenAVRc.h"

/*
 * 16 Bit Timer running @ 16MHz has a resolution of 0.5us.
 * This should give a PPM resolution of 2048.
*/
static uint16_t ppm_bb_cb()
{
  if ( *((uint16_t*)pulses2MHzRPtr) == 0) {

    if (g_model.pulsePol) PORTB &= ~(1<<OUT_B_PPM); // Set idle level.
    else PORTB |= (1<<OUT_B_PPM); // GCC optimisation should produce a single SBI instruction.

    // Schedule next Mixer calculations.
    SCHEDULE_MIXER_END(45*8+g_model.ppmFrameLength*8);

    setupPulsesPPM(PROTO_PPM);
    heartbeat |= HEART_TIMER_PULSES;
    return SETUP_PULSES_DURATION *2;
  }

  dt = TCNT1L; // Record Timer1 latency for DEBUG stats display.

  PORTB ^= (1<<OUT_B_PPM); // Toggle port bit.

  uint16_t temp = *((uint16_t *) pulses2MHzRPtr);
  pulses2MHzRPtr += sizeof(uint16_t); // Non PPM protocols use uint8_t pulse buffer.
  return temp; // Schedule next Timer1 interrupt vector (to this function).
}


static void initialize()
{
  pulses2MHzRPtr = pulses2MHz;
  *((uint16_t*) pulses2MHzRPtr) = 0;
  CLOCK_StartTimer(1000 *2, &ppm_bb_cb);
}


const void * PPM_BB_Cmds(enum ProtoCmds cmd)
{
    switch(cmd) {
        case PROTOCMD_INIT: initialize(); return 0;
        case PROTOCMD_DEINIT:
        case PROTOCMD_RESET:
            CLOCK_StopTimer();
            return (void *) 1L;
//        case PROTOCMD_CHECK_AUTOBIND: return 0;
//        case PROTOCMD_BIND:  initialize(); return 0;
//        case PROTOCMD_NUMCHAN: return (void *)((unsigned long) NUM_OUT_CHANNELS);
//        case PROTOCMD_DEFAULT_NUMCHAN: return (void *) 6L;
/*        case PROTOCMD_GETOPTIONS:
            if (Model.proto_opts[CENTER_PW] == 0) {
                Model.proto_opts[CENTER_PW] = 1100;
                Model.proto_opts[DELTA_PW] = 500;
                Model.proto_opts[NOTCH_PW] = 400;
                Model.proto_opts[PERIOD_PW] = 22500;
            }
            return ppm_opts;
*/
//        case PROTOCMD_TELEMETRYSTATE: return (void *)(long) PROTO_TELEM_UNSUPPORTED;
        default: break;
    }
    return 0;
}

