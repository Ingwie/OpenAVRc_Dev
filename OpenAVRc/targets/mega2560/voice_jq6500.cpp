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

#include "../../OpenAVRc.h"


#define QUEUE_LENGTH (18*2)  //bytes

enum JQ6500_State {
	START = 0, //0x7E Start
	NUMBY,     //0x04 Num bytes follow
	SELEC,     //0X03 Select file
	FILEH,     //0x00 Dummy file
	FILEL,     //0x00 Dummy file
	TERMI,     //0xEF Termination
};

volatile uint8_t JQstate = START;
uint8_t JQ6500_Data[6] = {0x7E, //Start
                          0x04, //Num bytes follow
                          0X03, //Select file
                          0x00, //Dummy MSB file
                          0x00, //Dummy LSB file
                          0xEF}; //Termination

uint8_t JQ6500_playlist[QUEUE_LENGTH] = {0};
uint8_t JQ6500_InputIndex = 0;
uint8_t JQ6500_PlayIndex = 0;

void pushPrompt(uint16_t prompt)
{
	// if mute active => no voice
	if (g_eeGeneral.beepMode == e_mode_quiet) return;
	++prompt;  // With SDformatter, first FAT address = 1 : MP3 files in a directory
	/* Load playlist and activate interrupt */
	JQ6500_playlist[JQ6500_InputIndex] = (uint8_t)(prompt >> 8);    // MSB first
	++JQ6500_InputIndex;
	JQ6500_playlist[JQ6500_InputIndex] = (uint8_t)(prompt & 0xFF);  // LSB after
	++JQ6500_InputIndex;
	if (JQ6500_InputIndex == QUEUE_LENGTH) JQ6500_InputIndex = 0;

}

void InitJQ6500UartTx(void)
{
#if !defined(SIMU)

#undef BAUD
#define BAUD 9600
#include <util/setbaud.h>

  UBRRH_N(TLM_JQ6500) = UBRRH_VALUE;
  UBRRL_N(TLM_JQ6500) = UBRRL_VALUE;
  UCSRA_N(TLM_JQ6500) &= ~(1 << U2X_N(TLM_JQ6500)); // disable double speed operation.

  UCSRB_N(TLM_JQ6500) = 0 | (0 << RXCIE_N(TLM_JQ6500)) | (0 << TXCIE_N(TLM_JQ6500))    // set 8N1
  | (0 << UDRIE_N(TLM_JQ6500)) | (0 << RXEN_N(TLM_JQ6500)) | (1 << TXEN_N(TLM_JQ6500)) // disable RX enable TX
  | (0 << UCSZ2_N(TLM_JQ6500));

  UCSRC_N(TLM_JQ6500) = 0 | (1 << UCSZ1_N(TLM_JQ6500)) | (1 << UCSZ0_N(TLM_JQ6500));

#endif
}

void JQ6500Check()
{

  if ((JQ6500_PlayIndex == JQ6500_InputIndex) || (JQstate != START) || (JQ6500_BUSY) ) return;

  JQ6500_Data[3] = JQ6500_playlist[JQ6500_PlayIndex];
  ++JQ6500_PlayIndex;
  JQ6500_Data[4] = JQ6500_playlist[JQ6500_PlayIndex];
  ++JQ6500_PlayIndex;
  if (JQ6500_PlayIndex == QUEUE_LENGTH) JQ6500_PlayIndex = 0;

  UDR_N(TLM_JQ6500) = JQ6500_Data[JQstate]; // Send Datas
  JQstate = START;
  UCSRB_N(TLM_JQ6500) |= (1 << UDRIE_N(TLM_JQ6500)); // enable UDRE(TLM_JQ6500) interrupt
}

ISR(USART_UDRE_vect_N(TLM_JQ6500))
{
  if (JQstate != TERMI) {
    UDR_N(TLM_JQ6500) = JQ6500_Data[++JQstate];
  } else {
    UCSRB_N(TLM_JQ6500) &= ~(1 << UDRIE_N(TLM_JQ6500)); // disable UDRE(TLM_JQ6500) interrupt
    JQstate = START;
  }
}

