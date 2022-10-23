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

#include "PROTO_PCM.h"

#define TR_PCMPROTOS            "PCM Protos"
const pm_char STR_PCMPROTOS[] PROGMEM = TR_PCMPROTOS;

#define TR_PCMFRAME            "Trame PCM"
const pm_char STR_PCMFRAME[] PROGMEM = TR_PCMFRAME;

//const pm_char STR_PCMPROTO[] PROGMEM = " FUT""SPCM"" MPX";
const pm_char STR_PCMPROTO[] PROGMEM = "FUTPCM1K""GRAPCM1K""MPXPCM2K";


const static RfOptionSettingsvar_t RfOpt_FUTPCM1K_Ser[] PROGMEM =
{
 /*rfProtoNeed*/0, //can be PROTO_NEED_SPI | BOOL1USED | BOOL2USED | BOOL3USED
 /*rfSubTypeMax*/2,
 /*rfOptionValue1Min*/0, // FREQFINE MIN
 /*rfOptionValue1Max*/0,  // FREQFINE MAX
 /*rfOptionValue2Min*/0,
 /*rfOptionValue2Max*/0,
 /*rfOptionValue3Max*/0,    // RF POWER
};


/*
Channels scaling:
Width (us) -> PCM1024 Value
       920 -> 0
      2120 -> 1023

 2120 - 920 = 1200
    0 -> 0
 1200 -> 1023
 Coef = 1200 / 1023 = 1.17307
 61/52 = 1.17307 -> Quasi error free, but computing needs to be done with long
 or:
 20/17 = 1.17647 -> 0.3% of error -> Acceptable, and computing can be done with int16_t -> Better
 DeltaBin = DeltaUs x 17 / 20
*/
#define PWM_US_MIN                 920
#define PWM_US_MAX                 2120
#define PWM_US_FULL_EXC            (PWM_US_MAX - PWM_US_MIN)       // 1200 us
#define PWM_US_CENTER              ((PWM_US_MIN + PWM_US_MAX) / 2) // 1520 us

const uint16_t FutPcm1kFailsafeTbl[8] PROGMEM = {PWM_US_CENTER, PWM_US_CENTER, PWM_US_MIN, PWM_US_CENTER, PWM_US_CENTER, PWM_US_CENTER, PWM_US_CENTER, PWM_US_CENTER}  ;

#define FUT_PCM_PACKETS_PER_FRAME  4 // See hereafter

/*
 PcmPacketIdx=0                      PcmPacketIdx=4                      PcmPacketIdx=0
   .---^---.                           .---^---.                           .---^---.
   .-------.-------.-------.-------.   .-------.-------.-------.-------.   .-------.-------.-------.-------.
   | Pos=1 | Pos=3 | Pos=5 | Pos=7 |   | Pos=2 | Pos=4 | Pos=6 | Pos=8 |   | Pos=1 | Pos=3 | Pos=5 | Pos=7 |
   |Delta=2|Delta=4|Delta=6|Delta=8|   |Delta=1|Delta=3|Delta=5|Delta=7|   |Delta=2|Delta=4|Delta=6|Delta=8|
   '-------'-------'-------'-------'   '-------'-------'-------'-------'   '-------'-------'-------'-------'
            Frame1=Odd                          Frame2=Even                         Frame1=Odd
   |---------------------------------|-----------------------------------|-----------------------------------> Time
                                 30ms                                60ms
   PosChId   = [2 x (PcmPacketIdx + 1)] / 2
   DeltaChId = PosChId + 1
                                       DeltaChId = [2 x (PcmPacketIdx - 3)] / 2
                                       PosChId   = DeltaChId + 1
*/

typedef struct{
  int16_t Min;
  int16_t Max;
}MinMaxSt_t;

