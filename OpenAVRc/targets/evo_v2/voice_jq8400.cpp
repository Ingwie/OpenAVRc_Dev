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

// JQ8400TF mp3 module driver
#include "../../OpenAVRc.h"

#define QUEUE_LENGTH (32)  // 32 words
uint16_t JQ6500_playlist[QUEUE_LENGTH];
uint8_t JQ6500_InputIndex = 0;
uint8_t JQ6500_PlayIndex = 0;

void pushPrompt(uint16_t prompt) {
  // If mute active => no voice
  if (g_eeGeneral.beepMode == e_mode_quiet)
    return;
  ++prompt; // With SDformatter, first FAT address = 1 : MP3 files in a directory.
  /* Load playlist buffer */
  JQ6500_playlist[JQ6500_InputIndex] = prompt;
  if (++JQ6500_InputIndex == QUEUE_LENGTH)
    JQ6500_InputIndex = 0;
}

uint8_t jq_pkt[] = { 0xAA, // Start Code.
    0x07, // Command - 0x07 = Play file (FAT index number).
    0X02, // Data bytes to follow.
    0x00, // Dummy MSB file
    0x08, // Dummy LSB file
    0xBB }; // Checksum - Low byte of the sum of all previous bytes.

enum JQ8400_pkt { // Play packet.
  START = 0, // Start code
  CMD,       // Command byte
  NUMBY,     // Data bytes to follow
  FILEH,     // FAT index high byte
  FILEL,     // FAT index low byte
  CHKSUM,    // Checksum - Low byte of the sum of all previous bytes.
};

void JQ6500Check() // Called every 10 ms.
{
#if !defined(SIMU)
// One UART character has a duration of ~1 ms, so send a char on each call if required. Don't use interrupts.

  enum vox_state {
    WAIT_NOT_BUSY,
    CHECK_PLAYLIST,
    MAKE_PLAY_PACKET,
    CHECK_VOX_PA_FREE,
    SEND_CHAR_START,
    SEND_CHAR_CMD,
    SEND_CHAR_NUMBY,
    SEND_CHAR_N,
    SEND_CHAR_CHKSUM,
    WAIT_BUSY_DELAY,
    WAIT_BUSY,
  };

  static uint8_t vox_state = WAIT_NOT_BUSY;
  static uint8_t ten_ms_counts;
  uint8_t num_bytes;

  switch (vox_state) {

  case WAIT_NOT_BUSY: // JQ8400 startup or MP3 playing or other Audio output e.g. Beeps using PA.
    if (!JQ6500_BUSY)
      vox_state = CHECK_PLAYLIST;
    break;

  case CHECK_PLAYLIST:
    if (JQ6500_PlayIndex != JQ6500_InputIndex)
      vox_state = MAKE_PLAY_PACKET;  // Something to play.
    break;

  case MAKE_PLAY_PACKET:
    jq_pkt[START] = 0xAA;
    jq_pkt[CMD] = 0x07;
    jq_pkt[NUMBY] = 0x02;
    jq_pkt[FILEH] = (uint8_t)(JQ6500_playlist[JQ6500_PlayIndex] >> 8);
    jq_pkt[FILEL] = (uint8_t)(0x00FF & JQ6500_playlist[JQ6500_PlayIndex]);
    jq_pkt[CHKSUM] = (uint8_t)(
        jq_pkt[START] + jq_pkt[CMD] + jq_pkt[NUMBY] + jq_pkt[FILEH]
            + jq_pkt[FILEL]);

    if (++JQ6500_PlayIndex == QUEUE_LENGTH)
      JQ6500_PlayIndex = 0;
    vox_state = CHECK_VOX_PA_FREE;
    break;

  case CHECK_VOX_PA_FREE:
    if (!JQ6500_BUSY)
      vox_state = SEND_CHAR_START;
    // If using Bi-directional Busy signal, make it output and active.
    // If using Power Amp control, This will turn on the PA now.
    ten_ms_counts = 0;
    break;

  case SEND_CHAR_START:
    // Delay before send ... Lazy JQ8400.
    if (ten_ms_counts++ < 10)
      break; // 0.1 seconds.

    if (VOICE_USART.STATUS & USART_DREIF_bm)
      VOICE_USART.DATA = jq_pkt[START]; // Send Data, clears DRE flag.
    vox_state = SEND_CHAR_CMD;
    break;

  case SEND_CHAR_CMD:
    if (VOICE_USART.STATUS & USART_DREIF_bm)
      VOICE_USART.DATA = jq_pkt[CMD];
    vox_state = SEND_CHAR_NUMBY;
    break;

  case SEND_CHAR_NUMBY:
    if (VOICE_USART.STATUS & USART_DREIF_bm)
      VOICE_USART.DATA = jq_pkt[NUMBY];
    vox_state = SEND_CHAR_N;
    break;

  case SEND_CHAR_N: // For different length packets.
    num_bytes = 0;

    while (jq_pkt[NUMBY] != num_bytes++) {
      if (VOICE_USART.STATUS & USART_DREIF_bm)
        VOICE_USART.DATA = jq_pkt[NUMBY + num_bytes];
    }

    vox_state = SEND_CHAR_CHKSUM;
    break;

  case SEND_CHAR_CHKSUM:
    if (VOICE_USART.STATUS & USART_DREIF_bm)
      VOICE_USART.DATA = jq_pkt[jq_pkt[NUMBY] + NUMBY + 1];

    ten_ms_counts = 0;
    vox_state = WAIT_BUSY_DELAY;
    break;

  case WAIT_BUSY_DELAY:
    // If using Bi-directional Busy signal, wait a set time before making it input and inactive.
    if (ten_ms_counts++ < 60)
      break; // 0.6 seconds.
    vox_state = WAIT_BUSY;
    break;

  case WAIT_BUSY: // Wait for JQ to become busy (playing MP3) after sending packet.
    if (JQ6500_BUSY)
      vox_state = WAIT_NOT_BUSY;
    break;

  }

#endif
}

void InitJQ6500UartTx() {
// 9600 8N1 transmit only.
#if !defined(SIMU)

  VOICE_USART.CTRLA = 0; // Disable interrupts.
  VOICE_USART.CTRLB = 0; // CLK2X = 0,
  USART_SET_BAUD_9K6 (VOICE_USART);
  USART_SET_MODE_8N1(VOICE_USART);
  VOICE_USART.CTRLB |= USART_TXEN_bm; // Enable transmitter.

  // Set JQ8400 Volume. Default is 20. Range 0 to 30.
  jq_pkt[START] = 0xAA;
  jq_pkt[CMD] = 0x13;
  jq_pkt[NUMBY] = 0x01;
  jq_pkt[3] = 30; // Volume.
  jq_pkt[4] = (uint8_t)(
      jq_pkt[START] + jq_pkt[CMD] + jq_pkt[NUMBY] + jq_pkt[3]);

  for (uint8_t i = 0; i < 5; i++) {
    while (!(VOICE_USART.STATUS & USART_DREIF_bm))
      ;
    VOICE_USART.DATA = jq_pkt[i];
  }

#endif
}

