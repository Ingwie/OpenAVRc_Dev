/*
 * Authors (alphabetical order)
 *
 * - Rick Kilvington (2018).
 *
 */


#include "../OpenAVRc.h"



ISR(TCF0_OVF_vect)
{
  /*
   * PROTO_PPMSIM uses TCF0 OC0B PF1.
   * 16 Bit Timer running @ 2MHz has a resolution of 0.5us.
   * This should give a PPM resolution of 2048.
  */
  if(*RptrA == 0) { // End of timing events.
    RptrA = &pulses2MHz.pword[PULSES_WORD_SIZE/2];
    // Set the PPM idle level.
    if (g_model.PULSEPOL) {
      TRAINER_PORT.PIN1CTRL |= PORT_INVEN_bm; // Maybe use MPC mask with bm.
    }
    else {
      TRAINER_PORT.PIN1CTRL &= ~PORT_INVEN_bm;
    }
    // Schedule next Mixer calculations.
    SCHEDULE_MIXER_END_IN_US(22500 + ((g_model.PPMFRAMELENGTH * 1000) / 2));
    if(g_model.rfProtocol == PROTOCOL_PPMSIM)
      setupPulsesPPM(PPMSIM);
    else  setupPulsesPPM(PPM16LAST); // PPM16 uses same vector.

    heartbeat |= HEART_TIMER_PULSES;
    TRAINER_TC.CCB = PULSES_SETUP_TIME_US *2;
    TRAINER_TC.PER = (*RptrA++) + TRAINER_TC.CCB;
  }
  else if (*(RptrA +1) == 0) { // Look ahead one timing event.
    TRAINER_TC.PER = (*RptrA++);
    TRAINER_TC.CCB = 0xFFFF; // Prevent compare.
  }
  else {
    TRAINER_TC.CCB = (*RptrA++);
    TRAINER_TC.PER = (*RptrA++) + TRAINER_TC.CCB;
  }

  dt = TRAINER_TC.CNT; // Time since overflow.
  if (dt > g_tmr1Latency_max) g_tmr1Latency_max = dt;
  if (dt < g_tmr1Latency_min) g_tmr1Latency_min = dt;
}


static void PROTO_PPMSIM_reset()
{
#if defined(FRSKY)
  telemetryReset();
#endif

  TRAINER_TC.CTRLA &= ~TC0_CLKSEL_gm; // Stop timer = OFF.
  TRAINER_TC.CTRLFSET = TC_CMD_RESET_gc;
  TRAINER_PORT.PIN1CTRL &= ~PORT_INVEN_bm;
  setup_trainer_tc(); // Restore tc settings.
}

static void PROTO_PPMSIM_initialize()
{
  PPM16_CONF();

#if defined(FRSKY)
  telemetryPPMInit();
#endif

  RptrA = &pulses2MHz.pword[PULSES_WORD_SIZE/2];
  *RptrA = 0;

  TRAINER_TC.CTRLA &= ~TC0_CLKSEL_gm; // Stop timer = OFF.
  TRAINER_TC.CTRLFSET = TC_CMD_RESET_gc;
  TRAINER_TC.INTCTRLA |=  (0b11 << TC0_OVFINTLVL_gp); // Level 3 - High Priority.
  TRAINER_TC.PER = 16000U *2; // Overflow in 16ms.
  TRAINER_TC.CCB = 0xFFFF; // Prevent compare.
  TRAINER_TC.CTRLC &= ~TC0_CMPB_bm; // Clear CMPB level in OFF state.
  TRAINER_TC.CTRLB = TC0_CCBEN_bm | (0b011 << TC0_WGMODE_gp); // Mode = SINGLESLOPE, Enable CCB.
  RF_PORT.DIRSET = PIN1_bm; //
  TRAINER_TC.CTRLA = 8 + 3; // Event channel 3 (prescaler of 16)
}


const void * PROTO_PPMSIM_Cmds(enum ProtoCmds cmd)
{
  switch(cmd) {
    case PROTOCMD_INIT:
      PROTO_PPMSIM_initialize();
    return 0;
    case PROTOCMD_RESET:
      PROTO_PPMSIM_reset();
      return 0;
    case PROTOCMD_GETOPTIONS:
     sendOptionsSettingsPpm();
    return 0;
    default: break;
  }
  return 0;
}
