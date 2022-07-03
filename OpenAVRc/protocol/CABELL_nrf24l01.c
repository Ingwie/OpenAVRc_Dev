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


const static RfOptionSettingsvar_t RfOpt_CABELL_Ser[] PROGMEM =
{
 /*rfProtoNeed*/PROTO_NEED_SPI | BOOL1USED | BOOL2USED, //can be PROTO_NEED_SPI | BOOL1USED | BOOL2USED | BOOL3USED
 /*rfSubTypeMax*/2,
 /*rfOptionValue1Min*/4, // N Ch min (12)
 /*rfOptionValue1Max*/16, // N Ch max (0)
 /*rfOptionValue2Min*/0,
 /*rfOptionValue2Max*/0,
 /*rfOptionValue3Max*/3, // rfpower
};

const pm_char STR_SUBTYPE_CABELL[] PROGMEM = " PPM""SPPM""SBUS";

// The Receiver for this protocol is available at: https://github.com/soligen2010/RC_RX_CABELL_V3_FHSS


#define CABELL_BIND_COUNT		2000		// At least 2000 so that if TX toggles the serial bind flag then bind mode is never exited
#define CABELL_PACKET_PERIOD	3000	// Do not set too low or else next packet may not be finished transmitting before the channel is changed next time around

#define CABELL_NUM_CHANNELS		16		// The maximum number of RC channels that can be sent in one packet
#define CABELL_MIN_CHANNELS		4			// The minimum number of channels that must be included in a packet, the number of channels cannot be reduced any further than this
#define CABELL_PAYLOAD_BYTES	24		// 12 bits per value * 16 channels
#define CABELL_PACKET_BYTES	  30		// 30 bits max

#define CABELL_RADIO_CHANNELS			9	    // This is 1/5 of the total number of radio channels used for FHSS
#define CABELL_RADIO_MIN_CHANNEL_NUM	3	// Channel 0 is right on the boarder of allowed frequency range, so move up to avoid bleeding over
#define CABELL_TELEMETRY_PACKET_LENGTH	4

#define CABELL_BIND_RADIO_ADDR	0xA4B7C123F7LL

#define CABELL_OPTION_MASK_CHANNEL_REDUCTION		0x0F
#define CABELL_RESERVED_MASK_CHANNEL            0x3F

#define cabell_sub_protocol g_model.rfSubType
#define cabell_ubind        g_model.rfOptionBool2
#define cabell_telemetry    g_model.rfOptionBool1
#define cabell_reduction    (16 - g_model.rfOptionValue1)

enum
{
 C_normal                 = 0,
 C_bind                   = 1,
 C_setFailSafe            = 2,
 C_normalWithTelemetry    = 3,
 C_telemetryResponse      = 4,
 C_bindFalesafeNoPulse    = 5,
 C_unBind                 = 127
} /*RxMode_t*/;

/*
typedef struct
{
 uint8_t RxMode; -> packet_p2M[0]
 uint8_t reserved; -> packet_p2M[1] * Contains the channel number that the packet was sent on in bits 0-5
 uint8_t option; -> packet_p2M[2]
 *   mask 0x0F    : Channel reduction.  The number of channels to not send (subtracted from the 16 max channels) at least 4 are always sent
  *   mask 0x30>>4 : Receiver output mode
  *                  0 (00) = Single PPM on individual pins for each channel
  *                  1 (01) = SUM PPM on channel 1 pin
  *                  2 (10) = SBUS output
  *                  3 (11) = Unused
  *   mask 0x40>>6   Contains max power override flag for Multi-protocol TX module. Also sent to RX
  *   mask 0x80>>7   Unused
  *
 uint8_t modelNum; -> packet_p2M[3]
 uint8_t checkSum_LSB; -> packet_p2M[4]
 uint8_t checkSum_MSB; -> packet_p2M[5]
 uint8_t payloadValue[CABELL_PAYLOAD_BYTES] = {0}; -> packet_p2M[6] - packet_p2M[30]//12 bits per channel value, unsigned
} CABELL_RxTxPacket_t;*/

