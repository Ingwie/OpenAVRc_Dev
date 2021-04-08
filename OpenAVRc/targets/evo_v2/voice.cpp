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



#include "../../OpenAVRc.h"

//#define BIDIRBUSY  // Bi-directional busy pin for control of audio amplifier and possible contention of voice and audio beeps.

#define VOX_QUEUE_LEN  22  // was 32 words

uint16_t JQ6500_playlist[VOX_QUEUE_LEN];
uint8_t JQ6500_InputIndex = 0;
uint8_t JQ6500_PlayIndex = 0;

#define RBUF_MASK  0x0F
uint8_t vox_rbuf[RBUF_MASK +1]; // was a ring buffer of size 2^n (simple maths), now linear buffer.
volatile uint8_t vox_rbuf_in = 0; // packet byte count.
volatile uint8_t vox_rbuf_out = 0; //

void make_play_pkt(void);
void make_volume_pkt(void);


void JQ6500Check() // Called every 10 ms.
{

  enum vox_state
  {
    CHECK_PLAYLIST,
    MAKE_VOLUME_PKT,
    SEND_VOLUME_PKT,
    GET_VOX_PA,
    MAKE_PLAY_PKT,
    SEND_PLAY_PKT,
    WAIT_BUSY_DELAY,
    WAIT_NOT_BUSY,
  };

  static uint8_t vox_state = WAIT_NOT_BUSY;
  static uint8_t ten_ms_counts;


#if !defined(SIMU)

  switch (vox_state)
  {

  case CHECK_PLAYLIST:
    if (JQ6500_PlayIndex == JQ6500_InputIndex) break;
    vox_state = MAKE_VOLUME_PKT; // Something to play.
    // drop through // break;

  case MAKE_VOLUME_PKT:
    if(vox_rbuf_in != vox_rbuf_out) break; // Some processing of another packet in progress.
    make_volume_pkt();
    vox_rbuf_out = 0;
    vox_state = SEND_VOLUME_PKT;
    // drop through // break;

  case SEND_VOLUME_PKT:
    vox_state = GET_VOX_PA;
    VOICE_USART.CTRLA |= USART_DREINTLVL_LO_gc;
    break;

  case GET_VOX_PA:
    if (!VOICE_BUSY)
    {
#if defined (BIDIRBUSY)
    // If using Bi-directional Busy signal, make it active low and output.
    // If using Audio Power Amp control, This will turn on the Amp now.
    VOICE_BUSY_PORT.OUTCLR = 1<< VOICE_BUSY_PIN; // Get use of Audio Amplifier.
    VOICE_BUSY_PORT.DIRSET = 1<< VOICE_BUSY_PIN;

#endif
    vox_state = MAKE_PLAY_PKT;
    }
    break;

  case MAKE_PLAY_PKT:
    if(vox_rbuf_in != vox_rbuf_out) break; // Some processing of another packet in progress.
    make_play_pkt();
    vox_rbuf_out = 0;
    if (++JQ6500_PlayIndex == VOX_QUEUE_LEN) JQ6500_PlayIndex = 0;
    vox_state = SEND_PLAY_PKT;
    break;

  case SEND_PLAY_PKT:
    ten_ms_counts = 0;
    vox_state = WAIT_BUSY_DELAY;
    VOICE_USART.CTRLA |= USART_DREINTLVL_LO_gc;
    break;

#if defined (VOICE_BY8001)
#define  BUSYDELAY  15
#elif defined (VOICE_JQ8400)
#define  BUSYDELAY  80
#endif

  case WAIT_BUSY_DELAY:
    // Wait for player to become busy. The busy signal will (should) be already active if using Bi-directional busy signal.
    if (ten_ms_counts++ < BUSYDELAY) break; // 150ms should be okay for BY8001. JQ8400 may be much slower (0.8s)!.
#if defined (BIDIRBUSY)
    VOICE_BUSY_PORT.DIRCLR = 1<< VOICE_BUSY_PIN; // MP3 player should be playing now.
#endif
    vox_state = WAIT_NOT_BUSY;
    break;

/*  We do not wait for the MP3 player to become busy because :-
 *  1. The MP3 file may not exist on SD (TF) CARD.
 *  2. The prompt request may be invalid ... I have seen this (0x02E0) !.
 */

  case WAIT_NOT_BUSY: // Module startup or MP3 playing or other Audio output e.g. Beeps using PA.
    if (!VOICE_BUSY) vox_state = CHECK_PLAYLIST;
    break;
  }

#endif
}


void pushPrompt(uint16_t prompt)
{
  // If mute active => no voice
  if (g_eeGeneral.beepMode == e_mode_quiet) return;
  ++prompt;  // With SDformatter, first FAT address = 1 : MP3 files in a directory
  /* Load playlist buffer */
  JQ6500_playlist[JQ6500_InputIndex] = prompt;
  if (++JQ6500_InputIndex == VOX_QUEUE_LEN) JQ6500_InputIndex = 0;
}


void InitVoiceUartTx()
{
  // 9600 8N1 transmit only.
  // One UART character has a duration of ~1 ms.
#if !defined (SIMU)
  VOICE_USART.CTRLA = 0; // Disable interrupts.
  VOICE_USART.CTRLB = 0; // CLK2X = 0,
  USART_SET_BAUD_9K6(VOICE_USART);
  USART_SET_MODE_8N1(VOICE_USART);
  VOICE_USART.CTRLB |= USART_TXEN_bm; // Enable transmitter.
#endif
}


