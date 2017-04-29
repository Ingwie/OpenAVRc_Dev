 /*
 **************************************************************************
 *                                                                        *
 *                 ____                ___ _   _____                      *
 *                / __ \___  ___ ___  / _ | | / / _ \____                 *
 *               / /_/ / _ \/ -_) _ \/ __ | |/ / , _/ __/                 *
 *               \____/ .__/\__/_//_/_/ |_|___/_/|_|\__/                  *
 *                   /_/                                                  *
 *                                                                        *
 *              This file is part of the OpenAVRc project.                *
 *                                                                        *
 *                         Based on code(s) named :                       *
 *             OpenTx - https://github.com/opentx/opentx                  *
 *             Deviation - https://www.deviationtx.com/                   *
 *                                                                        *
 *                Only AVR code here for visibility ;-)                   *
 *                                                                        *
 *   OpenAVRc is free software: you can redistribute it and/or modify     *
 *   it under the terms of the GNU General Public License as published by *
 *   the Free Software Foundation, either version 2 of the License, or    *
 *   (at your option) any later version.                                  *
 *                                                                        *
 *   OpenAVRc is distributed in the hope that it will be useful,          *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of       *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        *
 *   GNU General Public License for more details.                         *
 *                                                                        *
 *       License GPLv2: http://www.gnu.org/licenses/gpl-2.0.html          *
 *                                                                        *
 **************************************************************************
*/


#include "../../i2c_master.h"
#include "evo_mega2560.h"

bool read_keyb_matrix(void);
void read_A7_A9(void);
void read_A8_A10(void);