const MinMaxSt_t DeltaPcmTbl[] PROGMEM = {
                                  /*  0 */{-1023, -116  },
                                  /*  1 */{ -115, -88   },
                                  /*  2 */{  -87, -64   },
                                  /*  3 */{  -63, -44   },
                                  /*  4 */{  -43, -28   },
                                  /*  5 */{  -27, -16   },
                                  /*  6 */{  -15, -8    },
                                  /*  7 */{   -7, -4    },
                                  /*  8 */{   -3, +4    },
                                  /*  9 */{   +5, +8    },
                                  /* 10 */{   +9, +16   },
                                  /* 11 */{  +17, +28   },
                                  /* 12 */{  +29, +44   },
                                  /* 13 */{  +45, +64   },
                                  /* 14 */{  +65, +87   },
                                  /* 15 */{  +88, +1023 },
                                          };

const uint16_t SixToTenTbl[] PROGMEM = {
                              /* 0x00 */ (0b00000011 << 8) | 0b11111000,
                              /* 0x01 */ (0b00000011 << 8) | 0b11110011,
                              /* 0x02 */ (0b00000011 << 8) | 0b11100011,
                              /* 0x03 */ (0b00000011 << 8) | 0b11100111,
                              /* 0x04 */ (0b00000011 << 8) | 0b11000111,
                              /* 0x05 */ (0b00000011 << 8) | 0b11001111,
                              /* 0x06 */ (0b00000011 << 8) | 0b10001111,
                              /* 0x07 */ (0b00000011 << 8) | 0b10011111,
                              /* 0x08 */ (0b00000000 << 8) | 0b11111111,
                              /* 0x09 */ (0b00000000 << 8) | 0b01111111,
                              /* 0x0A */ (0b00000000 << 8) | 0b00111111,
                              /* 0x0B */ (0b00000011 << 8) | 0b00111111,
                              /* 0x0C */ (0b00000011 << 8) | 0b00011111,
                              /* 0x0D */ (0b00000011 << 8) | 0b00001111,
                              /* 0x0E */ (0b00000011 << 8) | 0b10000111,
                              /* 0x0F */ (0b00000011 << 8) | 0b11000011,
                              /* 0x10 */ (0b00000000 << 8) | 0b11111100,
                              /* 0x11 */ (0b00000000 << 8) | 0b11110011,
                              /* 0x12 */ (0b00000000 << 8) | 0b11100111,
                              /* 0x13 */ (0b00000000 << 8) | 0b11001111,
                              /* 0x14 */ (0b00000011 << 8) | 0b11001100,
                              /* 0x15 */ (0b00000011 << 8) | 0b10011100,
                              /* 0x16 */ (0b00000011 << 8) | 0b00111100,
                              /* 0x17 */ (0b00000011 << 8) | 0b00110011,
                              /* 0x18 */ (0b00000011 << 8) | 0b11110000,
                              /* 0x19 */ (0b00000011 << 8) | 0b11100000,
                              /* 0x1A */ (0b00000011 << 8) | 0b10000011,
                              /* 0x1B */ (0b00000011 << 8) | 0b00000111,
                              /* 0x1C */ (0b00000011 << 8) | 0b00011100,
                              /* 0x1D */ (0b00000011 << 8) | 0b10011000,
                              /* 0x1E */ (0b00000011 << 8) | 0b10001100,
                              /* 0x1F */ (0b00000011 << 8) | 0b00111000,
                              /* 0x20 */ (0b00000000 << 8) | 0b11000111,
                              /* 0x21 */ (0b00000000 << 8) | 0b01110011,
                              /* 0x22 */ (0b00000000 << 8) | 0b01100111,
                              /* 0x23 */ (0b00000000 << 8) | 0b11100011,
                              /* 0x24 */ (0b00000000 << 8) | 0b11111000,
                              /* 0x25 */ (0b00000000 << 8) | 0b01111100,
                              /* 0x26 */ (0b00000000 << 8) | 0b00011111,
                              /* 0x27 */ (0b00000000 << 8) | 0b00001111,
                              /* 0x28 */ (0b00000000 << 8) | 0b11001100,
                              /* 0x29 */ (0b00000000 << 8) | 0b11000011,
                              /* 0x2A */ (0b00000000 << 8) | 0b01100011,
                              /* 0x2B */ (0b00000000 << 8) | 0b00110011,
                              /* 0x2C */ (0b00000011 << 8) | 0b00110000,
                              /* 0x2D */ (0b00000011 << 8) | 0b00011000,
                              /* 0x2E */ (0b00000011 << 8) | 0b00001100,
                              /* 0x2F */ (0b00000011 << 8) | 0b00000011,
                              /* 0x30 */ (0b00000000 << 8) | 0b00111100,
                              /* 0x31 */ (0b00000000 << 8) | 0b01111000,
                              /* 0x32 */ (0b00000000 << 8) | 0b11110000,
                              /* 0x33 */ (0b00000000 << 8) | 0b11100000,
                              /* 0x34 */ (0b00000000 << 8) | 0b11000000,
                              /* 0x35 */ (0b00000011 << 8) | 0b11000000,
                              /* 0x36 */ (0b00000011 << 8) | 0b10000000,
                              /* 0x37 */ (0b00000011 << 8) | 0b00000000,
                              /* 0x38 */ (0b00000000 << 8) | 0b01100000,
                              /* 0x39 */ (0b00000000 << 8) | 0b01110000,
                              /* 0x3A */ (0b00000000 << 8) | 0b00110000,
                              /* 0x3B */ (0b00000000 << 8) | 0b00111000,
                              /* 0x3C */ (0b00000000 << 8) | 0b00011000,
                              /* 0x3D */ (0b00000000 << 8) | 0b00011100,
                              /* 0x3E */ (0b00000000 << 8) | 0b00001100,
                              /* 0x3F */ (0b00000000 << 8) | 0b00000111,
                                      };