ISR(VOICE_DRE_VECT)
{
#if !defined(SIMU)

  if(vox_rbuf_in == vox_rbuf_out)
  {
    VOICE_USART.DATA = vox_rbuf[vox_rbuf_out];
    VOICE_USART.CTRLA &= ~USART_DREINTLVL_gm;
  }
  else
  {
    VOICE_USART.DATA = vox_rbuf[vox_rbuf_out++];
  }
#endif
}

#if defined (VOICE_BY8001)
// BY8001 module driver.
/* The BY8001 packet format is as follows
 * Start code of 0x7E
 * Number of bytes to follow including End code (3 to 5).
 * Operation code.
 * Data byte(s).
 * Check code. XOR of bytes after Start code to here.
 * End code 0xEF
 */
#define START_CODE  0x7E
#define END_CODE    0xEF

void make_volume_pkt(void)
{
  // Set BY8001 Volume. This is stored in BY8001 NVM. Range 0 to 30.
  vox_rbuf[0] = START_CODE;
  vox_rbuf[1] = 0x04;
  vox_rbuf[2] = 0x31; // Command.
  vox_rbuf[3] = (uint8_t) 30; // Volume;
  vox_rbuf[4] = (uint8_t) vox_rbuf[1] ^ vox_rbuf[2] ^ vox_rbuf[3]; // Check byte.
  vox_rbuf[5] = END_CODE;
  vox_rbuf_in = 5;
}

void make_play_pkt(void)
{
  vox_rbuf[0] = START_CODE;
  vox_rbuf[1] = 0x05; // Number of following bytes.
  vox_rbuf[2] = 0x41; // Command.
  vox_rbuf[3] = (uint8_t) (JQ6500_playlist[JQ6500_PlayIndex] >> 8); // File high byte.
  vox_rbuf[4] = (uint8_t) (0x00FF & JQ6500_playlist[JQ6500_PlayIndex]); // File low byte.
  vox_rbuf[5] = (uint8_t) vox_rbuf[1] ^ vox_rbuf[2] ^ vox_rbuf[3] ^ vox_rbuf[4]; // Check byte.
  vox_rbuf[6] = END_CODE;
  vox_rbuf_in = 6;
}
#endif

#if defined (VOICE_JQ8400)
// JQ8400 module driver.
/* The JQ8400 packet format is as follows
 * Start code of 0xAA
 * Command byte.
 * Number of following data bytes.
 * Data byte(s).
 * Checksum - Low byte of the sum of all previous bytes.
 */
#define START_CODE  0xAA
void make_volume_pkt(void)
{
  // Set JQ8400 Volume. Default is 20. Range 0 to 30.
  vox_rbuf[0] = START_CODE;
  vox_rbuf[1] = 0x13; // Command.
  vox_rbuf[2] = 0x01; // Number of data bytes.
  vox_rbuf[3] = (uint8_t) 30; // Volume.
  vox_rbuf[4] = (uint8_t) vox_rbuf[0] + vox_rbuf[1] + vox_rbuf[2] + vox_rbuf[3];
  vox_rbuf_in = 4;
}

void make_play_pkt(void)
{
  vox_rbuf[0] = START_CODE;
  vox_rbuf[1] = 0x07; // Command.
  vox_rbuf[2] = 0x02; // Number of data bytes.
  vox_rbuf[3] = (uint8_t) (JQ6500_playlist[JQ6500_PlayIndex] >> 8); // File high byte.
  vox_rbuf[4] = (uint8_t) (0x00FF & JQ6500_playlist[JQ6500_PlayIndex]); // File low byte.
  vox_rbuf[5] = (uint8_t) vox_rbuf[0] + vox_rbuf[1] + vox_rbuf[2] + vox_rbuf[3] + vox_rbuf[4];
  vox_rbuf_in = 5;
}
#endif

#if 0 // JQ6500
// JQ6500 module driver.
/* The JQ6500 packet format is as follows
 * Start code of 0x7E
 * Number of following bytes.
 * Command byte.
 * Data byte(s).
 * End code of 0xEF.
 */
#define START_CODE  0x7E
#define END_CODE    0xEF

void make_volume_pkt(void)
{
  // Set JQ6500 Volume. Range 0 to 30.
  vox_rbuf[0] = START_CODE;
  vox_rbuf[1] = 0x03; // Number of following bytes.
  vox_rbuf[2] = 0x06; // Command.
  vox_rbuf[3] = (uint8_t) 30; // Volume.
  vox_rbuf[4] = END_CODE;
  vox_rbuf_in = 4;
}

void make_play_pkt(void)
{
  vox_rbuf[0] = START_CODE;
  vox_rbuf[1] = 0x04; // Number of following bytes.
  vox_rbuf[2] = 0x03; // Command.
  vox_rbuf[3] = (uint8_t) (JQ6500_playlist[JQ6500_PlayIndex] >> 8); // File high byte.
  vox_rbuf[4] = (uint8_t) (0x00FF & JQ6500_playlist[JQ6500_PlayIndex]); // File low byte.
  vox_rbuf[5] = END_CODE;
  vox_rbuf_in = 5;
}
#endif