static void CABELL_getChannelSequence (uint64_t permutation)
{
 /* This procedure initializes an array with the sequence progression of channels.
 * This is not the actual channels itself, but the sequence base to be used within bands of
 * channels.
 *
 * There are CABELL_RADIO_CHANNELS! permutations for arranging the channels
 * one of these permutations will be calculated based on the permutation input
 * permutation should be between 1 and CABELL_RADIO_CHANNELS! but the routine will constrain it
 * if these bounds are exceeded.  Typically the radio's unique TX ID should be used.
 *
 * The maximum CABELL_RADIO_CHANNELS is 20.  Anything larger than this will cause the uint64_t
 * variables to overflow, yielding unknown results (possibly infinite loop?).  Therefor
 * this routine constrains the value.
 */
 uint8_t i;   //iterator counts numChannels
 uint64_t indexOfNextSequenceValue;
 uint64_t numChannelsFactorial = 1;
 uint8_t  sequenceValue;

 for (i = 1; i <= CABELL_RADIO_CHANNELS; i++)
  {
   numChannelsFactorial *= i;      //  Calculate n!
   channel_used_p2M[i-1] = i-1;        //  Initialize array with the sequence
  }

 permutation = (permutation % numChannelsFactorial) + 1;    // permutation must be between 1 and n! or this algorithm will infinite loop

//Rearrange the array elements based on the permutation selected
 for (i = 0, permutation--; i < CABELL_RADIO_CHANNELS; i++ )
  {
   numChannelsFactorial /= ((uint64_t)CABELL_RADIO_CHANNELS)-i;
   indexOfNextSequenceValue = i + (permutation/numChannelsFactorial);
   permutation %= numChannelsFactorial;

   //Copy the value in the selected array position
   sequenceValue = channel_used_p2M[indexOfNextSequenceValue];

   //Shift the unused elements in the array to make room to move in the one just selected
   for( ; indexOfNextSequenceValue > i; indexOfNextSequenceValue--)
    channel_used_p2M[indexOfNextSequenceValue] = channel_used_p2M[indexOfNextSequenceValue-1];

   // Copy the selected value into it's new array slot
   channel_used_p2M[i] = sequenceValue;
  }
}

static uint8_t CABELL_getNextChannel(uint8_t prevChannel)
{
 /* Possible channels are in 5 bands, each band comprised of CABELL_RADIO_CHANNELS channels
 * channel_used_p2M contains CABELL_RADIO_CHANNELS elements in the relative order in which we should progress through the band
 *
 * Each time the channel is changes, bands change in a way so that the next channel will be in a
 * different non-adjacent band. Both the band changes and the index in channel_used_p2M is incremented.
 */
 prevChannel -= CABELL_RADIO_MIN_CHANNEL_NUM;				// Subtract CABELL_RADIO_MIN_CHANNEL_NUM because it was added to the return value
 if (prevChannel > (CABELL_RADIO_CHANNELS * 5))
  prevChannel = CABELL_RADIO_CHANNELS * 5;							// Constrain the values just in case something bogus was sent in.

 uint8_t currBand = prevChannel / CABELL_RADIO_CHANNELS;
 uint8_t nextBand = (currBand + 3) % 5;

 uint8_t prevChannalSeqArrayValue = prevChannel % CABELL_RADIO_CHANNELS;
 uint8_t prevChannalSeqArrayPosition = 0;

 for (uint8_t x = 0; x < CABELL_RADIO_CHANNELS; x++)
  {
   // Find the position of the previous channel in the array
   if (channel_used_p2M[x] == prevChannalSeqArrayValue)
    prevChannalSeqArrayPosition = x;
  }
 uint8_t nextChannalSeqArrayPosition = prevChannalSeqArrayPosition + 1;
 if (nextChannalSeqArrayPosition >= CABELL_RADIO_CHANNELS)
  nextChannalSeqArrayPosition = 0;

 return (CABELL_RADIO_CHANNELS * nextBand) + channel_used_p2M[nextChannalSeqArrayPosition] + CABELL_RADIO_MIN_CHANNEL_NUM;	// Add CABELL_RADIO_MIN_CHANNEL_NUM so we dont use channel 0 as it may bleed below 2.400 GHz
}

