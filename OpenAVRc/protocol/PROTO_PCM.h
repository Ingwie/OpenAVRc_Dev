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

#ifndef PROTO_PCM_H
#define PROTO_PCM_H

#define FUT_PCM1024_PROP_CH_NB       8


#define  FUT_PCM1024_FRAME_PERIOD_US 28500U

#define PcmProto g_model.rfSubType
uint16_t CheckPCMPeriod()
{
  if (PcmProto == 0)//Futaba PCM1024
  {
    return 28500U;
  }
  else if (PcmProto == 1)//Graupner S-PCM
  {
    return 44000U;
  }
  else if (PcmProto == 2)//Multiplex M-PCM
  {
    return 11100U;
  }
}


enum {FUT_PCM1024_BUILD_DO_NOTHING = 0, FUT_PCM1024_BUILD_2_FIRST_PACKETS, FUT_PCM1024_BUILD_2_LAST_PACKETS};

union PcmStreamBitNbCouple_Union
{
  uint8_t Value;
  struct{
    uint8_t
            LowNibbleBitNb:  4,
            HighNibbleBitNb: 4;
  };
};

#define PCM_STREAM_BYTE_NB   30 // Can store up to (2 x PCM_STREAM_BYTE_NB) bit durations
#define PCM_NBL_MAX_IDX      ((2 * PCM_STREAM_BYTE_NB) - 1)

typedef struct{
  uint8_t
					BuildState:   3, // 0: Nothing to do, 1: Channels 0 & 1 or 4 & 5 , 2: Channels 2 & 3 or 6 & 7
					PacketIdx:    3,
					IsrBufIdx:    1,
					BitVal:       1;
  uint8_t BuildNblIdx;
  uint8_t BuildEndNblIdx[2]; // For the ISR to know PCM frame is fully sent
  uint8_t TxNblIdx;
	uint8_t XanyChMap; // One bit per Channel (8 channels max from bit0 to bit7)
  int16_t MemoChOutputs[FUT_PCM1024_PROP_CH_NB]; // Used to compute Deltas
  PcmStreamBitNbCouple_Union StreamConsecBitTbl[2][PCM_STREAM_BYTE_NB]; // Double buffering (One buffer is filled during the other is transmitting)
}FutPcm1024St_t;

#if 0
typedef struct{
  FutPcm1024St_t Pcm1024;
}FutabaSt_t;
#endif

#define Futaba    pulses2MHz // Just for readability (Futaba PCM1024 structure is an union of pulses2MHz buffer)

/* PUBLIC FUNCTION PROTOTYPES */
void FutabaPcm1024_buildHalfRadioPcmBitStream(void); // SHALL be called as often as possible in the main loop

void FutabaPcm1024_updateXanyChannels(void);

#endif // PROTO_PCM_H