#ifndef SIMU
inline void boardInit()
{
  DDRA  = 0b00000000;// Trim Button Inputs ...
  PORTA = 0b11111111; // ... with Pullup.
  DDRB  = (1<<OUT_B_PPM) | OUT_B_SIM_CTL | 0b00000111; // uSDCARD [3:MISO 2:MOSI 1:SCK 0:CS]
  PORTB = 1<<3; // Pullup on MISO - No latch-up because MISO connects via 1K0.
  DDRC  = OUT_C_RF_CS_N | OUT_C_PWR_LED | OUT_C_PWR_HOLD;
  PORTC = INP_C_PWR_STATUS | OUT_C_PWR_HOLD; // Input pullup on pwr_sw_status, power_hold = 1.
  // DDRD
  DDRE  = 0;
  PORTE = INP_E_ROT_ENC_LH_INT | INP_E_ROT_ENC_LH_DIR | INP_E_ROT_ENC_RH_INT | INP_E_ROT_ENC_RH_DIR;
  // DDRF
  // DDRG
  DDRH  = OUT_H_Speaker;
  DDRJ  = 0; // All inputs
  PORTJ = IO_J_MPX_RF_EN; // Pullup on mpx_rf_en
  DDRK  = 0;
  PORTK = INP_K_KEYB_COL_C | INP_K_KEYB_COL_B | INP_K_KEYB_COL_A; // Pullups on keyboard matrix inputs.
  DDRL  = OUT_L_CPPM_TRN;
//todo  DDRL  = OUT_L_CPPM_OC5A | OUT_L_CPPM_TRN;
  PORTL = INP_L_PPM_IN; // Pullup on input capture.

  adcInit();
  getADC(); // Best to get some values before we start.

  // Setup Timer / Counter 2 as a 10m sec Timer.
  // Prescaler is 1024, F_CPU is 16MHz. Resulting in a clock frequency of 15625Hz.
  // We then divide by 156 to give a period of 9.984m sec ...
  // but correct the error with a 1 in 4 division of 157.
  // Over 4 cycles we are accurate @ 40m sec.
  TCCR2A = 0; // Mode 0 (Normal).
  //TCCR2A = (1<<COM2A0); // Toggle PB4 OC2A.
  TCCR2B = (0b111 << CS20); // Divide by 1024.
  TIFR2 = (1<<OCF2A) | (1<<TOV2); //Clear flags.
  TCNT2 = 0;
  OCR2A = 157;
  TIMSK2 = (1<<OCIE2A) |  (1<<TOIE2); // Enable Output Compare and Overflow interrupts

#if defined(AUDIO)
// TIMER4 (16bit) set to mode 9, Prescaler 16MHz/64=250kHz.
// Used for audio tone generation.
  TCCR4A = (0b01 << WGM40);
  TCCR4B = (0b10 << WGM42) | (0b011 << CS40);
#endif

  // Setup Timer 5 for CPPM Out(s) and CPPM In.
  // Normal mode (0), OVF @ TOP (0xFFFF), F_CPU/8, Noise canceler on ICP, Falling edge trigger.
  TCCR5A=0;
  TCCR5C=0;
  TCCR5B = (2<<CS50);
  //	// Set up for input capture on PL1 (ICP5).
  //	  TCCR5B &= ~(1<<ICES5); // Falling edge trigger, ICP1 has pull-up enabled.
  //  	  // Should work better when connected to open collector stages and when left unconnected.
  //	  TCCR5B |= (1<<ICNC5); // Input Capture Noise Canceler.


#if defined (VOICE)
#endif

#if defined(ROTARY_ENCODERS)
// Rotary encoder interrupt setup for Evo digi-adjusters.

  EIMSK = 0; // Disable ALL external interrupts.
  //       Encoder REa LH  /  Encoder REb RH.
  EICRB  = (0b10 << ISC60) | (0b10 << ISC40); // 10 = interrupt on -ve edge.
  // EICRB  = (0b11 << ISC60) | (0b11 << ISC40); // 11 = interrupt on +ve edge.
  EICRB |= (0b01 << ISC70) | (0b01 << ISC50); // 01 = interrupt on any logical change.

  EIFR = (1<<INTF7) | (1<<INTF6) | (1<<INTF5) | (1<<INTF4); // Clear the int' flags in case they were set when changing modes.
  EIMSK = (1<<INT6) | (1<<INT4); // Enable the first of each encoder interrupt pairs.
#endif


#if defined(SPIMODULES)
// Setup (M)SPI Port.
// USART3
// PORTJ0 RXD3
// PORTJ1 TXD3
// PORTJ2 XCK3
// PORTJ3 CC2500 CSN ... Use PORTC4 for now.

// Setup pin states and USART in MSPI mode.
// Initialisation of USART.

  RF_CS_CC2500_INACTIVE();

  UBRR3 = 0; // Reset is part of initialisation sequence.
  UCSR3C = 0xC3; // UMSEL21:0 = 3 DORD2=0 CPHA2=1 CPOL2=1  USART in Master SPI mode, MSB first, Clock phase=1 Polarity=1.
  UCSR3B = (1 << RXEN3) | (1 << TXEN3); // Transmit and Receive.
  UBRR3 = 1; // 4 MHz clock ... 16MHz/(2*(UBRR+1))

  DDRJ |= (1<<PORTJ1) | (1<<PORTJ2); // | (1<<PORTJ3);
  DDRJ &= ~(1<<PORTJ0);

#endif // SPIMODULES
}
#endif // !SIMU

//--------------------------------------------------------------------------------------------------------

uint8_t USART3_mspi_xfer(uint8_t data)
{
  /* Wait for empty transmit buffer */
  while ( !( UCSR3A & (1<<UDRE3)) );
  /* Put data into buffer, sends the data */
  UDR3 = data;
  /* Wait for data to be received */
  while ( !(UCSR3A & (1<<RXC3)) );
  /* Get and return received data from buffer */
  return UDR3;
}