static void CABELL_setAddress()
{
 uint64_t CABELL_addr;

 if (!bind_idx_p2M)
  {
   CABELL_addr =
    (((uint64_t)g_eeGeneral.fixed_ID.ID_8[0] << 32) |
     ((uint64_t)g_eeGeneral.fixed_ID.ID_8[1] << 24) |
     ((uint64_t)g_eeGeneral.fixed_ID.ID_8[2] << 16) |
     ((uint64_t)g_eeGeneral.fixed_ID.ID_8[3] << 8) |
     (uint64_t)g_eeGeneral.fixed_ID.ID_8[0]);					// Address to use after binding
  }
 else
  CABELL_addr = CABELL_BIND_RADIO_ADDR;				// Static addr for binding

 CABELL_getChannelSequence(CABELL_addr);		// Get the sequence for hopping through channels
 channel_index_p2M = CABELL_RADIO_MIN_CHANNEL_NUM;				// Initialize the channel sequence

 uint64_t CABELL_Telemetry_addr = ~CABELL_addr;			// Invert bits for reading so that telemetry packets have a different address.

 NRF24L01_WriteRegisterMulti(NRF24L01_0A_RX_ADDR_P0, (uint8_t*)(&CABELL_Telemetry_addr), 5);
 NRF24L01_WriteRegisterMulti(NRF24L01_0B_RX_ADDR_P1, (uint8_t*)(&CABELL_Telemetry_addr), 5);
 NRF24L01_WriteRegisterMulti(NRF24L01_10_TX_ADDR,    (uint8_t*)(&CABELL_addr), 5);
}

static void CABELL_init()
{
 NRF24L01_Initialize();
 NRF24L01_ManagePower();
 NRF24L01_SetBitrate(NRF24L01_BR_250K);			    	// slower data rate gives better range/reliability
 NRF24L01_WriteReg(NRF24L01_01_EN_AA, 0x00);  	  // No Auto Acknowledgment on all data pipes
 NRF24L01_SetTxRxMode(TX_EN);					          	//Power up and 16 bit CRC

 CABELL_setAddress();

 NRF24L01_WriteReg(NRF24L01_07_STATUS, 0x70);     // Reset status
 NRF24L01_FlushTx();
 NRF24L01_FlushRx();
 NRF24L01_WriteReg(NRF24L01_02_EN_RXADDR, 0x01);  // Enable only data Pipe 0
 NRF24L01_WriteReg(NRF24L01_11_RX_PW_P0, 0x20);		// 32 byte packet length
 NRF24L01_WriteReg(NRF24L01_12_RX_PW_P1, 0x20);		// 32 byte packet length
 NRF24L01_WriteReg(NRF24L01_03_SETUP_AW, 0x03);   // 5 bytes adress
 NRF24L01_WriteReg(NRF24L01_04_SETUP_RETR, 0x5F);	// no retransmits
 NRF24L01_Activate(0x73);					            		// Activate feature register
 NRF24L01_WriteReg(NRF24L01_1C_DYNPD, 0x3F);			// Enable dynamic payload length on all pipes
 NRF24L01_WriteReg(NRF24L01_1D_FEATURE, 0x04);		// Enable dynamic Payload Length
 NRF24L01_Activate(0x73);
}

FORCEINLINE void CABELL_get_telemetry()
{
// calculate TX rssi based on past 250 expected telemetry packets.  Cannot use full second count because telemetry_counter is not large enough
 if (++rfState16_p2M > 250)
  {
#if defined(FRSKY)
   telemetryData.rssi[1].set(receive_seq_p2M);
#endif
   receive_seq_p2M = 0;
   rfState16_p2M = 0;
  }

// Process incoming telemetry packet of it was received
 if (NRF24L01_NOP() & _BV(NRF24L01_07_RX_DR))
  {
   // data received from model
   NRF24L01_ReadPayload(telem_save_data_p2M, CABELL_TELEMETRY_PACKET_LENGTH);

   if ((telem_save_data_p2M[0] & 0x7F) == C_telemetryResponse)	// ignore high order bit in compare because it toggles with each packet
    {
#if defined(FRSKY)
     frskyStreaming = frskyStreaming ? FRSKY_TIMEOUT10ms : FRSKY_TIMEOUT_FIRST;
     telemetryData.rssi[0].set(telem_save_data_p2M[1]);	// Packet rate 0 to 255 where 255 is 100% packet rate
     telemetryData.analog[TELEM_ANA_A1].set(telem_save_data_p2M[2], g_model.telemetry.channels[TELEM_ANA_A1].type);	// Directly from analog input of receiver, but reduced to 8-bit depth (0 to 255).  Scaling depends on the input to the analog pin of the receiver.
     telemetryData.analog[TELEM_ANA_A2].set(telem_save_data_p2M[3], g_model.telemetry.channels[TELEM_ANA_A2].type);	// Directly from analog input of receiver, but reduced to 8-bit depth (0 to 255).  Scaling depends on the input to the analog pin of the receiver.
#endif
     receive_seq_p2M++;
    }
  }
 else
  {
   // If no telemetry packet was received then delay by the typical telemetry packet processing time
   // This is done to try to keep the sendPacket process timing more consistent. Since the SPI payload read takes some time
   _delay_us(50);
  }
 NRF24L01_SetTxRxMode(TX_EN);
 NRF24L01_FlushRx();
}

