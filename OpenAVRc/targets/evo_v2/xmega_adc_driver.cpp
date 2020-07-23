/*
 * Authors (alphabetical order)
 *
 * - Rick Kilvington (2018).
 *
 */


inline void ADC_atomic(uint8_t Analogs, int16_t sample);

#if !defined(ADCA_SAMPCTRL)
#define ADCA_SAMPCTRL  _SFR_MEM8(0x0208)
#endif
#if !defined(PRODSIGNATURES_ADCACAL0)
#define PRODSIGNATURES_ADCACAL0  _SFR_MEM8(0x0020)
#endif
#if !defined(PRODSIGNATURES_ADCACAL1)
#define PRODSIGNATURES_ADCACAL1  _SFR_MEM8(0x0021)
#endif

#if 0
typedef enum ADC_CURRLIMIT_enum
{
  ADC_CURRLIMIT_NO_gc = (0x00<<5),  /* No current limit,     300ksps max sampling rate */
  ADC_CURRLIMIT_LOW_gc = (0x01<<5),  /* Low current limit,    250ksps max sampling rate */
  ADC_CURRLIMIT_MED_gc = (0x02<<5),  /* Medium current limit, 150ksps max sampling rate */
  ADC_CURRLIMIT_HIGH_gc = (0x03<<5),  /* High current limit,   50ksps max sampling rate */
} ADC_CURRLIMIT_t;
#endif

void adcInit()
{
  PORTCFG.MPCMASK = 0xff;
  PORTA.PIN0CTRL =  PORT_ISC_INPUT_DISABLE_gc;
  PORTCFG.MPCMASK = 0x3f;
  PORTB.PIN0CTRL =  PORT_ISC_INPUT_DISABLE_gc;


// ADCA.CAL = (uint16_t) (PRODSIGNATURES_ADCACAL1 << 8) | PRODSIGNATURES_ADCACAL0; // Read from signature row.
  ADCA.CALL = PRODSIGNATURES_ADCACAL0;
  ADCA.CALH = PRODSIGNATURES_ADCACAL1;

  ADCA.CTRLA = ADC_ENABLE_bm; // Enable ADC.

  ADCA.CTRLB = ADC_CONMODE_bm; // Signed Mode.
  ADCA.CTRLB |= ADC_CURRLIMIT_NO_gc; // No current limit, max sampling rate 300k samples / sec.
  ADCA.CTRLB |= ADC_RESOLUTION_12BIT_gc; // 12 Bit right adjusted result.

  ADCA.PRESCALER = ADC_PRESCALER_DIV32_gc; // DIV32 = 1MHz Clock (Max is 1.8 MHz).

  ADCA.REFCTRL = ADC_BANDGAP_bm;

  ADCA_SAMPCTRL = 63; //0x3F; // SAMPCTRL = 0x3F = 63  Sampling time = (63+1)*(ClkADC/2).

  ADCA.CH0.INTFLAGS |= ADC_CH_CHIF_bm; // Clear flag.
  ADCA.CH0.CTRL |= ADC_CH_START_bm; // Start Conversion.
}

/*
 * Typically the potentiometers used on Joystick Gimbals produce a voltage change that is a fraction of the applied voltage.
 * For 60 degrees of movement there might be a range of about 1/4 of the applied voltage.
 * For the above reason I would like to use the gain stages in the xmega ADC.
 * However the gain stages have so far proved to be unreliable. Although they increase gain, they seem to increase
 * noise by a substantial amount. I will try to use them again one day.
 * So to increase dynamic range we can oversample and decimate (or average).
 * The general rule is that to get an extra n bits of precision, we need to oversample by x times. e.g.
 * x = 2^2n
 * So 4x oversampling would give 1 extra bit of resolution (decimation or divide by 2).
 * 16x samples would give an extra 2 bits (decimate by /4).
 *
 * The simplest way to implement a flexible gain system is to :-
 * 1. Don't use the gain stages.
 * 2. Have a fixed decimation of /2.
 * 3. Use the #define NUMBERSAMPLES to increase dynamic range. Values in the range 4 to 8 should be okay.
 * Try not to overload the int16_t Sample.
*/