#define six2ten(SixIdx)    (uint16_t)pgm_read_word_far(&SixToTenTbl[(SixIdx)])  // Convert six bits to ten bits

const uint8_t PcmCrcTbl[16] PROGMEM = {0x6B, 0xD6, 0xC7, 0xE5, 0xA1, 0x29, 0x52, 0xA4,
                                       0x23, 0x46, 0x8C, 0x73, 0xE6, 0xA7, 0x25, 0x4A};

#define FUT_24_BIT_PCM_PACKET_BIT_NB          24

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

#define FUT_40_BIT_RADIO_PCM_PACKET_BIT_NB    40

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

#define FUT_PCM_BIT_DURATION_HALF_US    (150 * 2) // 1 bit duration is 150us -> 2 x 2MHz Tick = 300 Ticks (Half us)

const uint16_t ConsecBitDurationHalfUs[] PROGMEM = {
                                         /*  0 */ (18 * FUT_PCM_BIT_DURATION_HALF_US), /* Index 0 is for Sync (18 bits) */
                                         /*  1 */ (1  * FUT_PCM_BIT_DURATION_HALF_US),
                                         /*  2 */ (2  * FUT_PCM_BIT_DURATION_HALF_US),
                                         /*  3 */ (3  * FUT_PCM_BIT_DURATION_HALF_US),
                                         /*  4 */ (4  * FUT_PCM_BIT_DURATION_HALF_US),
                                         /*  5 */ (5  * FUT_PCM_BIT_DURATION_HALF_US),
                                         /*  6 */ (6  * FUT_PCM_BIT_DURATION_HALF_US),
                                         /*  7 */ (7  * FUT_PCM_BIT_DURATION_HALF_US),
                                         /*  8 */ (8  * FUT_PCM_BIT_DURATION_HALF_US),
                                         /*  9 */ (9  * FUT_PCM_BIT_DURATION_HALF_US),
                                         /* 10 */ (10 * FUT_PCM_BIT_DURATION_HALF_US),
                                         /* 11 */ (11 * FUT_PCM_BIT_DURATION_HALF_US),
                                         /* 12 */ (12 * FUT_PCM_BIT_DURATION_HALF_US),
                                         /* 13 */ (13 * FUT_PCM_BIT_DURATION_HALF_US),
                                         /* 14 */ (14 * FUT_PCM_BIT_DURATION_HALF_US),
                                         /* 15 */ (15 * FUT_PCM_BIT_DURATION_HALF_US),
                                                  };