enum PowerState pwrCheck(void)
{
  static tmr10ms_t last10ms;

#if !defined(SIMU)
  if( ! (GPIO_C_PWR_STATUS & INP_C_PWR_STATUS) ) {
  // PWR Switch is on.
  last10ms = get_tmr10ms();
  return e_power_on;
  }
  // PWR Switch is off.
  else if( get_tmr10ms() - last10ms > 200 ) return e_power_off; // 2 seconds.
  else return e_power_on;

#else
  return e_power_on;
#endif
}


void pwrOff(void) // aka boardOff()
{
  GPIO_C_PWR_HOLD &= ~OUT_C_PWR_HOLD;
}


bool switches[NUM_SWS - SW_BASE];

bool switchState(EnumKeys enuk)
{
  if(enuk < (int)DIM(keys))
    return keys[enuk].state() ? 1 : 0;

  else if(enuk < NUM_SWS)
    return switches[enuk - SW_BASE];

  else
    return 0;
}


// Trim Buttons.
static const pm_uchar crossTrim[] PROGMEM ={
  TRIMS_GPIO_PIN_LH_PO, //left hand -port
  TRIMS_GPIO_PIN_LH_ST, //left hand -starboard
  TRIMS_GPIO_PIN_LH_UP,
  TRIMS_GPIO_PIN_LH_DN,
  TRIMS_GPIO_PIN_RH_DN,
  TRIMS_GPIO_PIN_RH_UP,
  TRIMS_GPIO_PIN_RH_ST,
  TRIMS_GPIO_PIN_RH_PO
};

uint8_t trimDown(uint8_t idx)
{
  return( (~GPIO_TRIMS) & pgm_read_byte(crossTrim+idx));
}

FORCEINLINE uint8_t keyDown()
{
// TODO Might have to set row outputs low.
  return( (~PINK) & (INP_K_KEYB_COL_A | INP_K_KEYB_COL_B | INP_K_KEYB_COL_C));
}

FORCEINLINE void readKeysAndTrims()
{
  // Keyboard.
  read_keyb_matrix();

  // Switches and Encoder Buttons.
//  getADC();
  read_A7_A9();
  read_A8_A10();

  // Trim Buttons.
  for(uint8_t i=0; i<8; i++) {
    keys[TRM_BASE + i].input( (~GPIO_TRIMS) & pgm_read_byte(crossTrim+i));
  }
}


/*
The Evo has two switch modules which convert the various switch states into an analogue voltages.
Each module has 2 * Push button switches, 2 * Two position switches and 2 * Three position switches.
Rather than reverse engineer the circuits, the four analogue voltages were fed into the ADC and a "state table"
was created.
The arrays below details the nominal voltage thresholds from the switch modules.
The #define ANA_TOL provides a transition zone around each voltage.

Switches H I K RHencoder feed A7
Switches G J feed A8
Switches M N P LHencoder feed A9
Switches L O feed A10
*/

#define ANA_TOL 40 // Assumed 1% resistors fitted.
// Therefore 1% of Max Value into 10 Bit AD (0x39D). Bit shifted due to pseudo 12 bit resolution.

