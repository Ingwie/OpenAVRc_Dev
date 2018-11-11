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
  PORTCFG.MPCMASK = 0x1f;
  PORTB.PIN0CTRL =  PORT_ISC_INPUT_DISABLE_gc;

  ADCA.CALL = (uint16_t) (PRODSIGNATURES_ADCACAL1 << 8) | PRODSIGNATURES_ADCACAL0; // Read from signature row.

  ADCA.CTRLA = ADC_ENABLE_bm; // Enable ADC.

  ADCA.CTRLB = ADC_CONMODE_bm; // Signed Mode.
  ADCA.CTRLB |= ADC_CURRLIMIT_NO_gc; // High current limit, max sampling rate 75k samples/sec.
  ADCA.CTRLB |= ADC_RESOLUTION_12BIT_gc; // 12 Bit right adjusted result .

  ADCA.PRESCALER = ADC_PRESCALER_DIV32_gc; // DIV32 = 1MHz Clock.

  ADCA.REFCTRL = ADC_BANDGAP_bm;

  ADCA_SAMPCTRL = 0x3F; // SAMPCTRL = 0x3F = 63  Sampling time = (63+1)*(ClkADC/2).

  ADCA.CH0.CTRL |= ADC_CH_START_bm; // Start Conversion to set flag for first time.
  while(! ADCA.INTFLAGS & ADC_CH_CHIF_bm);
}


void getADC()
{
  int16_t OverSample;

  if(! ADCA.INTFLAGS & ADC_CH_CHIF_bm) return;
  for(uint8_t adc_output = STICK1; adc_output < NUMBER_ANALOG; adc_output++) {

    switch (adc_output) {
      case STICK1:
      case STICK2:
      case STICK3:
      case STICK4:

        // Differential measurement with gain. Gain = 4. VREF = 3.3V/1.6 = 2.0625V
        // VINN = AVCC_REF_N = 3.3V/2 connected to PINADC4.
        // Measured range = 3285 counts.

         typedef enum ADC_CH_MUXNEG_enum  // File iox256d3.h is wrong. Use this typdef.
        {
            ADC_CH_MUXNEG_PIN4_gc = (0x00<<0),  /* Input pin 4 */
            ADC_CH_MUXNEG_PIN5_gc = (0x01<<0),  /* Input pin 5 */
            ADC_CH_MUXNEG_PIN6_gc = (0x02<<0),  /* Input pin 6 */
            ADC_CH_MUXNEG_PIN7_gc = (0x03<<0),  /* Input pin 7 */
        } ADC_CH_MUXNEG_t;


        ADCA.REFCTRL = ADC_REFSEL_INTVCC_gc | ADC_BANDGAP_bm; //

        ADCA.CH0.CTRL = ADC_CH_GAIN_4X_gc; //
        ADCA.CH0.CTRL |= ADC_CH_INPUTMODE_DIFFWGAIN_gc; //

        ADCA.CH0.MUXCTRL = 0b000 << ADC_CH_MUXNEG_gp; // PIN4
        ADCA.CH0.MUXCTRL |= adc_output << ADC_CH_MUXPOS_gp;

        ADCA.CH0.INTFLAGS |= ADC_CH_CHIF_bm; // Clear flag.
        ADCA.CH0.CTRL |= ADC_CH_START_bm; // Start Conversion.
        while(! ADCA.INTFLAGS & ADC_CH_CHIF_bm);
        OverSample = ADCA.CH0.RES;

        ADCA.CH0.INTFLAGS |= ADC_CH_CHIF_bm; // Clear flag.
        ADCA.CH0.CTRL |= ADC_CH_START_bm; // Start Conversion.
        while(! ADCA.INTFLAGS & ADC_CH_CHIF_bm);
        OverSample += ADCA.CH0.RES;

        ADCA.CH0.INTFLAGS |= ADC_CH_CHIF_bm; // Clear flag.
        ADCA.CH0.CTRL |= ADC_CH_START_bm; // Start Conversion.
        while(! ADCA.INTFLAGS & ADC_CH_CHIF_bm);
        OverSample += ADCA.CH0.RES;

        ADCA.CH0.INTFLAGS |= ADC_CH_CHIF_bm; // Clear flag.
        ADCA.CH0.CTRL |= ADC_CH_START_bm; // Start Conversion.
        while(! ADCA.INTFLAGS & ADC_CH_CHIF_bm);
        OverSample += ADCA.CH0.RES;

        ADC_atomic(adc_output, OverSample /4);

        break;

      case POT1:
      case POT2:
      case ANAL_SWS_1:
      case ANAL_SWS_2:
      case ANAL_SWS_3:
      case ANAL_SWS_4:

        // Differential measurement with gain. Gain = 1. VREF = 3.3V/1.6 = 2.0625V
        // VINN = AVCC_REF_N = 3.3V/2 connected to PINADC4.
        // Measured range = 3282 counts.

        ADCA.REFCTRL = ADC_REFSEL_INTVCC_gc | ADC_BANDGAP_bm;

        ADCA.CH0.CTRL = ADC_CH_GAIN_1X_gc; //
        ADCA.CH0.CTRL |= ADC_CH_INPUTMODE_DIFFWGAIN_gc; //

        ADCA.CH0.MUXCTRL = 0b000 << ADC_CH_MUXNEG_gp; // PIN4
        ADCA.CH0.MUXCTRL |=  adc_output << ADC_CH_MUXPOS_gp;

        ADCA.CH0.INTFLAGS |= ADC_CH_CHIF_bm; // Clear flag.
        ADCA.CH0.CTRL |= ADC_CH_START_bm; // Start Conversion.
        while(! ADCA.INTFLAGS & ADC_CH_CHIF_bm);
        ADC_atomic(adc_output, ADCA.CH0.RES); // = (signed) ADCA.CH0.RES + 2048L;
        break;

      case TX_VOLTAGE:

        // Differential measurement with gain. Gain = 1. VREF = 1.00V derived from Bandgap.
        // VINN = Pad GND

        ADCA.REFCTRL = ADC_REFSEL_INT1V_gc | ADC_BANDGAP_bm;
        ADCA.CH0.CTRL = ADC_CH_GAIN_1X_gc; //
        ADCA.CH0.CTRL |= ADC_CH_INPUTMODE_DIFFWGAIN_gc; //
        ADCA.CH0.MUXCTRL = 0b111 << ADC_CH_MUXNEG_gp; // Pad GND
        ADCA.CH0.MUXCTRL |= TX_VOLTAGE << ADC_CH_MUXPOS_gp;

        ADCA.CH0.INTFLAGS |= ADC_CH_CHIF_bm; // Clear flag.
        ADCA.CH0.CTRL |= ADC_CH_START_bm; // Start Conversion.
        while(! ADCA.INTFLAGS & ADC_CH_CHIF_bm);
        ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
          s_anaFilt[TX_VOLTAGE] = (signed) ADCA.CH0.RES;
        }
        break;

      case AVCC_REF_N:
        // ADC input only used for VINN when performing differential input conversions. Connected to VCC /2 externally.
        s_anaFilt[AVCC_REF_N] = 0xD1FF;
        break;

      case KEYBOARD:

        s_anaFilt[KEYBOARD] = 0xCEB0; // ToDo
        break;
    }
  }
}

inline void ADC_atomic(uint8_t Analogs, int16_t sample)
{
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    s_anaFilt[Analogs] = (signed) sample + 0x4000;
  }
}