#define HALF_TX_NBL_IDX                 30 // Around the middle of the bit stream nibble buffer (Futaba.Pcm1024.StreamConsecBitTbl[])

/* PRIVATE FUNCTION PROTOTYPES */
static uint16_t UsToPcmValue(int16_t PwmUs, uint8_t Delta);
static uint8_t  DeltaUsToDeltaCode(int16_t DeltaUs);
static inline void    PcmStreamSetConsecBitNb(uint8_t BufIdx, uint8_t NblIdx, uint8_t ConsecBitNb);
static inline uint8_t PcmStreamGetConsecBitNb(uint8_t BufIdx, uint8_t NblIdx);

/* PUBLIC FUNCTIONS */
#warning TO DO: if PCM1024 is the selected protocol, call FutabaPcm1024_updateXanyChMap() when Xany Channel is validated
void FutabaPcm1024_updateXanyChMap(void)
{
	uint8_t ChIdx;

	Futaba.Pcm1024.XanyChMap = 0;
	for(uint8_t XanyIdx = 0; XanyIdx < NUM_X_ANY; XanyIdx++)
	{
		if(g_model.Xany[XanyIdx].Active)
		{
			ChIdx = g_model.Xany[XanyIdx].ChId;
			if(ChIdx < FUT_PCM1024_PROP_CH_NB)
			{
				Futaba.Pcm1024.XanyChMap |= (1 << ChIdx);
			}
		}
	}
}