void read_A7_A9(void)
{
  static const uint16_t A7A9[16] = {
    // AD reading, switch state bitmap value for two position switches
    // e.g. H/M I/N K/P EncRight/EncLeft
    0xDB0,// 0 0 0 0
    0x5AA,// 0 0 0 1
    0x9B2,// 0 0 1 0
    0x1CA,// 0 0 1 1
    0xE72,//4
    0x66A,//5
    0xA72,//6
    0x286,//7
    0xBCA,//8
    0x3CE,//9
    0x7D4,//10
    0,    //11
    0xC8A,//12
    0x48A,//13
    0x892,//14
    0x0B2,// 1 1 1 1
  };

  uint16_t AD_value;
  uint16_t upper;
  uint16_t lower;

  AD_value = s_anaFilt[ANALOG_A7];

  if((unsigned)AD_value + ANA_TOL < AD_value)
    upper = 0xFFFF;
  else upper = AD_value + ANA_TOL;

  if((unsigned)AD_value - ANA_TOL > AD_value)
    lower = 0;
  else lower = AD_value - ANA_TOL;

  for(uint8_t x=0; x<16; x++) {
    if( A7A9[x] < upper && A7A9[x] > lower ) {
      switches[SW_H - SW_BASE] = ( (x & 8) ? 1 : 0); // H
      switches[SW_I - SW_BASE] = ( (x & 4) ? 1 : 0); // I
      switches[SW_K - SW_BASE] = ( (x & 2) ? 1 : 0); // K
      keys[BTN_REb].input( (x & 1) ? 1 : 0); // EncRight
    }
  }

  AD_value = s_anaFilt[ANALOG_A9];

  if((unsigned)AD_value + ANA_TOL < AD_value)
    upper = 0xFFFF;
  else upper = AD_value + ANA_TOL;

  if((unsigned)AD_value - ANA_TOL > AD_value)
    lower = 0;
  else lower = AD_value - ANA_TOL;

  for(uint8_t x=0; x<16; x++) {
    if( A7A9[x] < upper && A7A9[x] > lower ) {
      switches[SW_M - SW_BASE] = ( (x & 8) ? 1 : 0); // M
      switches[SW_N - SW_BASE] = ( (x & 4) ? 1 : 0); // N
      switches[SW_P - SW_BASE] = ( (x & 2) ? 1 : 0); // P
      keys[BTN_REa].input( (x & 1) ? 1 : 0); // EncLeft
    }
  }
}


void read_A8_A10(void)
{
  static const uint16_t A8A10[9] = {
  // AD reading, switch state for three position switches
  // e.g.  O/J L/G.
    0x9B2,// up up
    0xA72,// up mi
    0x892,// up dn
    0xDB0,// mi up
    0xE72,// mi mi
    0xC8A,// mi dn
    0x5AA,// dn up
    0x66A,// dn mi
    0x48A,// dn dn
  };

  uint16_t AD_value;
  uint16_t upper;
  uint16_t lower;

  AD_value = s_anaFilt[ANALOG_A8];

  if((unsigned)AD_value + ANA_TOL < AD_value)
    upper = 0xFFFF;
  else upper = AD_value + ANA_TOL;

  if((unsigned)AD_value - ANA_TOL > AD_value)
    lower = 0;
  else lower = AD_value - ANA_TOL;

  for(uint8_t x=0; x<9; x++) {
    if( A8A10[x] < upper && A8A10[x] > lower ) {
      switches[SW_Jup - SW_BASE] = ( (x==0 || x==1 || x==2) ? 1 : 0); // J Up
      switches[SW_Jmi - SW_BASE] = ( (x==3 || x==4 || x==5) ? 1 : 0); // J Middle
      switches[SW_Jdn - SW_BASE] = ( (x==6 || x==7 || x==8) ? 1 : 0); // J Down
      switches[SW_Gup - SW_BASE] = ( (x==0 || x==3 || x==6) ? 1 : 0); // G Up
      switches[SW_Gmi - SW_BASE] = ( (x==1 || x==4 || x==7) ? 1 : 0); // G Middle
      switches[SW_Gdn - SW_BASE] = ( (x==2 || x==5 || x==8) ? 1 : 0); // G Down
    }
  }

  AD_value = s_anaFilt[ANALOG_A10];

  if((unsigned)AD_value + ANA_TOL < AD_value)
    upper = 0xFFFF;
  else upper = AD_value + ANA_TOL;

  if((unsigned)AD_value - ANA_TOL > AD_value)
    lower = 0;
  else lower = AD_value - ANA_TOL;

  for(uint8_t x=0; x<9; x++) {
    if( A8A10[x] < upper && A8A10[x] > lower ) {
      switches[SW_Oup - SW_BASE] = ( (x==0 || x==1 || x==2) ? 1 : 0); // O Up
      switches[SW_Omi - SW_BASE] = ( (x==3 || x==4 || x==5) ? 1 : 0); // O Middle
      switches[SW_Odn - SW_BASE] = ( (x==6 || x==7 || x==8) ? 1 : 0); // O Down
      switches[SW_Lup - SW_BASE] = ( (x==0 || x==3 || x==6) ? 1 : 0); // L Up
      switches[SW_Lmi - SW_BASE] = ( (x==1 || x==4 || x==7) ? 1 : 0); // L Middle
      switches[SW_Ldn - SW_BASE] = ( (x==2 || x==5 || x==8) ? 1 : 0); // L Dowm
    }
  }
}