void getADC()
{
  if(! ADCA.CH0.INTFLAGS & ADC_CH_CHIF_bm) return; // Conversion running.

  int16_t Sample;
  uint8_t adc_output = NUMBER_ANALOG;

  #define NUMBERSAMPLES 7

  while(adc_output) {

    uint8_t over_samples = NUMBERSAMPLES;

    switch (--adc_output) {
      case STICK_RH:
      case STICK_LV:
      case STICK_RV:
      case STICK_LH:

        // Differential measurement with gain. Gain = 4. VREF = 3.3V/1.6 = 2.0625V
        // VINN = AVCC_REF_N = 3.3V/2 connected to PINADC4.
        // Measured range = 3285 counts.

/*
        // File iox256d3.h is wrong. Use this typedef.
        typedef enum ADC_CH_MUXNEG_enum
        {
            ADC_CH_MUXNEG_PIN4_gc = (0x00<<0),  // Input pin 4.
            ADC_CH_MUXNEG_PIN5_gc = (0x01<<0),  // Input pin 5.
            ADC_CH_MUXNEG_PIN6_gc = (0x02<<0),  // Input pin 6.
            ADC_CH_MUXNEG_PIN7_gc = (0x03<<0),  // Input pin 7.
        } ADC_CH_MUXNEG_t;

        // Reference voltage selection.
        typedef enum ADC_REFSEL_enum
        {
            ADC_REFSEL_INT1V_gc = (0x00<<4),  // Internal 1V.
            ADC_REFSEL_INTVCC_gc = (0x01<<4),  // Internal VCC / 1.6.
            ADC_REFSEL_AREFA_gc = (0x02<<4),  // External reference on PORT A.
            ADC_REFSEL_AREFB_gc = (0x03<<4),  // External reference on PORT B.
            ADC_REFSEL_INTVCC2_gc = (0x04<<4),  // Internal VCC / 2.
        } ADC_REFSEL_t;
*/

        ADCA.REFCTRL = ADC_REFSEL_AREFA_gc | ADC_BANDGAP_bm; //

        ADCA.CH0.MUXCTRL = 0b000 << ADC_CH_MUXNEG_gp; // PIN0
        ADCA.CH0.MUXCTRL |= adc_output << ADC_CH_MUXPOS_gp;
        ADCA.CH0.CTRL = ADC_CH_INPUTMODE_DIFF_gc; //

        Sample = 0;
        while(over_samples--) {
          ADCA.CH0.INTFLAGS |= ADC_CH_CHIF_bm; // Clear flag.
          ADCA.CH0.CTRL |= ADC_CH_START_bm; // Start Conversion.
            while(! ADCA.CH0.INTFLAGS & ADC_CH_CHIF_bm);
          Sample += ADCA.CH0.RES;
        }

        Sample /= 2;
        ADC_atomic(adc_output, Sample);

        break;

      case POT1:
      case POT2:
      case ANAL_SWS_1:
      case ANAL_SWS_2:
      case ANAL_SWS_3:
      case ANAL_SWS_4:
      case KEYBOARD:

        // Differential measurement. VREF = AREFA (Anal' 3V3 /2).
        // VINN = PINADC ... also AREFA.

        ADCA.REFCTRL = ADC_REFSEL_AREFA_gc | ADC_BANDGAP_bm;

        ADCA.CH0.CTRL = ADC_CH_INPUTMODE_DIFF_gc; //

        ADCA.CH0.MUXCTRL = 0b000 << ADC_CH_MUXNEG_gp; // PIN0
        ADCA.CH0.MUXCTRL |=  adc_output << ADC_CH_MUXPOS_gp;

        ADCA.CH0.INTFLAGS |= ADC_CH_CHIF_bm; // Clear flag.
        ADCA.CH0.CTRL |= ADC_CH_START_bm; // Start Conversion.
        while(! ADCA.CH0.INTFLAGS & ADC_CH_CHIF_bm);
        // Discard result due to ADC having auto-zero issues when previous conversion is at max and min voltages e.g. 3V3 and OV.
        // ADC_atomic(adc_output, ADCA.CH0.RES); // = (signed) ADCA.CH0.RES + 2048L;

        ADCA.CH0.INTFLAGS |= ADC_CH_CHIF_bm; // Clear flag.
        ADCA.CH0.CTRL |= ADC_CH_START_bm; // Start Conversion.
        while(! ADCA.CH0.INTFLAGS & ADC_CH_CHIF_bm);
        ADC_atomic(adc_output, ADCA.CH0.RES); // = (signed) ADCA.CH0.RES + 2048L;

        break;


      case TX_VOLTAGE:

        // Differential measurement. VREF = 1.00V derived from internal Bandgap.
        // VINN = Pad GND

        ADCA.REFCTRL = ADC_REFSEL_INT1V_gc | ADC_BANDGAP_bm;

        ADCA.CH0.CTRL = ADC_CH_INPUTMODE_DIFF_gc; //

        ADCA.CH0.MUXCTRL = 0b101 << ADC_CH_MUXNEG_gp; // Pad GND
        ADCA.CH0.MUXCTRL |= TX_VOLTAGE << ADC_CH_MUXPOS_gp;

        ADCA.CH0.INTFLAGS |= ADC_CH_CHIF_bm; // Clear flag.
        ADCA.CH0.CTRL |= ADC_CH_START_bm; // Start Conversion.
        while(! ADCA.CH0.INTFLAGS & ADC_CH_CHIF_bm);
        ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
          s_anaFilt[TX_VOLTAGE] = (signed) ADCA.CH0.RES;
        }
        break;

      case GAIN_REF_N:

        // Differential measurement of ADC5 and ADC5 to test gain stage.

        ADCA.REFCTRL = ADC_REFSEL_AREFA_gc | ADC_BANDGAP_bm; //

        ADCA.CH0.MUXCTRL = 0b001 << ADC_CH_MUXNEG_gp; // ADC5
        ADCA.CH0.MUXCTRL |= adc_output << ADC_CH_MUXPOS_gp;
        ADCA.CH0.CTRL = ADC_CH_INPUTMODE_DIFFWGAIN_gc | ADC_CH_GAIN_4X_gc; //

        ADCA.CH0.INTFLAGS |= ADC_CH_CHIF_bm; // Clear flag.
        ADCA.CH0.CTRL |= ADC_CH_START_bm; // Start Conversion.
        while(! ADCA.CH0.INTFLAGS & ADC_CH_CHIF_bm);

        // Discard result due to ADC having auto-zero issues when previous conversion is at max and min voltages e.g. 3V3 and OV.
        // ADC_atomic(adc_output, ADCA.CH0.RES); // = (signed) ADCA.CH0.RES + 2048L;

        ADCA.CH0.INTFLAGS |= ADC_CH_CHIF_bm; // Clear flag.
        ADCA.CH0.CTRL |= ADC_CH_START_bm; // Start Conversion.
        while(! ADCA.CH0.INTFLAGS & ADC_CH_CHIF_bm);
        ADC_atomic(adc_output, ADCA.CH0.RES); // = (signed) ADCA.CH0.RES + 2048L;
        break;

      case AREFA:
        // ADC External Reference Voltage. Connected to VCC /2 via circuitry.
        s_anaFilt[AREFA] = 0x0EFA;
        break;
    }
  }
}


inline void ADC_atomic(uint8_t Analogs, int16_t sample)
{
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {

    if (Analogs == STICK_RH)
#if defined(INV_STICK_RH)
      s_anaFilt[STICK_RH] = (signed) -sample + 0x800;
#else
      s_anaFilt[STICK_RH] = (signed) sample + 0x800;
#endif

    else if (Analogs == STICK_LV)
#if defined(INV_STICK_LV)
      s_anaFilt[STICK_LV] = (signed) -sample + 0x800;
#else
      s_anaFilt[STICK_LV] = (signed) sample + 0x800;
#endif

    else if (Analogs == STICK_RV)
#if defined(INV_STICK_RV)
      s_anaFilt[STICK_RV] = (signed) -sample + 0x800;
#else
      s_anaFilt[STICK_RV] = (signed) sample + 0x800;
#endif

    else if (Analogs == STICK_LH)
#if defined(INV_STICK_LH)
      s_anaFilt[STICK_LH] = (signed) -sample + 0x800;
#else
      s_anaFilt[STICK_LH] = (signed) sample + 0x800;
#endif
    else
      s_anaFilt[Analogs] = (signed) sample + 0x800;
  }
}