void FutabaPcm1024_buildHalfRadioPcmBitStream(void)
{
  uint8_t  PacketIdx = 0, PosChIdx, DeltaChIdx, IsOdd, BitIdx, BitCnt;
  uint64_t BitMask;
  Fut24BitPcmPacket_Union      Fut24BitPcmPacket;
  Fut40BitRadioPcmPacket_Union Fut40BitRadioPcmPacket[FUT_PCM_PACKETS_PER_FRAME / 2];

  // Here, we will build 2 PCM Packets at a time to share the Bit Stream buffer with ISR
  if(Futaba.Pcm1024.BuildState == FUT_PCM1024_BUILD_2_FIRST_PACKETS)
  {
    IsOdd = (Futaba.Pcm1024.PacketIdx < FUT_PCM_PACKETS_PER_FRAME); // Futaba.Pcm1024.PacketIdx 0 to 3 -> frame is odd
    // OK: the 4 Radio PCM Packets are ready -> Build the Bit stream of the Radio PCM Frame
    Futaba.Pcm1024.BuildNblIdx    = 0;
    // Preamble -> Odd: 1100, Even: 110000
    PcmStreamSetConsecBitNb(!Futaba.Pcm1024.IsrBufIdx, Futaba.Pcm1024.BuildNblIdx++, 2);PcmStreamSetConsecBitNb(!Futaba.Pcm1024.IsrBufIdx, Futaba.Pcm1024.BuildNblIdx++, IsOdd? 2: 4);
    // Sync Pulse: 18 x '1' of 150 us
    PcmStreamSetConsecBitNb(!Futaba.Pcm1024.IsrBufIdx, Futaba.Pcm1024.BuildNblIdx++, 0); // 0 means 18 as ConsecBitNb coded on 4 bits -> Take this into account in ISR
    // Odd and Even Frame Code -> Odd: 00000011,  Even: 000011
    PcmStreamSetConsecBitNb(!Futaba.Pcm1024.IsrBufIdx, Futaba.Pcm1024.BuildNblIdx++, IsOdd? 6: 4);PcmStreamSetConsecBitNb(!Futaba.Pcm1024.IsrBufIdx, Futaba.Pcm1024.BuildNblIdx++, 2);
    // Continue to populate the Bit stream table with the 4 x Fut40BitRadioPcmPacket
    Futaba.Pcm1024.BitVal = 1;
  }
  while (PacketIdx < (FUT_PCM_PACKETS_PER_FRAME / 2))
  {
    Fut24BitPcmPacket.AuxBit1 = !(Futaba.Pcm1024.PacketIdx & 1);
    Fut24BitPcmPacket.AuxBit0 = 0;
    if(Futaba.Pcm1024.PacketIdx < FUT_PCM_PACKETS_PER_FRAME)
    {
      PosChIdx   = (2 * (Futaba.Pcm1024.PacketIdx % FUT_PCM_PACKETS_PER_FRAME)); // PosChIdx = 0, 2, 4, 6 -> ChId = 1, 3, 5, 7
      DeltaChIdx = PosChIdx + 1;                   // DeltaChIdx = 1, 3, 5, 7 -> ChId = 2, 4, 6, 8
    }
    else
    {
      DeltaChIdx = (2 * (Futaba.Pcm1024.PacketIdx % FUT_PCM_PACKETS_PER_FRAME)); // DeltaChIdx = 0, 2, 4, 6 -> ChId = 1, 3, 5, 7
      PosChIdx   = DeltaChIdx + 1;                 // PosChIdx = 1, 3, 5, 7 -> ChId = 2, 4, 6, 8
    }
    Fut24BitPcmPacket.Position  = UsToPcmValue(PPM_CENTER + (channelOutputs[PosChIdx] / 2), 0);
		Fut24BitPcmPacket.Delta = DeltaUsToDeltaCode((channelOutputs[DeltaChIdx] - Futaba.Pcm1024.MemoChOutputs[DeltaChIdx]) / 2);
		if(Futaba.Pcm1024.XanyChMap & (1 << DeltaChIdx))
		{
			Fut24BitPcmPacket.Delta = 8; // X-Any cannot work with "smooth" variation (so delta (in us) between 2 Positions shall be almost null)
		}
    Futaba.Pcm1024.MemoChOutputs[DeltaChIdx] = channelOutputs[DeltaChIdx]; // MemoChOutputs[] is used to compute Deltas
    //Compute ECC
    Fut24BitPcmPacket.Ecc = 0;
    for(uint8_t BitIdx = 0; BitIdx < 16; BitIdx++)
    {
      if(Fut24BitPcmPacket.Value & (1UL << (8 + BitIdx)))
      {
        Fut24BitPcmPacket.Ecc ^= (uint8_t)pgm_read_byte_far(&PcmCrcTbl[BitIdx]);
      }
    }
    // OK: PCM Frame is ready -> Build Radio PCM Packets by translating 6 bits to 10 bits
    Fut40BitRadioPcmPacket[Futaba.Pcm1024.PacketIdx % 2].TenBitBlock0 = six2ten(Fut24BitPcmPacket.SixBitBlock0);
    Fut40BitRadioPcmPacket[Futaba.Pcm1024.PacketIdx % 2].TenBitBlock1 = six2ten(Fut24BitPcmPacket.SixBitBlock1);
    Fut40BitRadioPcmPacket[Futaba.Pcm1024.PacketIdx % 2].TenBitBlock2 = six2ten(Fut24BitPcmPacket.SixBitBlock2);
    Fut40BitRadioPcmPacket[Futaba.Pcm1024.PacketIdx % 2].TenBitBlock3 = six2ten(Fut24BitPcmPacket.SixBitBlock3);
    Futaba.Pcm1024.PacketIdx++; // Futaba.Pcm1024.PacketIdx will automatically overlap to 0 as soon it will reach 8 (since it is coded on 3 bits)
    PacketIdx++;
  }
  for(PacketIdx = 0; PacketIdx < (FUT_PCM_PACKETS_PER_FRAME / 2); PacketIdx++)
  {
    Futaba.Pcm1024.BuildNblIdx--;
    BitCnt = PcmStreamGetConsecBitNb(!Futaba.Pcm1024.IsrBufIdx, Futaba.Pcm1024.BuildNblIdx); // Retrieve last BitCnt in case subsequent bit(s) is(are) identical

    for(BitIdx = (FUT_40_BIT_RADIO_PCM_PACKET_BIT_NB - 1); BitIdx < 255; BitIdx--)
    {
      BitMask = (1LL << BitIdx);
      if(Futaba.Pcm1024.BitVal == !!(Fut40BitRadioPcmPacket[PacketIdx].Value & BitMask))
      {
        BitCnt++;
      }
      else
      {
        PcmStreamSetConsecBitNb(!Futaba.Pcm1024.IsrBufIdx, Futaba.Pcm1024.BuildNblIdx++, BitCnt);
        Futaba.Pcm1024.BitVal = !Futaba.Pcm1024.BitVal;
        BitCnt = 1;
      }
    }
    PcmStreamSetConsecBitNb(!Futaba.Pcm1024.IsrBufIdx, Futaba.Pcm1024.BuildNblIdx++, BitCnt); // Last bit(s)
  }
  if(!(Futaba.Pcm1024.PacketIdx % FUT_PCM_PACKETS_PER_FRAME))
  {
    Futaba.Pcm1024.BuildEndNblIdx[!Futaba.Pcm1024.IsrBufIdx] = Futaba.Pcm1024.BuildNblIdx - 1; // Used by ISR for frame lenght
  }
  if(Futaba.Pcm1024.BuildState == FUT_PCM1024_BUILD_2_LAST_PACKETS)
  {
    // Schedule here since this part is shorter than the first part (no Preamble, no Sync, no Odd/Even code)
#if defined(X_ANY)
    Xany_scheduleTx_AllInstance();
#endif
  }
  Futaba.Pcm1024.BuildState = FUT_PCM1024_BUILD_DO_NOTHING;
}

