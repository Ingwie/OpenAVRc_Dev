/*
**************************************************************************
*                                                                        *
*              This file is part of the OpenAVRc project.                *
*                                                                        *
*                         Based on code named                            *
*             OpenTx - https://github.com/opentx/opentx                  *
*                                                                        *
*                Only AVR code here for lisibility ;-)                   *
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


// JQ6500 mp3 module driver
// 6 X 10 (bits) X 104µs = 6.24 mS to send a playfile command

#include "../../OpenAVRc.h"


#define QUEUE_LENGTH 16*2  //bytes

enum JQ6500_State {
  START = 0x7E, //Start
  NUMBY = 0x04, //Num bytes follow
  SELEC = 0X03, //Select file
  FILEH = 0x00, //Dummy file
  FILEL = 0x01, //Dummy file
  TERMI = 0xEF  //Termination
};

JQ6500_State JQstate = START;
uint8_t JQ6500_playlist[QUEUE_LENGTH] = {0};
volatile uint8_t JQ6500_InputIndex = 0;
uint8_t JQ6500_PlayIndex = 0;



uint8_t isPlaying()
{
  /* interrupts active on Output Compare A Match ? */
#if defined(SIMU)
  return false;
#else
  return (TIMSK5 & (1<<OCIE5A));
#endif
}

void pushPrompt(uint16_t prompt)
{
    // if mute active => no voice
  if (g_eeGeneral.beepMode == e_mode_quiet) return;
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) { // To be sure than prompt list is fully updated
  ++prompt;  // With SDformatter, first FAT address = 1 : MP3 files in a directory
  /* Load playlist and activate interrupt */
  JQ6500_playlist[JQ6500_InputIndex] = (uint8_t)(prompt >> 8);    // MSB first
  ++JQ6500_InputIndex;
  JQ6500_playlist[JQ6500_InputIndex] = (uint8_t)(prompt & 0xFF);  // LSB after
  ++JQ6500_InputIndex;
  if (JQ6500_InputIndex == QUEUE_LENGTH) JQ6500_InputIndex = 0;
  }
  if (!isPlaying()) {
    TCNT5=0;
    OCR5A = 0xFA;
    TIMSK5 |= (1<<OCIE5A); // enable interrupts on Output Compare A Match
  }
}

uint8_t JQ6500_sendbyte(uint8_t Data_byte)
{
  static uint8_t i = 0;

  if (!i) {
    JQ6500_Serial_off;  // serial start bit
    ++i;
    return 0;
  }

  if (i==9) {
    JQ6500_Serial_on;  // serial stop bit
    i = 0;
    return 1;
  }

  if ((Data_byte >> (i-1)) & 0x01) {
    JQ6500_Serial_on; // send data bits
  } else {
    JQ6500_Serial_off;
  }
  ++i;
  return 0;
}

#if !defined(SIMU)
ISR(TIMER5_COMPA_vect) // every 104µS / 9600 Bauds serial
{
  sei();
  if (JQstate == START) {
    OCR5A = 0x19;
    if (JQ6500_BUSY) return;
    if (JQ6500_sendbyte(JQstate)) {
      JQstate = NUMBY;
      return;
    }
  }

  if (JQstate == NUMBY) {
    if (JQ6500_sendbyte(JQstate)) {
      JQstate = SELEC;
      return;
    }
  }

  if (JQstate == SELEC) {
    if (JQ6500_sendbyte(JQstate)) {
      JQstate = FILEH;
      return;
    }
  }

  if (JQstate == FILEH) {
    if (JQ6500_sendbyte(JQ6500_playlist[JQ6500_PlayIndex])) {
      ++JQ6500_PlayIndex;
      JQstate = FILEL;
      return;
    }
  }

  if (JQstate == FILEL) {
    if (JQ6500_sendbyte(JQ6500_playlist[JQ6500_PlayIndex])) {
      ++JQ6500_PlayIndex;
      JQstate = TERMI;
      return;
    }
  }

  if (JQstate == TERMI) {
    if (JQ6500_sendbyte(JQstate)) {
      JQstate = START;
      if (JQ6500_PlayIndex == QUEUE_LENGTH) JQ6500_PlayIndex = 0;
      if (JQ6500_PlayIndex == JQ6500_InputIndex) {
        OCR5A = 0x19;  // stop reentrance
        TIMSK5 &= ~(1<<OCIE5A);
      }
      return;
    }
  }
  cli();
}
#endif
