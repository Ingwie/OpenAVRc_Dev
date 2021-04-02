/*
 * Authors (alphabetical order)
 *
 * - Rick Kilvington (2018).
 *
 */


#include "../OpenAVRc.h"



ISR(TCE0_OVF_vect)
{
  /*
   * PROTO_PPM uses TCE0 Output Compare 0D PE3.
   * 16 Bit Timer running @ 2MHz has a resolution of 0.5us.
   * This should give a PPM resolution of 2048.
  */
  if(*RptrB == 0) { // End of timing events.
    RptrB = &pulses2MHz.pword[0];
    // Set the PPM idle level.
    if (g_model.PULSEPOL) {
      RF_OUT_PIN_CTRL_REG |= PORT_INVEN_bm; // Maybe use MPC mask with bm for USART_TXD_PIN.
    }
    else {
      RF_OUT_PIN_CTRL_REG &= ~PORT_INVEN_bm;
    }
    // Schedule next Mixer calculations.
    SCHEDULE_MIXER_END_IN_US(22500 + ((g_model.PPMFRAMELENGTH * 1000) / 2));
    if(g_model.rfProtocol == PROTOCOL_PPM)
      setupPulsesPPM(PPM);
    else setupPulsesPPM(PPM16FIRST); // PPM16 uses same vector.

    heartbeat |= HEART_TIMER_PULSES;
    RF_TC.CCD = PULSES_SETUP_TIME_US *2;
    RF_TC.PER = (*RptrB++) + RF_TC.CCD;
  }
  else if (*(RptrB +1) == 0) { // Look ahead one timing event.
    RF_TC.PER = (*RptrB++);
    RF_TC.CCD = 0xFFFF; // Prevent compare.
  }
  else {
    RF_TC.CCD = (*RptrB++);
    RF_TC.PER = (*RptrB++) + RF_TC.CCD;
  }

  dt = RF_TC.CNT; // Time since overflow.
  if (dt > g_tmr1Latency_max) g_tmr1Latency_max = dt;
  if (dt < g_tmr1Latency_min) g_tmr1Latency_min = dt;
}


static void PROTO_PPM_reset()
{
  RF_TC.CTRLA &= ~TC0_CLKSEL_gm; // Stop timer = OFF.
  RF_TC.CTRLFSET = TC_CMD_RESET_gc;
  RF_OUT_PIN_CTRL_REG &= ~PORT_INVEN_bm;
  //setup_rf_tc(); // Restore tc settings.
}

static void PROTO_PPM_initialize()
{
#if defined(FRSKY)
  telemetryPPMInit();
#endif

  RptrB = &pulses2MHz.pword[0];
  *RptrB = 0;

  RF_TC.CTRLA &= ~TC0_CLKSEL_gm; // Stop timer = OFF.
  RF_TC.CTRLFSET = TC_CMD_RESET_gc;
  RF_TC.INTCTRLA |= TC_OVFINTLVL_HI_gc; // Level 3 - High Priority.
  RF_TC.PER = 16000U *2; // Overflow in 16ms.
  RF_TC.CCD = 0xFFFF; // Prevent compare.
  RF_TC.CTRLC &= ~TC0_CMPD_bm; // Clear CMPD level in OFF state.
  RF_TC.CTRLB = TC0_CCDEN_bm | (0b011 << TC0_WGMODE_gp); // Mode = SINGLESLOPE, Enable CCD.
  RF_PORT.DIRSET = 1<< RF_OUT_PIN;
  RF_TC.CTRLA = 8 + 1; // Event channel 1 (prescaler of 16)
}


const void * PROTO_PPM_Cmds(enum ProtoCmds cmd)
{
  switch(cmd) {
    case PROTOCMD_INIT:
      PROTO_PPM_initialize();
    return 0;
    case PROTOCMD_RESET:
      PROTO_PPM_reset();
    return 0;
    case PROTOCMD_GETOPTIONS:
     sendOptionsSettingsPpm();
    return 0;
    default: break;
  }
  return 0;
}