/* PRIVATE FUNCTIONS */
static uint16_t UsToPcmValue(int16_t PwmUs, uint8_t Delta)
{
  int16_t PcmBin;

  if(!Delta)
  {
    PwmUs -= PWM_US_MIN;
    if(PwmUs < 0)               PwmUs = 0;
    if(PwmUs > PWM_US_FULL_EXC) PwmUs = PWM_US_FULL_EXC;
  }
  PcmBin = ((PwmUs * 17) / 20); // 20/17 = 1.1764 -> Can use int16_t for computation

  return(PcmBin);
}

static uint8_t DeltaUsToDeltaCode(int16_t DeltaUs) // DeltaUs = NewWidthUs - PrevWidthUs
{
  int16_t SignedDeltaPcm, Min, Max;
  uint8_t Idx;

  SignedDeltaPcm = UsToPcmValue(DeltaUs, 1);
  for(Idx = 0; Idx < TBL_ITEM_NB(DeltaPcmTbl); Idx++)
  {
    Min = (int16_t)pgm_read_word(&DeltaPcmTbl[Idx].Min);
    Max = (int16_t)pgm_read_word(&DeltaPcmTbl[Idx].Max);
    if(SignedDeltaPcm >= Min && SignedDeltaPcm <= Max) break;
  }
  if(Idx >= TBL_ITEM_NB(DeltaPcmTbl)) Idx = 8; //Error -> Do NOT move

  return(Idx);
}

static inline void PcmStreamSetConsecBitNb(uint8_t BufIdx, uint8_t NblIdx, uint8_t ConsecBitNb)
{
  if(NblIdx <= PCM_NBL_MAX_IDX)
  {
    if(NblIdx & 1) Futaba.Pcm1024.StreamConsecBitTbl[BufIdx][NblIdx / 2].LowNibbleBitNb  = ConsecBitNb;
    else           Futaba.Pcm1024.StreamConsecBitTbl[BufIdx][NblIdx / 2].HighNibbleBitNb = ConsecBitNb;
  }
}

