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

#define US_TO_PROTO_TICK(us)   ((us) * 2) // Here, Tick is half us
#define SLICE_DURATION_TICK    US_TO_PROTO_TICK(128) // 128 us converted into timer ticks
#define GET_PROTO_TICK()       TCNT1

/* Custom Macro to extend protothreads features: this macro allows executing protothread by time slice */
#define PT_YIELD_AFTER_TICK(pt, CurTick, StartTick, DurationInTick)  PT_WAIT_UNTIL(pt, ((CurTick) - (StartTick)) < (DurationInTick));

#define TR_PCMFRAME            "Trame PCM"
const pm_char STR_PCMFRAME[] PROGMEM = TR_PCMFRAME;

const pm_char STR_PCMPROTO[] PROGMEM = "FUTPCM1K""GRAPCM1K""MPPCM256";

const static RfOptionSettingsvar_t RfOpt_PCM_Ser[] PROGMEM =
{
 /*rfProtoNeed*/0, //can be PROTO_NEED_SPI | BOOL1USED | BOOL2USED | BOOL3USED
 /*rfSubTypeMax*/PCM_PROTO_NB - 1,
 /*rfOptionValue1Min*/0, // FREQFINE MIN
 /*rfOptionValue1Max*/0,  // FREQFINE MAX
 /*rfOptionValue2Min*/0,
 /*rfOptionValue2Max*/0,
 /*rfOptionValue3Max*/0,    // RF POWER
};

/*************************************************/
/*                 FUTABA PCM1024                */
/*************************************************/
/*
Documentation credit: W.Pasman (Futaba PCM1024 protocol description)
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
#define FUT_PWM_US_MIN                 920
#define FUT_PWM_US_MAX                 2120
#define FUT_PWM_US_FULL_EXC            (FUT_PWM_US_MAX - FUT_PWM_US_MIN)       // 1200 us
#define FUT_PWM_US_CENTER              ((FUT_PWM_US_MIN + FUT_PWM_US_MAX) / 2) // 1520 us

/*
 PcmPacketIdx goes from 0 to 7

 PcmPacketIdx=0                      PcmPacketIdx=4                      PcmPacketIdx=0
   .---^---.                           .---^---.                           .---^---.
   .-------.-------.-------.-------.   .-------.-------.-------.-------.   .-------.-------.-------.-------.
   | Pos=1 | Pos=3 | Pos=5 | Pos=7 |   | Pos=2 | Pos=4 | Pos=6 | Pos=8 |   | Pos=1 | Pos=3 | Pos=5 | Pos=7 |
   |Delta=2|Delta=4|Delta=6|Delta=8|   |Delta=1|Delta=3|Delta=5|Delta=7|   |Delta=2|Delta=4|Delta=6|Delta=8|
   '-------'-------'-------'-------'   '-------'-------'-------'-------'   '-------'-------'-------'-------'
            Frame1=Odd                          Frame2=Even                         Frame1=Odd
   |---------------------------------|-----------------------------------|-----------------------------------> Time
                                 30ms                                60ms
   PosChId   = 1 + [2 x (PcmPacketIdx % 4)]
   DeltaChId = PosChId + 1
                                       DeltaChId = 1 + [2 x (PcmPacketIdx % 4)]
                                       PosChId   = DeltaChId + 1
*/

typedef struct{
  int16_t Min;
  int16_t Max;
}MinMaxSt_t;

