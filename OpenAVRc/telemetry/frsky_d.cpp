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


#include "../OpenAVRc.h"



// Alarms level sent to the FrSky module

uint8_t frskyAlarmsSendState = 0 ;

void frskyPushValue(uint8_t *&ptr, uint8_t value)
{
  // byte stuff the only byte than might need it
  bool bytestuff = false;

  if (value == START_STOP) {
    bytestuff = true;
    value = 0x5e;
  } else if (value == BYTESTUFF) {
    bytestuff = true;
    value = 0x5d;
  }

  *ptr++ = value;
  if (bytestuff)
    *ptr = BYTESTUFF;
}

void frskySendPacket(uint8_t type, uint8_t value, uint8_t p1, uint8_t p2)
{
  uint8_t *ptr = &Usart0TxBuffer[0];

  *ptr++ = START_STOP;        // End of packet
  *ptr++ = 0x00;
  *ptr++ = 0x00;
  *ptr++ = 0x00;
  *ptr++ = 0x00;
  *ptr++ = 0x00;
  *ptr++ = (IS_SOUND_OFF() ? alarm_off : p2);
  *ptr++ = p1;
  frskyPushValue(ptr, value);
  *ptr++ = type;
  *ptr++ = START_STOP; // Start of packet

  Usart0TxBufferCount = ptr - &Usart0TxBuffer[0];
#if !defined(SIMU)
  telemetryTransmitBuffer();
#endif
}

inline void frskyDSendNextAlarm(void)
{
  if (Usart0TxBufferCount)
    return; // we only have one buffer. If it's in use, then we can't send yet.

  // Now send a packet
  frskyAlarmsSendState -= 1;
  uint8_t alarm = 1 - (frskyAlarmsSendState % 2);
  if (frskyAlarmsSendState < SEND_MODEL_ALARMS) {
    uint8_t channel = 1 - (frskyAlarmsSendState / 2);
    frskySendPacket(A22PKT + frskyAlarmsSendState, g_model.frsky.channels[channel].alarms_value[alarm], ALARM_GREATER(channel, alarm), ALARM_LEVEL(channel, alarm));
  } else {
    frskySendPacket(RSSI1PKT-alarm, getRssiAlarmValue(alarm), 0, (2+alarm+g_model.frsky.rssiAlarms[alarm].level) % 4);
  }
}
