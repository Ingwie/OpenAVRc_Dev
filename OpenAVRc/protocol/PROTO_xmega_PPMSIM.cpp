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
   * PROTO_PPMSIM uses TCF0 Output Compare 0D PF3.
   * 16 Bit Timer running @ 2MHz has a resolution of 0.5us.
   * This should give a PPM resolution of 2048.
  */
  if(*RptrA == 0) { // End of timing events.
    RptrA = &pulses2MHz.pword[PULSES_WORD_SIZE/2];
    // Set the PPM idle level.
    if (g_model.PULSEPOL) {
      TRN_PULSES_OUT_PIN_CTRL_REG |= PORT_INVEN_bm; // Maybe use MPC mask with bm.
    }
    else {
      TRN_PULSES_OUT_PIN_CTRL_REG &= ~PORT_INVEN_bm;
    }

    if(g_model.rfProtocol == PROTOCOL_PPMSIM)
    {
      // Schedule next Mixer calculations.
      SCHEDULE_MIXER_END_IN_US(22500 + ((g_model.PPMFRAMELENGTH * 1000) / 2));
      setupPulsesPPM(PPMSIM);
    }
    else  setupPulsesPPM(PPM16LAST); // PPM16 uses same vector.

    heartbeat |= HEART_TIMER_PULSES;
    PULSES_OUT_TC.CCD = PULSES_SETUP_TIME_US *2;
    PULSES_OUT_TC.PER = (*RptrA++) + PULSES_OUT_TC.CCD;
  }
  else if (*(RptrA +1) == 0) { // Look ahead one timing event.
    PULSES_OUT_TC.PER = (*RptrA++);
    PULSES_OUT_TC.CCD = 0xFFFF; // Prevent compare.
  }
  else {
    PULSES_OUT_TC.CCD = (*RptrA++);
    PULSES_OUT_TC.PER = (*RptrA++) + PULSES_OUT_TC.CCD;
  }

  dt = PULSES_OUT_TC.CNT; // Time since overflow.
  if (dt > g_tmr1Latency_max) g_tmr1Latency_max = dt;
  if (dt < g_tmr1Latency_min) g_tmr1Latency_min = dt;
}


static void PROTO_PPMSIM_reset()
{
  PULSES_OUT_TC.CTRLA &= ~TC0_CLKSEL_gm; // Stop timer = OFF.
  PULSES_OUT_TC.CTRLFSET = TC_CMD_RESET_gc;
  TRN_PULSES_OUT_PIN_CTRL_REG &= ~PORT_INVEN_bm;
  TRAINER_PORT.DIRCLR = 1 << TRN_PULSES_OUT_PIN; //
  setup_trainer_tc(); // Restore tc settings.
  WAIT_PUPIL();
}

static void PROTO_PPMSIM_initialize()
{
  PPM16_CONF();

#if defined(FRSKY)
  telemetryPPMInit();
#endif

  RptrA = &pulses2MHz.pword[PULSES_WORD_SIZE/2];
  *RptrA = 0;

  PULSES_OUT_TC.CTRLA &= ~TC0_CLKSEL_gm; // Stop timer = OFF.
  PULSES_OUT_TC.CTRLFSET = TC_CMD_RESET_gc;
  PULSES_OUT_TC.INTCTRLA |=  (0b11 << TC0_OVFINTLVL_gp); // Level 3 - High Priority.
  PULSES_OUT_TC.PER = 16000U *2; // Overflow in 16ms.
  PULSES_OUT_TC.CCD = 0xFFFF; // Prevent compare.
  PULSES_OUT_TC.CTRLC &= ~TC0_CMPC_bm; // Clear CMPc level in OFF state.
  PULSES_OUT_TC.CTRLB = TC0_CCDEN_bm | (0b011 << TC0_WGMODE_gp); // Mode = SINGLESLOPE, Enable CCD.
  PULSES_OUT_TC.CTRLA = 8 + 1; // Event channel 1 (prescaler of 16)
  TRAINER_PORT.DIRSET = 1 << TRN_PULSES_OUT_PIN; //
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

