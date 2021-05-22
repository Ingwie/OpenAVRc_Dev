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


// JQ6500 mp3 module driver
// 6 X 10 (bits) X 104µs = 6.24 mS to send a playfile command
// If anyone know how to change JQ6500 USART speed -> Tell me please ;-)

#include "../../OpenAVRc.h"

enum JQ6500_State {
	START = 0, //0x7E Start
	NUMBY,     //0x04 Num bytes follow
	SELEC,     //0X03 Select file
	FILEH,     //0x00 Dummy file
	FILEL,     //0x00 Dummy file
	TERMI,     //0xEF Termination
	WAIT1LOOP, //Wait one loop state
};

uint8_t JQ6500_Data[6] = {0x7E, //Start
                          0x04, //Num bytes follow
                          0X03, //Select file
                          0x00, //Dummy MSB file
                          0x00, //Dummy LSB file
                          0xEF}; //Termination

volatile uint8_t JQstate = START;
Fifo<JQ6500_QUEUE_LENGTH> JQ6500_Fifo;

void InitJQ6500UartTx()
{
#if !defined(SIMU)
  USART_SET_BAUD_9K6(TLM_JQ6500);
  USART_SET_MODE_8N1(TLM_JQ6500);
  USART_ENABLE_TX(TLM_JQ6500);
#endif
}

void pushPrompt(uint16_t prompt)
{
	// if mute active => no voice
	if (g_eeGeneral.beepMode == e_mode_quiet) return;
	++prompt;  // With SDformatter, first FAT address = 1 : MP3 files in a directory
	/* Load playlist buffer - Overflow is not controled */
	JQ6500_Fifo.push((uint8_t)(prompt >> 8)); // MSB first
	JQ6500_Fifo.push((uint8_t)prompt);        // LSB after
}

void JQ6500Check()
{
#if !defined(SIMU)
  if (!JQ6500_Fifo.available()) return; // Nothing to play, return ...

  if (JQ6500_BUSY) {JQstate = WAIT1LOOP; return;}    // Wait first 10mS after JQ "is buzy"

  if (JQstate == WAIT1LOOP) {--JQstate; return;}     // Wait another 10mS before sending new prompt

  JQ6500_Data[3] = JQ6500_Fifo.pop();
  JQ6500_Data[4] = JQ6500_Fifo.pop();
  JQstate = START;
  UDR_N(TLM_JQ6500) = JQ6500_Data[JQstate]; // Send Datas
  USART_TRANSMIT_BUFFER(TLM_JQ6500); // enable UDRE(TLM_JQ6500) interrupt

#endif
}

ISR(USART_UDRE_vect_N(TLM_JQ6500))
{
#if !defined(SIMU)
  if (JQstate != TERMI) {
    UDR_N(TLM_JQ6500) = JQ6500_Data[++JQstate];
  } else {
    UCSRB_N(TLM_JQ6500) &= ~(1 << UDRIE_N(TLM_JQ6500)); // disable UDRE(TLM_JQ6500) interrupt
  }
#endif
}

