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


#define QUEUE_LENGTH (24*2)  //bytes

enum JQ6500_State {
	START = 0, //0x7E Start
	NUMBY,     //0x04 Num bytes follow
	SELEC,     //0X03 Select file
	FILEH,     //0x00 Dummy file
	FILEL,     //0x00 Dummy file
	TERMI,     //0xEF Termination
};

uint8_t JQstate = START;
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
	JQ6500_playlist[JQ6500_InputIndex++] = (uint8_t)(prompt >> 8);    // MSB first
	JQ6500_playlist[JQ6500_InputIndex++] = (uint8_t)(prompt);  // LSB after
	if (JQ6500_InputIndex == QUEUE_LENGTH) JQ6500_InputIndex = 0;

}

void InitJQ6500UartTx()
{
// 9600 8N1
#if !defined(SIMU)

  VOICE_USART.CTRLA = 0; // Disable interrupts.
  VOICE_USART.CTRLB = 0; // CLK2X = 0,
  USART_SET_BAUD_9K6(VOICE_USART);
  USART_SET_MODE_8N1(VOICE_USART);
  VOICE_USART.CTRLB |= USART_TXEN_bm; // Enable transmitter.
#endif
}


void JQ6500Check()
{
#if !defined(SIMU)
  if ((JQ6500_PlayIndex == JQ6500_InputIndex) || (JQstate != START) || (JQ6500_BUSY) ) return;

  JQstate = START;

  JQ6500_Data[3] = JQ6500_playlist[JQ6500_PlayIndex++];
  JQ6500_Data[4] = JQ6500_playlist[JQ6500_PlayIndex++];
  if (JQ6500_PlayIndex == QUEUE_LENGTH) JQ6500_PlayIndex = 0;

  VOICE_USART.DATA = JQ6500_Data[JQstate]; // Send Data, clears DRE flag.
  VOICE_USART.CTRLA = USART_DREINTLVL_LO_gc;
#endif
}


ISR(VOICE_DRE_VECT)
{
#if !defined(SIMU)
  if (JQstate != TERMI) {
    VOICE_USART.DATA = JQ6500_Data[++JQstate];
  } else {
    VOICE_USART.CTRLA &= ~USART_DREINTLVL_gm;
    VOICE_USART.STATUS &= ~USART_DREIF_bm; // precautionary.
    JQstate = START;
  }
#endif
}