bool read_keyb_matrix()
{
/*
The EVO has 11 buttons.
Rather than hardwire the buttons to specific functions we hardcode them.
The most efficient way (other than analogue - thankyou Multiplex) to read 11 buttons is to use a keyboard matrix.
We use a matrix of 4 row outputs and 3 column inputs.
To minimise circuitry we output a low on the selected row and look for lows on the column inputs.
The columns have input pullups enabled.
The rows are either logic low output or input: simulating open drain outputs.
The function reads the column pins selected by the row in the previous call
to the function.
The function is called every 10mS.
*/

  static uint8_t key_matrix_row_current = OUT_J_KEYB_ROW_A;
  static uint8_t key_matrix_row_previous =0;
  bool key_pressed =0;

  // Read column using previous row selection.
  key_pressed = (bool)((~GPIO_KEYB_MATRIX_COL) & (INP_K_KEYB_COL_A | INP_K_KEYB_COL_B | INP_K_KEYB_COL_C));

  if(key_matrix_row_previous == OUT_J_KEYB_ROW_A) {
    keys[KEY_LEFT].input( (GPIO_KEYB_MATRIX_COL & INP_K_KEYB_COL_A) ? 0 :1); //Spanner icon - (setup)
    keys[KEY_RIGHT].input( (GPIO_KEYB_MATRIX_COL & INP_K_KEYB_COL_B) ? 0 :1); //Joystick icon - (controls)
  }
  else if(key_matrix_row_previous == OUT_J_KEYB_ROW_B) {
    //if(GPIO_KEYB_MATRIX_COL & INP_K_KEYB_COL_A) incRotaryEncoder(1, +1); // test code
    //if(GPIO_KEYB_MATRIX_COL & INP_K_KEYB_COL_B) incRotaryEncoder(1, -1); // test code
  }
  else if(key_matrix_row_previous == OUT_J_KEYB_ROW_C) {
    keys[KEY_DOWN].input( (GPIO_KEYB_MATRIX_COL & INP_K_KEYB_COL_A) ? 0 :1); //Down arrow
    keys[KEY_UP].input( (GPIO_KEYB_MATRIX_COL & INP_K_KEYB_COL_B) ? 0 :1); //Up arrow
    keys[KEY_MENU].input( (GPIO_KEYB_MATRIX_COL & INP_K_KEYB_COL_C) ? 0 :1); //Enter
  }
  else if(key_matrix_row_previous == OUT_J_KEYB_ROW_D) {
    keys[KEY_EXIT].input( (GPIO_KEYB_MATRIX_COL & INP_K_KEYB_COL_A) ? 0 :1); //Rev/clr
  }
  else;

  // Output row as open drain.
  DDR_KEYB_MATRIX_ROW &= 0b00001111;
  DDR_KEYB_MATRIX_ROW |= key_matrix_row_current;
  //TODO Should really check port is outputting a low.
  key_matrix_row_previous = key_matrix_row_current;
  key_matrix_row_current = key_matrix_row_current << 1;

  if(key_matrix_row_current == 0) key_matrix_row_current = OUT_J_KEYB_ROW_A; // Bit left shifted to destruction.

  return key_pressed;
}