static void CABELL_send_packet()
{
 if ((!bind_idx_p2M) && cabell_telemetry) // check for incoming packet and switch radio back to TX mode if we were listening for telemetry
  {
   CABELL_get_telemetry();
  }

 uint8_t channelReduction = cabell_reduction;

 if (bind_idx_p2M)
  channelReduction = 0;	// Send full packet to bind as higher channels will contain bind info

 packet_p2M[2] = channelReduction | ((0x03 & cabell_sub_protocol) << 4);

 packetSize_p2M = CABELL_PACKET_BYTES - ((((channelReduction - (channelReduction % 2))/ 2)) * 3); // reduce 3 bytes per 2 channels, but not last channel if it is odd
 uint8_t maxPayloadValueIndex = CABELL_PAYLOAD_BYTES - (CABELL_PACKET_BYTES - packetSize_p2M);

 if (cabell_ubind && (!bind_idx_p2M))
  {
   packet_p2M[0] = C_unBind;
  }
 else
  {
   /*if (sub_protocol == CABELL_SET_FAIL_SAFE && !bind_idx_p2M)
   	packet_p2M[0] = C_setFailSafe;
   else*/
   {
    if (bind_idx_p2M)
     packet_p2M[0] = C_bind;
    else
     {
      if (cabell_telemetry)
       packet_p2M[0] = C_normalWithTelemetry;
      else
       packet_p2M[0] = C_normal;
     }
   }
  }

// Set channel for next transmission
 channel_index_p2M = CABELL_getNextChannel(channel_index_p2M);
 packet_p2M[1] = channel_index_p2M & CABELL_RESERVED_MASK_CHANNEL;

 packet_p2M[3] = RXNUM;

 uint8_t payloadIndex = 0;
 int16_t holdValue;

 memclear(&packet_p2M[6], maxPayloadValueIndex);  // Reset values

 for (uint8_t x = 0; x < (CABELL_NUM_CHANNELS - channelReduction); x++)
  {
   // valid channel values are 1000 to 2000
   holdValue = (FULL_CHANNEL_OUTPUTS(x))/2; // +-1024 to +-512
   holdValue += PPM_CENTER; // + 1500 offset
   holdValue = limit<int16_t>(1000, holdValue, 2000);

   if (bind_idx_p2M)
    {
     switch (x)
      {
      //tx address sent for bind
      case 11			:
       holdValue = 1000 + g_eeGeneral.fixed_ID.ID_8[0];
       break;
      case 12			:
       holdValue = 1000 + g_eeGeneral.fixed_ID.ID_8[1];
       break;
      case 13			:
       holdValue = 1000 + g_eeGeneral.fixed_ID.ID_8[2];
       break;
      case 14			:
       holdValue = 1000 + g_eeGeneral.fixed_ID.ID_8[3];
       break;
      case 15			:
       holdValue = 1000 + g_eeGeneral.fixed_ID.ID_8[0];
       break;
      }
    }

   // use 12 bits per value
   if (x % 2)
    {
     //output channel number is ODD
     holdValue <<= 4;
     payloadIndex--;
    }
   else
    holdValue &= 0x0FFF;

   packet_p2M[6 + payloadIndex] |= holdValue & 0xFF;
   payloadIndex++;
   packet_p2M[6 + payloadIndex] |= holdValue >> 8;
   payloadIndex++;
  }

 uint16_t checkSum = packet_p2M[0] + packet_p2M[1] + packet_p2M[2] + packet_p2M[3]; // Start Calculate checksum

 for(uint8_t x = 0; x < maxPayloadValueIndex ; x++)
  checkSum += packet_p2M[6 + x];  // Finish Calculate checksum

 packet_p2M[4] = checkSum & 0xFF;
 packet_p2M[5] = checkSum >> 8;

 packet_p2M[0] ^= 0x80;   // This causes the 8th bit of the first byte to toggle with each xmit so consecutive payloads are not identical.
// This is a work around for a reported bug in clone NRF24L01 chips that mis-took this case for a re-transmit of the same packet.

 NRF24L01_WriteReg(NRF24L01_05_RF_CH, channel_index_p2M); // send channel
 NRF24L01_ManagePower();
 NRF24L01_WritePayload(packet_p2M, packetSize_p2M);      // and payload
}