const MinMaxSt_t FutDeltaPcmTbl[] PROGMEM = {
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

const uint16_t FutSixToTenTbl[] PROGMEM = {
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

#define FutSix2Ten(SixIdx)    (uint16_t)pgm_read_word_far(&FutSixToTenTbl[(SixIdx)])  // Convert six bits to ten bits

const uint8_t FutPcmCrcTbl[16] PROGMEM = {0x6B, 0xD6, 0xC7, 0xE5, 0xA1, 0x29, 0x52, 0xA4,
                                          0x23, 0x46, 0x8C, 0x73, 0xE6, 0xA7, 0x25, 0x4A};

#define FUT_24_BIT_PCM_PACKET_BIT_NB          24

#define FUT_40_BIT_RADIO_PCM_PACKET_BIT_NB    40

#define FUT_PCM_BIT_DURATION_HALF_US          US_TO_PROTO_TICK(150) // 1 bit duration is 150us -> 2 x 2MHz Tick = 300 Ticks (Half us)

const uint16_t FutConsecBitDurationHalfUs[] PROGMEM = {
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

/* FUTABA PRIVATE FUNCTION PROTOTYPES */
static                PT_THREAD(FutBuildRadioPcmBitStream(struct pt *pt));
static uint16_t       FutUsToPcmValue(int16_t PwmUs, uint8_t Delta);
static uint8_t        FutDeltaUsToDeltaCode(int16_t DeltaUs);

/*************************************************/
/*                GRAUPNER PCM1024 (S-PCM)       */
/*************************************************/
// Will be implemented later

/*************************************************/
/*                MULTIPLEX PCM256               */
/*************************************************/
/*
Documentation credit: Marius Greuel https://github.com/rc-hacks/mpx-pcm-spec (Multiplex PCM256 protocol description)

Channels scaling:
Width (us) -> PCM256 Value
      1050 -> 0
      2150 -> 255
 2150 - 1050 = 1100
    0 -> 0
 1100 -> 255

 PulseWidth(us) = 1050 + 550 x PcmValue / 128
 Coef = 550 / 128 = 4.297 = 43 / 10
 43/10 = 4.3 -> Quasi error free
 PulseWidth(us) = 1050 + PcmValue x 43 / 10
 PcmValue = (PulseWidth(us) - 1050) x 10 / 43
*/
enum {MPX_PCM256_BUILD_FRAME_WITH_CH7_AND_CH8 = 0, MPX_PCM256_BUILD_FRAME_WITH_CH9_AND_CH10};

#define MPX_PWM_US_MIN                 1050
#define MPX_PWM_US_MAX                 2150
#define MPX_PWM_US_FULL_EXC            (MPX_PWM_US_MAX - MPX_PWM_US_MIN)       // 1100 us
#define MPX_PWM_US_CENTER              ((MPX_PWM_US_MIN + MPX_PWM_US_MAX) / 2) // 1600 us

/*
  --.            .----------.      .-------.      .-----------.      .-- ...
    |            |          |      |       |      |           |      |
    '------------'          '------'       '------'           '------'

    |<--TLsync-->|<-THsync->|<-TL->|       |<-TL->|
                            |<----TP0----->|<-------TP1------>|<-----TP2- ...
  The frame starts with a sync low-pulse, which is TLsync=1000µs long, followed by a high-pulse THsync=620µs.
  All following low-pulses are TL=375µs long, followed by a variable-length high-pulse. Similar to PPM,
  the bits are encoded by the width of the period TPx (i.e. the time between two falling edges), depicted as TP0, TP1, TP2, ...
*/

#define TL_SYNC_US    1000
#define TH_SYNC_US     620
#define TL_US          375

const uint16_t MpxConsecBitDurationHalfUs[] PROGMEM = {
                                         /* 0: S0         */ US_TO_PROTO_TICK( 880 - TL_US),
                                         /* 1: S1         */ US_TO_PROTO_TICK(1020 - TL_US),
                                         /* 2: S2         */ US_TO_PROTO_TICK(1160 - TL_US),
                                         /* 3: S3         */ US_TO_PROTO_TICK(1300 - TL_US),
                                         /* 4: S4         */ US_TO_PROTO_TICK(1440 - TL_US),
                                         /* 5: S5         */ US_TO_PROTO_TICK(1580 - TL_US),
                                         /* 6: S6         */ US_TO_PROTO_TICK(1720 - TL_US),
                                         /* 7: TL_SYNC_US */ US_TO_PROTO_TICK(TL_SYNC_US),
                                         /* 8: TH_SYNC_US */ US_TO_PROTO_TICK(TH_SYNC_US),
                                         /* 9: TL         */ US_TO_PROTO_TICK(TL_US),
                                                    };

#define TL_SYNC_IDX    7
#define TH_SYNC_IDX    8
#define TL_IDX         9
#define GAP_IDX        6

enum {MPX_S0 = 0, MPX_S1, MPX_S2, MPX_S3, MPX_S4, MPX_S5, MPX_S6, MPX_S7, MPX_S_NB};

enum {MPX_SET_A = 0, MPX_SET_B, MPX_SET_C, MPX_SET_D};

const uint8_t MpxSet_Tbl[4][4] PROGMEM = {{MPX_S0, MPX_S1, MPX_S2, MPX_S3}, {MPX_S1, MPX_S2, MPX_S3, MPX_S4}, {MPX_S2, MPX_S3, MPX_S4, MPX_S5}, {MPX_S3, MPX_S4, MPX_S5, MPX_S6}};
#define MpxSymbol(SetIdx, BitPair)    (uint8_t)pgm_read_byte_far(&MpxSet_Tbl[SetIdx][BitPair])

#define B00            0
#define B01            1
#define B10            2
#define B11            3
#define B00000011      3
#define B11000000      (3 << 6)

/* MULTIPLEX PRIVATE FUNCTION PROTOTYPES */
static                PT_THREAD(MpxBuildRadioPcmBitStream(struct pt *pt));
static uint8_t        MpxGetSetIdx(uint8_t PrevBitPair);
static uint8_t        MpxUsToPcmValue(uint16_t PwmUs);


/* COMMON PRIVATE FUNCTION PROTOTYPES */
static inline void    PcmStreamSetConsecBitNb(uint8_t BufIdx, uint8_t NblIdx, uint8_t ConsecBitNb);
static inline uint8_t PcmStreamGetConsecBitNb(uint8_t BufIdx, uint8_t NblIdx);

static struct pt pcm_pt; // Protothread line continuation storage (Common to all the PCM variants)

/* PUBLIC FUNCTIONS */
#if defined(X_ANY)
void Pcm_updateXanyChMap(void)
{
	uint8_t ChIdx;

	Proto.Pcm.XanyChMap = 0;
	for(uint8_t XanyIdx = 0; XanyIdx < NUM_X_ANY; XanyIdx++)
	{
		if(g_model.Xany[XanyIdx].Active)
		{
			ChIdx = g_model.Xany[XanyIdx].ChId;
			if(ChIdx < FUT_PCM1024_PROP_CH_NB)
			{
				Proto.Pcm.XanyChMap |= (1 << ChIdx);
			}
		}
	}
}
#endif

/* FUTABA PRIVATE FUNCTIONS */

static PT_THREAD(FutBuildRadioPcmBitStream(struct pt *pt)) // This protothread is executed time slice per time slice (of 128 us)
{
  uint8_t  PosChIdx, DeltaChIdx, IsOdd;
  uint64_t BitMask;
  uint16_t StartTick; // SHALL be local variables

  StartTick = GET_PROTO_TICK();

  PT_BEGIN(pt);
  if(!Proto.Pcm.BuildEndNblIdx[!Proto.Pcm.IsrBufIdx])//If buf not used by ISR is not already filled
  {
    // Here, we will build 2 PCM Packets at a time to share the Bit Stream buffer with ISR
    if(Proto.Pcm.BuildState == FUT_PCM1024_BUILD_2_FIRST_PACKETS)
    {
      IsOdd = (Proto.Pcm.PacketIdx < FUT_PCM_PACKETS_PER_FRAME); // Proto.Pcm.PacketIdx 0 to 3 -> frame is odd
      // OK: the 4 Radio PCM Packets are ready -> Build the Bit stream of the Radio PCM Frame
      Proto.Pcm.BuildNblIdx = 0;
      // Preamble -> Odd: 1100, Even: 110000
      PcmStreamSetConsecBitNb(!Proto.Pcm.IsrBufIdx, Proto.Pcm.BuildNblIdx++, 2);PcmStreamSetConsecBitNb(!Proto.Pcm.IsrBufIdx, Proto.Pcm.BuildNblIdx++, IsOdd? 2: 4);
      // Sync Pulse: 18 x '1' of 150 us
      PcmStreamSetConsecBitNb(!Proto.Pcm.IsrBufIdx, Proto.Pcm.BuildNblIdx++, 0); // 0 means 18 as ConsecBitNb coded on 4 bits -> Take this into account in ISR
      // Odd and Even Frame Code -> Odd: 00000011,  Even: 000011
      PcmStreamSetConsecBitNb(!Proto.Pcm.IsrBufIdx, Proto.Pcm.BuildNblIdx++, IsOdd? 6: 4);PcmStreamSetConsecBitNb(!Proto.Pcm.IsrBufIdx, Proto.Pcm.BuildNblIdx++, 2);
      // Continue to populate the Bit stream table with the 4 x Fut40BitRadioPcmPacket
      Proto.Pcm.BitVal = 1;
    }// 20us
    Proto.Pcm.PtCtx.PacketIdx = 0;
    while (Proto.Pcm.PtCtx.PacketIdx < (FUT_PCM_PACKETS_PER_FRAME / 2))
    {
      Proto.Pcm.PtCtx.Fut24BitPcmPacket.AuxBit1 = !(Proto.Pcm.PacketIdx & 1);
      Proto.Pcm.PtCtx.Fut24BitPcmPacket.AuxBit0 = 0;
      if(Proto.Pcm.PacketIdx < FUT_PCM_PACKETS_PER_FRAME)
      {
        DeltaChIdx = (2 * (Proto.Pcm.PacketIdx % FUT_PCM_PACKETS_PER_FRAME)); // DeltaChIdx = 0, 2, 4, 6 -> ChId = 1, 3, 5, 7
        PosChIdx   = DeltaChIdx + 1;                                          // PosChIdx   = 1, 3, 5, 7 -> ChId = 2, 4, 6, 8
      }
      else
      {
        PosChIdx   = (2 * (Proto.Pcm.PacketIdx % FUT_PCM_PACKETS_PER_FRAME)); // PosChIdx   = 0, 2, 4, 6 -> ChId = 1, 3, 5, 7
        DeltaChIdx = PosChIdx + 1;                                            // DeltaChIdx = 1, 3, 5, 7 -> ChId = 2, 4, 6, 8
      }
      Proto.Pcm.PtCtx.Fut24BitPcmPacket.Position  = FutUsToPcmValue(PPM_CENTER + (channelOutputs[PosChIdx] / 2), 0);
      Proto.Pcm.PtCtx.Fut24BitPcmPacket.Delta = FutDeltaUsToDeltaCode((channelOutputs[DeltaChIdx] - Proto.Pcm.MemoChOutputs[DeltaChIdx]) / 2); // Takes #40us
      if(Proto.Pcm.XanyChMap & (1 << DeltaChIdx))
      {
        Proto.Pcm.PtCtx.Fut24BitPcmPacket.Delta = 8; // X-Any cannot work with "smooth" variation (so delta (in us) between 2 Positions shall be almost null)
      }
      Proto.Pcm.MemoChOutputs[DeltaChIdx] = channelOutputs[DeltaChIdx]; // MemoChOutputs[] is used to compute Deltas
      //Compute ECC
      Proto.Pcm.PtCtx.Fut24BitPcmPacket.Ecc = 0;
      for(Proto.Pcm.PtCtx.BitIdx = 0; Proto.Pcm.PtCtx.BitIdx < 16; Proto.Pcm.PtCtx.BitIdx++)
      {
        if(Proto.Pcm.PtCtx.Fut24BitPcmPacket.Value & (1UL << (8 + Proto.Pcm.PtCtx.BitIdx)))
        {
          Proto.Pcm.PtCtx.Fut24BitPcmPacket.Ecc ^= (uint8_t)pgm_read_byte_far(&FutPcmCrcTbl[Proto.Pcm.PtCtx.BitIdx]);
        }
        PT_YIELD_AFTER_TICK(pt, GET_PROTO_TICK(), StartTick, SLICE_DURATION_TICK); // Here Tick is half Us
      } // Takes 164us -> Needs to be sliced by PT_YIELD_AFTER_TICK() macro
      // OK: PCM Frame is ready -> Build Radio PCM Packets by translating 6 bits to 10 bits
      Proto.Pcm.PtCtx.Fut40BitRadioPcmPacket[Proto.Pcm.PacketIdx % 2].TenBitBlock0 = FutSix2Ten(Proto.Pcm.PtCtx.Fut24BitPcmPacket.SixBitBlock0);
      Proto.Pcm.PtCtx.Fut40BitRadioPcmPacket[Proto.Pcm.PacketIdx % 2].TenBitBlock1 = FutSix2Ten(Proto.Pcm.PtCtx.Fut24BitPcmPacket.SixBitBlock1);
      Proto.Pcm.PtCtx.Fut40BitRadioPcmPacket[Proto.Pcm.PacketIdx % 2].TenBitBlock2 = FutSix2Ten(Proto.Pcm.PtCtx.Fut24BitPcmPacket.SixBitBlock2);
      Proto.Pcm.PtCtx.Fut40BitRadioPcmPacket[Proto.Pcm.PacketIdx % 2].TenBitBlock3 = FutSix2Ten(Proto.Pcm.PtCtx.Fut24BitPcmPacket.SixBitBlock3);
      Proto.Pcm.PacketIdx++; // Proto.Pcm.PacketIdx will automatically overlap to 0 as soon it will reach 8 (since it is coded on 3 bits)
      //208us per packet
      Proto.Pcm.PtCtx.PacketIdx++;
    }
    Proto.Pcm.PtCtx.PacketIdx = 0;
    while (Proto.Pcm.PtCtx.PacketIdx < (FUT_PCM_PACKETS_PER_FRAME / 2))
    {
      Proto.Pcm.BuildNblIdx--;
      Proto.Pcm.PtCtx.BitCnt = PcmStreamGetConsecBitNb(!Proto.Pcm.IsrBufIdx, Proto.Pcm.BuildNblIdx); // Retrieve last BitCnt in case subsequent bit(s) is(are) identical

      Proto.Pcm.PtCtx.BitIdx = (FUT_40_BIT_RADIO_PCM_PACKET_BIT_NB - 1);
      do
      {
        BitMask = (1LL << Proto.Pcm.PtCtx.BitIdx);
        if(Proto.Pcm.BitVal == !!(Proto.Pcm.PtCtx.Fut40BitRadioPcmPacket[Proto.Pcm.PtCtx.PacketIdx].Value & BitMask))
        {
          Proto.Pcm.PtCtx.BitCnt++;
        }
        else
        {
          PcmStreamSetConsecBitNb(!Proto.Pcm.IsrBufIdx, Proto.Pcm.BuildNblIdx++, Proto.Pcm.PtCtx.BitCnt);
          Proto.Pcm.BitVal = !Proto.Pcm.BitVal;
          Proto.Pcm.PtCtx.BitCnt = 1;
        }
        Proto.Pcm.PtCtx.BitIdx--;
        PT_YIELD_AFTER_TICK(pt, GET_PROTO_TICK(), StartTick, SLICE_DURATION_TICK); // Here Tick is Us
      }while(Proto.Pcm.PtCtx.BitIdx < 255);
      PcmStreamSetConsecBitNb(!Proto.Pcm.IsrBufIdx, Proto.Pcm.BuildNblIdx++, Proto.Pcm.PtCtx.BitCnt); // Last bit(s)
      Proto.Pcm.PtCtx.PacketIdx++;
    }
    if(!(Proto.Pcm.PacketIdx % FUT_PCM_PACKETS_PER_FRAME))
    {
      Proto.Pcm.BuildEndNblIdx[!Proto.Pcm.IsrBufIdx] = Proto.Pcm.BuildNblIdx - 1; // Used by ISR for frame lenght
    }
    #if defined(X_ANY)
    if(Proto.Pcm.BuildState == FUT_PCM1024_BUILD_2_LAST_PACKETS)
    {
      // Schedule here since this part is shorter than the first part (no Preamble, no Sync, no Odd/Even code)
      Xany_scheduleTx_AllInstance();
    }
    #endif
    Proto.Pcm.BuildState = !Proto.Pcm.BuildState;
  }
  PT_END(pt);
}

static uint16_t FutUsToPcmValue(int16_t PwmUs, uint8_t Delta)
{
  int16_t PcmBin;

  if(!Delta)
  {
    PwmUs -= FUT_PWM_US_MIN;
    if(PwmUs < 0)               PwmUs = 0;
    if(PwmUs > FUT_PWM_US_FULL_EXC) PwmUs = FUT_PWM_US_FULL_EXC;
  }
  PcmBin = ((PwmUs * 17) / 20); // 20/17 = 1.1764 -> Can use int16_t for computation

  return(PcmBin);
}

static uint8_t FutDeltaUsToDeltaCode(int16_t DeltaUs) // DeltaUs = NewWidthUs - PrevWidthUs
{
  int16_t SignedDeltaPcm, Min, Max;
  uint8_t Idx;

  SignedDeltaPcm = FutUsToPcmValue(DeltaUs, 1);
  for(Idx = 0; Idx < TBL_ITEM_NB(FutDeltaPcmTbl); Idx++)
  {
    Min = (int16_t)pgm_read_word(&FutDeltaPcmTbl[Idx].Min);
    Max = (int16_t)pgm_read_word(&FutDeltaPcmTbl[Idx].Max);
    if(SignedDeltaPcm >= Min && SignedDeltaPcm <= Max) break;
  }
  if(Idx >= TBL_ITEM_NB(FutDeltaPcmTbl)) Idx = 8; //Error -> Do NOT move

  return(Idx);
}


/* MULTIPLEX PRIVATE FUNCTIONS */

static PT_THREAD(MpxBuildRadioPcmBitStream(struct pt *pt))
{
  uint8_t ChOffset;
  uint8_t MpxChVal;
  uint8_t BitPairIdx;
  uint8_t PrevBitPair = 0xFF;
  uint8_t BitPair = 0;
  uint8_t SetIdx;
  uint8_t BitPairChks;
  uint8_t ChIdx = 0;

  pt = pt; // To avoid a compilation warning (since this is a fake protothread)
  if(!Proto.Pcm.BuildEndNblIdx[!Proto.Pcm.IsrBufIdx])//If buff not used by ISR is not already filled
  {
		Proto.Pcm.BuildNblIdx = 0;
		// Sync
		PcmStreamSetConsecBitNb(!Proto.Pcm.IsrBufIdx, Proto.Pcm.BuildNblIdx++, TL_SYNC_IDX);
		PcmStreamSetConsecBitNb(!Proto.Pcm.IsrBufIdx, Proto.Pcm.BuildNblIdx++, TH_SYNC_IDX);
    for(ChIdx = 0; ChIdx < 8; ChIdx++) // 8 channels per frame
    {
      // CH1 to CH6 + CH7 & CH8 or CH9 & CH10 (CH7 to CH10 are transmitted every 2 frames)
      BitPairChks = 0;
      ChOffset = 0; // CH7 and CH8
      if((ChIdx >= 6) && (Proto.Pcm.BuildState == MPX_PCM256_BUILD_FRAME_WITH_CH9_AND_CH10))
      {
        ChOffset = 2; // CH9 and CH10
      }
      MpxChVal = MpxUsToPcmValue(PPM_CENTER + (channelOutputs[ChIdx + ChOffset] / 2));
      PrevBitPair = B11; // For each channel, we start with MPX_SET_A, so previous bit pair SHALL be B11
      for(BitPairIdx = 0; BitPairIdx < 4; BitPairIdx++)
      {
        BitPair = (MpxChVal & B11000000) >> 6;
        BitPairChks ^= BitPair; // Update 2 bit checksum
        SetIdx = MpxGetSetIdx(PrevBitPair);
        PcmStreamSetConsecBitNb(!Proto.Pcm.IsrBufIdx, Proto.Pcm.BuildNblIdx++, TL_IDX);
        PcmStreamSetConsecBitNb(!Proto.Pcm.IsrBufIdx, Proto.Pcm.BuildNblIdx++, MpxSymbol(SetIdx, BitPair));
        MpxChVal <<= 2;
        PrevBitPair = BitPair;
      }
      BitPairChks ^= B00000011;
      SetIdx = MpxGetSetIdx(PrevBitPair);
      PcmStreamSetConsecBitNb(!Proto.Pcm.IsrBufIdx, Proto.Pcm.BuildNblIdx++, TL_IDX);
      PcmStreamSetConsecBitNb(!Proto.Pcm.IsrBufIdx, Proto.Pcm.BuildNblIdx++, MpxSymbol(SetIdx, BitPairChks));
      PrevBitPair = BitPairChks;
    }
    SetIdx = MpxGetSetIdx(PrevBitPair);
    PcmStreamSetConsecBitNb(!Proto.Pcm.IsrBufIdx, Proto.Pcm.BuildNblIdx++, TL_IDX);
    if(Proto.Pcm.BuildState == MPX_PCM256_BUILD_FRAME_WITH_CH7_AND_CH8)
    { // TYP1 -> CH7 and CH8
      PcmStreamSetConsecBitNb(!Proto.Pcm.IsrBufIdx, Proto.Pcm.BuildNblIdx++, MpxSymbol(SetIdx, B11));
      PrevBitPair = B11;
      SetIdx = MpxGetSetIdx(PrevBitPair);
      PcmStreamSetConsecBitNb(!Proto.Pcm.IsrBufIdx, Proto.Pcm.BuildNblIdx++, TL_IDX);
      PcmStreamSetConsecBitNb(!Proto.Pcm.IsrBufIdx, Proto.Pcm.BuildNblIdx++, MpxSymbol(SetIdx, B00));
    }
    else
    { //TYP2 -> CH9 and CH10
      PcmStreamSetConsecBitNb(!Proto.Pcm.IsrBufIdx, Proto.Pcm.BuildNblIdx++, MpxSymbol(SetIdx, B10));
      PrevBitPair = B10;
      SetIdx = MpxGetSetIdx(PrevBitPair);
      PcmStreamSetConsecBitNb(!Proto.Pcm.IsrBufIdx, Proto.Pcm.BuildNblIdx++, TL_IDX);
      PcmStreamSetConsecBitNb(!Proto.Pcm.IsrBufIdx, Proto.Pcm.BuildNblIdx++, MpxSymbol(SetIdx, B01));
    }
    // GAP
    PcmStreamSetConsecBitNb(!Proto.Pcm.IsrBufIdx, Proto.Pcm.BuildNblIdx++, TL_IDX);
    PcmStreamSetConsecBitNb(!Proto.Pcm.IsrBufIdx, Proto.Pcm.BuildNblIdx++, GAP_IDX);
    Proto.Pcm.BuildState = !Proto.Pcm.BuildState; // Alternate between CH7 & CH8 and CH9 & CH10
		Proto.Pcm.BuildEndNblIdx[!Proto.Pcm.IsrBufIdx] = Proto.Pcm.BuildNblIdx - 1; // Used by ISR for frame lenght
		#if defined(X_ANY)
    Xany_scheduleTx_AllInstance();
		#endif
  }
	return(0);
}

static uint8_t MpxGetSetIdx(uint8_t PrevBitPair)
{
  uint8_t SetIdx = MPX_SET_A;

  switch(PrevBitPair)
  {
    case B00:
    SetIdx = MPX_SET_D;
    break;

    case B01:
    SetIdx = MPX_SET_C;
    break;

    case B10:
    SetIdx = MPX_SET_B;
    break;

    case B11:
    SetIdx = MPX_SET_A;
    break;
  }

  return(SetIdx);
}

static uint8_t MpxUsToPcmValue(uint16_t PwmUs)
{
  uint8_t PcmVal;

  if(PwmUs < MPX_PWM_US_MIN) PwmUs = MPX_PWM_US_MIN;
  if(PwmUs > MPX_PWM_US_MAX) PwmUs = MPX_PWM_US_MAX;

  PcmVal = ((PwmUs - MPX_PWM_US_MIN) * 10) / 43;

  return(PcmVal); // PcmVal is in the range [0..255] (8 bits)
}


/* COMMON PRIVATE FUNCTIONS */

static inline void PcmStreamSetConsecBitNb(uint8_t BufIdx, uint8_t NblIdx, uint8_t ConsecBitNb)
{
  if(NblIdx <= PCM_NBL_MAX_IDX)
  {
    if(NblIdx & 1) Proto.Pcm.StreamConsecBitTbl[BufIdx][NblIdx / 2].LowNibbleBitNb  = ConsecBitNb;
    else           Proto.Pcm.StreamConsecBitTbl[BufIdx][NblIdx / 2].HighNibbleBitNb = ConsecBitNb;
  }
}

static inline uint8_t PcmStreamGetConsecBitNb(uint8_t BufIdx, uint8_t NblIdx)
{
  if(NblIdx & 1) return(Proto.Pcm.StreamConsecBitTbl[BufIdx][NblIdx / 2].LowNibbleBitNb);
  else           return(Proto.Pcm.StreamConsecBitTbl[BufIdx][NblIdx / 2].HighNibbleBitNb);
}


void PROTO_PCM_cb1()
{
  uint8_t  ConsecBitNb;
  uint16_t dt, half_us;

  ConsecBitNb = PcmStreamGetConsecBitNb(Proto.Pcm.IsrBufIdx, Proto.Pcm.TxNblIdx);
  half_us = (uint16_t)pgm_read_word_far(&Proto.Pcm.ConsecBitDurationHalfUs[ConsecBitNb]); // Use pre-computed values
  if(Proto.Pcm.TxNblIdx >= Proto.Pcm.BuildEndNblIdx[Proto.Pcm.IsrBufIdx])
  {
		Proto.Pcm.TxNblIdx = 255; // Will become 0 after incrementation
		Proto.Pcm.BuildEndNblIdx[Proto.Pcm.IsrBufIdx] = 0; // Free marker: this will trig the bit stream computation in the Protothread
		Proto.Pcm.IsrBufIdx = !Proto.Pcm.IsrBufIdx; // Buffer Flip
    // Schedule next Mixer calculations.
    SCHEDULE_MIXER_END_IN_US(FUT_PCM1024_FRAME_PERIOD_US); // Smallest period among all the PCM variants
    heartbeat |= HEART_TIMER_PULSES;
  }
  Proto.Pcm.TxNblIdx++;
  Proto.Pcm.BuildRadioPcmBitStream(&pcm_pt); // build a part of the next PCM bit stream at each interrupt
  dt = TCNT1 - OCR1B; // Calculate latency and jitter.
  OCR1B  += half_us;
  if(dt > US_TO_PROTO_TICK(80)) // if dt < 80us, this means that the bit stream creation is terminated
  {
    // This gives the elapsed min/max time to compute a part of the bit stream creation
    if(dt > g_tmr1Latency_max) g_tmr1Latency_max = dt;
    if(dt < g_tmr1Latency_min) g_tmr1Latency_min = dt;
  }
}


static void PROTO_PCM_reset()
{
  // Make pin idle state before disconnecting switching output.
  if(g_model.PULSEPOL) PORTB &= ~PIN6_bm;
  else PORTB |= PIN6_bm;
  TCCR1A &= ~(0b11<<COM1B0);
  TIMSK1 &= ~(1<<OCIE1B); // Disable Output Compare B interrupt.
  TIFR1 |= 1<<OCF1B; // Reset Flag.
}

static void PROTO_PCM_initialize()
{
	switch(PcmProto)
	{
		case PCM_PROTO_FUT:
		Proto.Pcm.BuildRadioPcmBitStream  = FutBuildRadioPcmBitStream;
		Proto.Pcm.ConsecBitDurationHalfUs = FutConsecBitDurationHalfUs;
		Proto.Pcm.BuildState     = FUT_PCM1024_BUILD_2_FIRST_PACKETS;
		Proto.Pcm.PacketIdx      = 0;
		Proto.Pcm.BitVal         = 0;
		Proto.Pcm.BuildNblIdx    = 0;
		Proto.Pcm.IsrBufIdx      = 0;
		Proto.Pcm.BuildEndNblIdx[ Proto.Pcm.IsrBufIdx] = PCM_NBL_MAX_IDX;
		Proto.Pcm.BuildEndNblIdx[!Proto.Pcm.IsrBufIdx] = 0;
		Proto.Pcm.TxNblIdx       = 0;
		memset(&Proto.Pcm.StreamConsecBitTbl, 0x22, sizeof(Proto.Pcm.StreamConsecBitTbl));
		break;

		case PCM_PROTO_GRA:
		Proto.Pcm.BuildRadioPcmBitStream  = FutBuildRadioPcmBitStream;  // TO DO: when ready
		Proto.Pcm.ConsecBitDurationHalfUs = FutConsecBitDurationHalfUs; // TO DO: when ready
		Proto.Pcm.BuildState     = 0;// TO DO: when ready
		Proto.Pcm.PacketIdx      = 0;
		Proto.Pcm.BitVal         = 0;
		Proto.Pcm.BuildNblIdx    = 0;
		Proto.Pcm.IsrBufIdx      = 0;
		Proto.Pcm.BuildEndNblIdx[ Proto.Pcm.IsrBufIdx] = PCM_NBL_MAX_IDX;
		Proto.Pcm.BuildEndNblIdx[!Proto.Pcm.IsrBufIdx] = 0;
		Proto.Pcm.TxNblIdx       = 0;
		memset(&Proto.Pcm.StreamConsecBitTbl, 0x22, sizeof(Proto.Pcm.StreamConsecBitTbl));
		break;

		case PCM_PROTO_MPX:
		Proto.Pcm.BuildRadioPcmBitStream  = MpxBuildRadioPcmBitStream;
		Proto.Pcm.ConsecBitDurationHalfUs = MpxConsecBitDurationHalfUs;
		Proto.Pcm.BuildState     = MPX_PCM256_BUILD_FRAME_WITH_CH7_AND_CH8;
		Proto.Pcm.BuildNblIdx    = 0;
		Proto.Pcm.IsrBufIdx      = 0;
		Proto.Pcm.BuildEndNblIdx[ Proto.Pcm.IsrBufIdx] = PCM_NBL_MAX_IDX;
		Proto.Pcm.BuildEndNblIdx[!Proto.Pcm.IsrBufIdx] = 0;
		Proto.Pcm.TxNblIdx       = 0;
		memset(&Proto.Pcm.StreamConsecBitTbl, 0x22, sizeof(Proto.Pcm.StreamConsecBitTbl));
		break;
	}
  PT_INIT(&pcm_pt);
#if defined(X_ANY)
	Pcm_updateXanyChMap();
#endif
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    ocr1b_function_ptr = PROTO_PCM_cb1; // Setup function pointer used in ISR.
    TCCR1A = (TCCR1A | (1<<COM1B1)) & ~(1<<COM1B0); // Clear
    TCCR1C = 1<<FOC1B; // Strobe FOC1.
    TCCR1A = (TCCR1A | (1<<COM1B0)) & ~(1<<COM1B1); // Toggle OC1x on next match.

    OCR1B = TCNT1 + US_TO_PROTO_TICK(300); // Next interrupt in 300us for all PCM variants
    TIFR1  |= 1<<OCF1B;  // Reset Flag.
    TIMSK1 |= 1<<OCIE1B; // Enable Output Compare interrupt.
  }
}

const void * PROTO_PCM_Cmds(enum ProtoCmds cmd)
{
  switch(cmd) {
    case PROTOCMD_INIT: PROTO_PCM_initialize();
    return 0;
    case PROTOCMD_RESET:
      PROTO_PCM_reset();
    return 0;
  case PROTOCMD_GETOPTIONS:
   SetRfOptionSettings(pgm_get_far_address(RfOpt_PCM_Ser),
                       STR_PCMPROTO,   //Failsafe modes
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