#if defined(ROTARY_ENCODERS)
/*
The Rotary Encoder (Digi-adjuster) ISR's.
There is hardware filtering of the signals,
however the ISR's debounce the encoder signals by working alternately.
e.g.
INT4 & 5 work as a pair for Encoder B.
INT6 & 7 work as a pair for Encoder A.

EICRB ISC Bits.
01 = interrupt on any logical change.
10 = interrupt on -ve edge.
11 = interrupt on +ve edge.
*/


ISR(INT4_vect) // REb (RH)
{
  if(EICRB & (0b11<<ISC40)) { // If +ve edge triggered change to -ve.
    EICRB &= ~(1<<ISC40);
    if(GPIO_ROT_ENCS & INP_E_ROT_ENC_RH_DIR) incRotaryEncoder(1, -1);
    else incRotaryEncoder(1, +1);
  }
  else EICRB |= (0b11<<ISC40);
  // Disables itself and enables INT5.
  EIMSK &= ~(1<<INT4);
  EIFR = (1<<INTF5) | (1<<INTF4);
  EIMSK |= (1<<INT5);
}

ISR(INT5_vect) // REb (RH) Level change ISR.
{
// Disables itself and enables INT4.
  EIMSK &= ~(1<<INT5);
  EIFR = (1<<INTF5) | (1<<INTF4);
  EIMSK |= (1<<INT4);
}

ISR(INT6_vect) // REa (LH)
{
  if(EICRB & (0b11<<ISC60)) {
    EICRB &= ~(1<<ISC60);
    if(GPIO_ROT_ENCS & INP_E_ROT_ENC_LH_DIR) incRotaryEncoder(0, -1);
    else incRotaryEncoder(0, +1);
  }
  else EICRB |= (0b11<<ISC60);
  // Disables itself and enables the other.
  EIMSK &= ~(1<<INT6);
  EIFR = (1<<INTF7) | (1<<INTF6);
  EIMSK |= (1<<INT7);
}

ISR(INT7_vect) // REa (LH) Level change ISR.
{
// Disables itself and enables the other.
  EIMSK &= ~(1<<INT7);
  EIFR = (1<<INTF7) | (1<<INTF6);
  EIMSK |= (1<<INT6);
}
#endif

#if 0
ISR(TIMER_10MS_VECT, ISR_NOBLOCK) // TIMER_COMPA_vect
{
// Clocks every 9.984ms & 10.048ms
  static uint8_t accuracyWarble = 0;

  TIMER_10MS_COMPVAL += (++accuracyWarble & 0x03) ? 156 : 157;


#if defined(AUDIO)
  AUDIO_HEARTBEAT();
#endif

#if defined(BUZZER)
  BUZZER_HEARTBEAT();
#endif

#if defined(HAPTIC)
  HAPTIC_HEARTBEAT();
#endif

  per10ms();
}
#endif

uint16_t getTmr16KHz(void);
extern volatile uint8_t g_tmr16KHz;

#if 0
uint16_t getTmr16KHz()
{
// g_tmr16KHz should not be global. It is only used by this function.
//
  while(1) {
    uint8_t hb  = g_tmr16KHz;
    uint8_t lb  = COUNTER_16KHZ;
    if(hb == g_tmr16KHz) return (hb<<8)|lb; //rick.
  }
}
#endif

bool check_slave_mode(void)
{
#if 0
  /*
  * This quirky function reads the mpx_rf_en signal which goes
  * to the mpx module and the trainer DIN.
  * If it ever reads a low, it stores the low in the port register.
  * All further reads are low because the pin is then configured as an output.
  */
  if(PINJ & IO_J_MPX_RF_EN) {
    return 0;
  }
  else {
    PORTJ &= ~IO_J_MPX_RF_EN; // Pin is low.
    DDRJ |= IO_J_MPX_RF_EN; // Pin is output.
    return 1;
  }
#endif
return 0;
}