static uint16_t CABELL_manage_time()
{
 uint16_t packet_period;

 if (!bind_idx_p2M && cabell_telemetry)
  {
     // switch radio to rx as soon as packet is sent
     // calculate transmit time based on packet size and data rate of 250 Kbs per sec
   uint16_t rxDelay = /* Variable time air */(4 * 8 * packetSize_p2M) + /* Fixed */450;

   if (!telem_save_seq_p2M)
    {
     bind_counter_p2M = PROTOCOL_GetElapsedTime(); // use bind_counter_p2M as memory only here
     packet_period = rxDelay + bind_counter_p2M;
     telem_save_seq_p2M = 1; // indicate to switch to RX mode next time
    }
   else
    {
     // increase packet period by 100 us for each channel over 6
     packet_period = limit<uint16_t>(0, (uint8_t)(g_model.rfOptionValue1 - 6), 10);
     packet_period *= 100;
     packet_period += CABELL_PACKET_PERIOD;
     packet_period -= rxDelay + bind_counter_p2M; // remove RX time
     telem_save_seq_p2M = 0; // reset switch to RX
    }
  }
 else
  packet_period = CABELL_PACKET_PERIOD;   // Standard packet period when not in telemetry mode.

 return packet_period;
}

static uint16_t CABELL_cb()
{
 if (cabell_telemetry && telem_save_seq_p2M) // we need to switch to RX mode to read telemetry
  {
   NRF24L01_WriteReg(NRF24L01_00_CONFIG, 0x0F);  // RX mode with 16 bit CRC
  }
 else
  {
   if (++rfState8_p2M >= 4)
    {
     rfState8_p2M = 0;
     SCHEDULE_MIXER_END_IN_US(12000); // Schedule next Mixer calculations.
    }
   CABELL_send_packet();
  }
 uint16_t protocol_period = CABELL_manage_time();
 heartbeat |= HEART_TIMER_PULSES;
 CALCULATE_LAT_JIT(); // Calculate latency and jitter.
 return protocol_period * 2; // From 3mS to 4mS
}

static void CABELL_initialize(uint8_t bind)
{
 bind_idx_p2M = bind; // store bind state
 CABELL_init();
 PROTO_Start_Callback(CABELL_cb);
}

const void *CABELL_Cmds(enum ProtoCmds cmd)
{
 switch(cmd)
  {
  case PROTOCMD_INIT:
   CABELL_initialize(0);
   return 0;
  case PROTOCMD_RESET:
   PROTO_Stop_Callback();
   NRF24L01_Reset();
   return 0;
  case PROTOCMD_BIND:
   CABELL_initialize(1);
   return 0;
  case PROTOCMD_GETOPTIONS:
   SetRfOptionSettings(pgm_get_far_address(RfOpt_CABELL_Ser),
                       STR_SUBTYPE_CABELL,      //Sub proto
                       STR_NUMCH,      //Option 1 (int) Num channels (4-16Ch : -> 12 to 0)
                       STR_DUMMY,      //Option 2 (int)
                       STR_RFPOWER,    //Option 3 (uint 0 to 31)
                       STR_TELEMETRY,  //OptionBool 1
                       STR_UBIND,      // UnBind
                       STR_DUMMY       //OptionBool 3
                      );
   return 0;
  default:
   break;
  }
 return 0;
}
/*
A0 00 2E 00 10 86 0D E8 83 3E E8 83 3E E8 83 3E E8 83 3E E8 83 3E E8 83 3E E8 83 3E E8 83 3E
A0 80 25 00 10 7D 0D E8 83 3E E8 83 3E E8 83 3E E8 83 3E E8 83 3E E8 83 3E E8 83 3E E8 83 3E
A0 00 24 00 10 7C 0D E8 83 3E E8 83 3E E8 83 3E E8 83 3E E8 83 3E E8 83 3E E8 83 3E E8 83 3E
A0 00 26 00 10 7E 0D E8 83 3E E8 83 3E E8 83 3E E8 83 3E E8 83 3E E8 83 3E E8 83 3E E8 83 3E
*/
