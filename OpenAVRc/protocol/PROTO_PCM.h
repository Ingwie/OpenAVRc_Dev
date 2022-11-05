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

#ifndef PCM_H
#define PCM_H

#include "../thirdparty/pt/pt.h" // Include Protothread header

#define FUT_PCM1024_PROP_CH_NB       8

#define FUT_PCM1024_FRAME_PERIOD_US  28500U

#define PcmProto g_model.rfSubType
uint16_t CheckPCMPeriod()
{
  if (PcmProto == 0)//Futaba PCM1024
  {
    return FUT_PCM1024_FRAME_PERIOD_US;
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

union Fut24BitPcmPacket_Union
{
  uint32_t  Value;
  struct {
  uint32_t
            Ecc:          8,  // ^
            Position:     10, // |
            Delta:        4,  // |  24
            AuxBit0:      1,  // | bits
            AuxBit1:      1,  // v
            Reserved:     8;
  };
  struct {
  uint32_t
            SixBitBlock3: 6,  // ^
            SixBitBlock2: 6,  // |  24
            SixBitBlock1: 6,  // | bits
            SixBitBlock0: 6,  // v
            Reserved2:    8;
  };
}__attribute__((__packed__));

union Fut40BitRadioPcmPacket_Union
{
  uint64_t Value;
  struct{
  uint64_t
           TenBitBlock3: 10,
           TenBitBlock2: 10,
           TenBitBlock1: 10,
           TenBitBlock0: 10,
           Reserved:     24;
  };
}__attribute__((__packed__));

#define FUT_PCM_PACKETS_PER_FRAME  4 // See hereafter

typedef struct{
  uint8_t                      PacketIdx;
  uint8_t                      BitIdx;
  uint8_t                      BitCnt;
  Fut24BitPcmPacket_Union      Fut24BitPcmPacket;
  Fut40BitRadioPcmPacket_Union Fut40BitRadioPcmPacket[FUT_PCM_PACKETS_PER_FRAME / 2];
}FutPtCtx_t; // ProtoThread context (since context cannot be saved locally in the ProtoThread)

typedef struct{
  uint8_t
					                   BuildState:   3, // 0: Nothing to do, 1: Channels 0 & 1 or 4 & 5 , 2: Channels 2 & 3 or 6 & 7
					                   PacketIdx:    3,
					                   IsrBufIdx:    1,
					                   BitVal:       1;
  uint8_t                    BuildNblIdx;
  uint8_t                    BuildEndNblIdx[2]; // For the ISR to know PCM frame is fully sent
  uint8_t                    TxNblIdx;
	uint8_t                    XanyChMap; // One bit per Channel (8 channels max from bit0 to bit7)
  int16_t                    MemoChOutputs[FUT_PCM1024_PROP_CH_NB]; // Used to compute Deltas
  PcmStreamBitNbCouple_Union StreamConsecBitTbl[2][PCM_STREAM_BYTE_NB]; // Double buffering (One buffer is filled during the other is transmitting)
  FutPtCtx_t   PtCtx; // Specific to Futaba
}PcmSt_t;

#define Proto    pulses2MHz // Just for readability (Proto structure is an union of pulses2MHz buffer)

/* PUBLIC FUNCTION PROTOTYPES */
#if defined(X_ANY)
void Pcm_updateXanyChMap(void);
#endif

#endif // PCM_H