static inline uint8_t PcmStreamGetConsecBitNb(uint8_t BufIdx, uint8_t NblIdx)
{
  if(NblIdx & 1) return(Futaba.Pcm1024.StreamConsecBitTbl[BufIdx][NblIdx / 2].LowNibbleBitNb);
  else           return(Futaba.Pcm1024.StreamConsecBitTbl[BufIdx][NblIdx / 2].HighNibbleBitNb);
}
#if 0
/*
 * PROTO_FUTPCM1K uses PB6 OC1B
 * 16 Bit Timer running @ 16MHz has a resolution of 0.5us.
 * This should give a PPM resolution of 2048.
*/
static uint16_t PROTO_FUTPCM1K_cb2() // Needs to be renamed PROTO_FUTABA_PCM1024_cb2()
{
  if(*RptrB == 0) { // End of timing events.
    RptrB = &pulses2MHz.pword[0];
    // Set the PPM idle level.
    if (g_model.PULSEPOL) {
      TCCR1A = (TCCR1A | (1<<COM1B1)) & ~(1<<COM1B0); // Clear
    }
    else {
      TCCR1A |= (0b11<<COM1B0); // Set
    }
    TCCR1C = 1<<FOC1B; // Strobe FOC1.
    TCCR1A = (TCCR1A | (1<<COM1B0)) & ~(1<<COM1B1); // Toggle OC1x on next match.

    // Schedule next Mixer calculations.
    SCHEDULE_MIXER_END_IN_US(22500 + ((g_model.PPMFRAMELENGTH * 1000) / 2));
    setupPulsesPPM(PPM);
    heartbeat |= HEART_TIMER_PULSES;
    dt = TCNT1 - OCR1B; // Show how long to setup pulses and ISR jitter.
    return PULSES_SETUP_TIME_US *2;
  }
  else if (*(RptrB +1) == 0) { // Look ahead one timing event.
    // Need to prevent next toggle.
    // Read pin and store before disconnecting switching output.
    if(PINB & PIN6_bm) PORTB |= PIN6_bm;
    else PORTB &= ~PIN6_bm;
    TCCR1A &= ~(0b11<<COM1B0);
    return *RptrB++;
  }
  else // Toggle pin.
    return *RptrB++;
}
#endif

void PROTO_FUTPCM1K_cb1() // Needs to be renamed PROTO_FUTABA_PCM1024_cb1()
{
#if 0
uint16_t half_us = PROTO_FUTPCM1K_cb2();

  if(! half_us) {
    PROTO_FUTPCM1K_Cmds(PROTOCMD_RESET);
    return;
  }

//  dt = TCNT1 - OCR1B; // Calculate latency and jitter.
  if(dt > g_tmr1Latency_max) g_tmr1Latency_max = dt;
  if(dt < g_tmr1Latency_min) g_tmr1Latency_min = dt;

  OCR1B += half_us;
#endif
  uint8_t  ConsecBitNb;
  uint16_t half_us;

  ConsecBitNb = PcmStreamGetConsecBitNb(Futaba.Pcm1024.IsrBufIdx, Futaba.Pcm1024.TxNblIdx);
  //half_us = (uint16_t)pgm_read_word_far(&ConsecBitDurationHalfUs[ConsecBitNb]); // Use pre-computed values
  uint_farptr_t temp = pgm_get_far_address(ConsecBitDurationHalfUs);
  half_us = (uint16_t)pgm_read_word_far(temp +ConsecBitNb); // Use pre-computed values

  OCR1B  += half_us;
  if(Futaba.Pcm1024.TxNblIdx >= Futaba.Pcm1024.BuildEndNblIdx[Futaba.Pcm1024.IsrBufIdx])
  {
		Futaba.Pcm1024.TxNblIdx = 255; // Will become 0 after incrementation
		Futaba.Pcm1024.BuildEndNblIdx[Futaba.Pcm1024.IsrBufIdx] = 0; // Free marker
		Futaba.Pcm1024.IsrBufIdx = !Futaba.Pcm1024.IsrBufIdx; // Buffer Flip
    // Schedule next Mixer calculations.
    SCHEDULE_MIXER_END_IN_US(FUT_PCM1024_FRAME_PERIOD_US);
    heartbeat |= HEART_TIMER_PULSES;
  }
  Futaba.Pcm1024.TxNblIdx++;
}


static void PROTO_FUTPCM1K_reset() // Needs to be renamed PROTO_FUTABA_PCM1024_reset()
{
  // Make pin idle state before disconnecting switching output.
  if(g_model.PULSEPOL) PORTB &= ~PIN6_bm;
  else PORTB |= PIN6_bm;
  TCCR1A &= ~(0b11<<COM1B0);
  TIMSK1 &= ~(1<<OCIE1B); // Disable Output Compare B interrupt.
  TIFR1 |= 1<<OCF1B; // Reset Flag.

  Futaba.Pcm1024.BuildState = FUT_PCM1024_BUILD_DO_NOTHING;
}

static void PROTO_FUTPCM1K_initialize() // Needs to be renamed PROTO_FUTABA_PCM1024_initialize()
{
//if defined(FRSKY)
//  telemetryPPMInit();
//#endif
  CheckPCMPeriod();

  Futaba.Pcm1024.BuildState     = FUT_PCM1024_BUILD_DO_NOTHING;
  Futaba.Pcm1024.PacketIdx      = 0;
  Futaba.Pcm1024.BitVal         = 0;
  Futaba.Pcm1024.BuildNblIdx    = 0;
  Futaba.Pcm1024.IsrBufIdx      = 0;
  Futaba.Pcm1024.BuildEndNblIdx[ Futaba.Pcm1024.IsrBufIdx] = PCM_NBL_MAX_IDX;
  Futaba.Pcm1024.BuildEndNblIdx[!Futaba.Pcm1024.IsrBufIdx] = 0;
  Futaba.Pcm1024.TxNblIdx       = HALF_TX_NBL_IDX;
  memset(&Futaba.Pcm1024.StreamConsecBitTbl, 0x22, sizeof(Futaba.Pcm1024.StreamConsecBitTbl));
	FutabaPcm1024_updateXanyChMap();
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    ocr1b_function_ptr = PROTO_FUTPCM1K_cb1; // Setup function pointer used in ISR.
/* vvv This cfg needs to be checked vvv */
//#warning Check the timer config to toggle on next match!
    TCCR1A = (TCCR1A | (1<<COM1B1)) & ~(1<<COM1B0); // Clear
    TCCR1C = 1<<FOC1B; // Strobe FOC1.
    TCCR1A = (TCCR1A | (1<<COM1B0)) & ~(1<<COM1B1); // Toggle OC1x on next match.
/* ^^^ This cfg needs to be checked ^^^ */

    OCR1B = TCNT1 + (FUT_PCM1024_FRAME_PERIOD_US *2);
    TIFR1  |= 1<<OCF1B;  // Reset Flag.
    TIMSK1 |= 1<<OCIE1B; // Enable Output Compare interrupt.
  }
}

const void * PROTO_PCM_Cmds(enum ProtoCmds cmd) // Needs to be renamed PROTO_FUTABA_PCM1024_Cmds()
{
  switch(cmd) {
    case PROTOCMD_INIT: PROTO_FUTPCM1K_initialize();
    return 0;
    case PROTOCMD_RESET:
      PROTO_FUTPCM1K_reset();
    return 0;
  case PROTOCMD_GETOPTIONS:
   SetRfOptionSettings(pgm_get_far_address(RfOpt_FUTPCM1K_Ser),
                       STR_PCMPROTO,   //FUT, SPCM and MPX
                       STR_DUMMY,      //Option 1 (int)
                       STR_DUMMY,      //Option 2 (int)
                       STR_DUMMY,      //Option 3 (uint 0 to 31)
                       STR_DUMMY,      //OptionBool 1
                       STR_DUMMY,      //OptionBool 2
                       STR_DUMMY       //OptionBool 3
                      );
   return 0;
  default: break;
  }
  return 0;
}

