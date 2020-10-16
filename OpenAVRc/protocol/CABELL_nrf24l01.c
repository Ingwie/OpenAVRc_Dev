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
#define CABELL_PACKET_PERIOD	3000		// Do not set too low or else next packet_p2M may not be finished transmitting before the channel is changed next time around

#define CABELL_NUM_CHANNELS		16			// The maximum number of RC channels that can be sent in one packet_p2M
#define CABELL_MIN_CHANNELS		4			// The minimum number of channels that must be included in a packet_p2M, the number of channels cannot be reduced any further than this
#define CABELL_PAYLOAD_BYTES	24			// 12 bits per value * 16 channels

#define CABELL_RADIO_CHANNELS			9	// This is 1/5 of the total number of radio channels used for FHSS
#define CABELL_RADIO_MIN_CHANNEL_NUM	3	// Channel 0 is right on the boarder of allowed frequency range, so move up to avoid bleeding over
#define CABELL_TELEMETRY_PACKET_LENGTH	4

#define CABELL_BIND_RADIO_ADDR	0xA4B7C123F7LL

#define CABELL_OPTION_MASK_CHANNEL_REDUCTION		0x0F
#define CABELL_RESERVED_MASK_CHANNEL            0x3F

#define cabell_sub_protocol g_model.rfSubType
#define cabell_ubind        g_model.rfOptionBool2
#define cabell_telemetry    g_model.rfOptionBool1
#define cabell_num_ch       (16 - g_model.rfOptionValue1)
#define cabell_num_ch_raw   g_model.rfOptionValue1

enum
{
 normal                 = 0,
 CabellBbind            = 1, // not used here
 setFailSafe            = 2,
 normalWithTelemetry    = 3,
 telemetryResponse      = 4,
 bindFalesafeNoPulse    = 5,
 unBind                 = 127
} /*RxMode_t*/;

typedef struct
{
 uint8_t RxMode;
 uint8_t reserved;  /* Contains the channel number that the packet_p2M was sent on in bits 0-5
                          */
 uint8_t option;
 /*   mask 0x0F    : Channel reduction.  The number of channels to not send (subtracted from the 16 max channels) at least 4 are always sent
  *   mask 0x30>>4 : Receiver output mode
  *                  0 (00) = Single PPM on individual pins for each channel
  *                  1 (01) = SUM PPM on channel 1 pin
  *                  2 (10) = SBUS output
  *                  3 (11) = Unused
  *   mask 0x40>>6   Contains max power override flag for Multi-protocol TX module. Also sent to RX
  *   mask 0x80>>7   Unused
  */
 uint8_t modelNum;
 uint8_t checkSum_LSB;
 uint8_t checkSum_MSB;
 uint8_t payloadValue [CABELL_PAYLOAD_BYTES]; //12 bits per channel value, unsigned
} CABELL_RxTxPacket_t;

static void CABELL_getChannelSequence (uint8_t outArray[], uint8_t numChannels, uint64_t permutation)
{
 /* This procedure initializes an array with the sequence progression of channels.
 * This is not the actual channels itself, but the sequence base to be used within bands of
 * channels.
 *
 * There are numChannels! permutations for arranging the channels
 * one of these permutations will be calculated based on the permutation input
 * permutation should be between 1 and numChannels! but the routine will constrain it
 * if these bounds are exceeded.  Typically the radio's unique TX ID should be used.
 *
 * The maximum numChannels is 20.  Anything larger than this will cause the uint64_t
 * variables to overflow, yielding unknown results (possibly infinite loop?).  Therefor
 * this routine constrains the value.
 */
 uint8_t i;   //iterator counts numChannels
 uint64_t indexOfNextSequenceValue;
 uint64_t numChannelsFactorial=1;
 uint8_t  sequenceValue;

 numChannels = limit<uint8_t>(1, numChannels, 20);

 for (i = 1; i <= numChannels; i++)
  {
   numChannelsFactorial *= i;      //  Calculate n!
   outArray[i-1] = i-1;            //  Initialize array with the sequence
  }

 permutation = (permutation % numChannelsFactorial) + 1;    // permutation must be between 1 and n! or this algorithm will infinite loop

//Rearrange the array elements based on the permutation selected
 for (i=0, permutation--; i<numChannels; i++ )
  {
   numChannelsFactorial /= ((uint64_t)numChannels)-i;
   indexOfNextSequenceValue = i+(permutation/numChannelsFactorial);
   permutation %= numChannelsFactorial;

   //Copy the value in the selected array position
   sequenceValue = outArray[indexOfNextSequenceValue];

   //Shift the unused elements in the array to make room to move in the one just selected
   for( ; indexOfNextSequenceValue > i; indexOfNextSequenceValue--)
    outArray[indexOfNextSequenceValue] = outArray[indexOfNextSequenceValue-1];

   // Copy the selected value into it's new array slot
   outArray[i] = sequenceValue;
  }
}

static void CABELL_setAddress(uint8_t bind)
{
 uint64_t CABELL_addr;

 if (!bind)
  {
   CABELL_addr =
    (((uint64_t)temp_rfid_addr_p2M[0]) << 32) +
    (((uint64_t)temp_rfid_addr_p2M[1]) << 24) +
    (((uint64_t)temp_rfid_addr_p2M[2]) << 16) +
    (((uint64_t)temp_rfid_addr_p2M[3]) << 8) +
    (((uint64_t)temp_rfid_addr_p2M[0]));					// Address to use after binding
  }
 else
  CABELL_addr = CABELL_BIND_RADIO_ADDR;				// Static addr for binding

 CABELL_getChannelSequence(channel_used_p2M,CABELL_RADIO_CHANNELS,CABELL_addr);		// Get the sequence for hopping through channels
 channel_index_p2M = CABELL_RADIO_MIN_CHANNEL_NUM;				// Initialize the channel sequence

 packet_count_p2M = 0;

 uint64_t CABELL_Telemetry_addr = ~CABELL_addr;			// Invert bits for reading so that telemetry packets have a different address.

 NRF24L01_WriteRegisterMulti(NRF24L01_0A_RX_ADDR_P0, reinterpret_cast<uint8_t*>(&CABELL_Telemetry_addr), 5);
 NRF24L01_WriteRegisterMulti(NRF24L01_0B_RX_ADDR_P1, reinterpret_cast<uint8_t*>(&CABELL_Telemetry_addr), 5);
 NRF24L01_WriteRegisterMulti(NRF24L01_10_TX_ADDR,    reinterpret_cast<uint8_t*>(&CABELL_addr), 5);
}

static void CABELL_init(uint8_t bind)
{
 NRF24L01_ManagePower();
 NRF24L01_SetBitrate(NRF24L01_BR_250K);				// slower data rate gives better range/reliability
 NRF24L01_WriteReg(NRF24L01_01_EN_AA, 0x00);			// No Auto Acknowledgment on all data pipes
 NRF24L01_SetTxRxMode(TX_EN);						//Power up and 16 bit CRC

 CABELL_setAddress(bind);

 NRF24L01_WriteReg(NRF24L01_07_STATUS, 0x70);
 NRF24L01_FlushTx();
 NRF24L01_FlushRx();
 NRF24L01_WriteReg(NRF24L01_02_EN_RXADDR, 0x01);
 NRF24L01_WriteReg(NRF24L01_11_RX_PW_P0, 0x20);		// 32 byte packet_p2M length
 NRF24L01_WriteReg(NRF24L01_12_RX_PW_P1, 0x20);		// 32 byte packet_p2M length
 NRF24L01_WriteReg(NRF24L01_03_SETUP_AW, 0x03);
 NRF24L01_WriteReg(NRF24L01_04_SETUP_RETR, 0x5F);	// no retransmits
 NRF24L01_Activate(0x73);							// Activate feature register
 NRF24L01_WriteReg(NRF24L01_1C_DYNPD, 0x3F);			// Enable dynamic payload length on all pipes
 NRF24L01_WriteReg(NRF24L01_1D_FEATURE, 0x04);		// Enable dynamic Payload Length
 NRF24L01_Activate(0x73);
}


static uint16_t CABELL_getNextChannel (uint8_t seqArray[], uint8_t seqArraySize, uint8_t prevChannel)
{
 /* Possible channels are in 5 bands, each band comprised of seqArraySize channels
 * seqArray contains seqArraySize elements in the relative order in which we should progress through the band
 *
 * Each time the channel is changes, bands change in a way so that the next channel will be in a
 * different non-adjacent band. Both the band changes and the index in seqArray is incremented.
 */
 prevChannel -= CABELL_RADIO_MIN_CHANNEL_NUM;				// Subtract CABELL_RADIO_MIN_CHANNEL_NUM because it was added to the return value
 if(prevChannel>(seqArraySize * 5))
  prevChannel=seqArraySize * 5;							// Constrain the values just in case something bogus was sent in.

 uint8_t currBand = prevChannel / seqArraySize;
 uint8_t nextBand = (currBand + 3) % 5;

 uint8_t prevChannalSeqArrayValue = prevChannel % seqArraySize;
 uint8_t prevChannalSeqArrayPosition = 0;
 for (int x = 0; x < seqArraySize; x++)
  {
   // Find the position of the previous channel in the array
   if (seqArray[x] == prevChannalSeqArrayValue)
    prevChannalSeqArrayPosition = x;
  }
 uint8_t nextChannalSeqArrayPosition = prevChannalSeqArrayPosition + 1;
 if (nextChannalSeqArrayPosition >= seqArraySize)
  nextChannalSeqArrayPosition = 0;

 return (seqArraySize * nextBand) + seqArray[nextChannalSeqArrayPosition] + CABELL_RADIO_MIN_CHANNEL_NUM;	// Add CABELL_RADIO_MIN_CHANNEL_NUM so we dont use channel 0 as it may bleed below 2.400 GHz
}

static void CABELL_get_telemetry()
{
// calculate TX rssi based on past 250 expected telemetry packets.  Cannot use full second count because telemetry_counter is not large enough
 if (++rfState16_p2M > 250)
  {
   telemetryData.rssi[1].set(receive_seq_p2M);
   receive_seq_p2M = 0;
   rfState16_p2M = 0;
  }

// Process incoming telemetry packet_p2M of it was received
 if (NRF24L01_ReadReg(NRF24L01_07_STATUS) & _BV(NRF24L01_07_RX_DR))
  {
   // data received from model
   NRF24L01_ReadPayload(packet_p2M, CABELL_TELEMETRY_PACKET_LENGTH);
   if ((packet_p2M[0] & 0x7F) == telemetryResponse)	// ignore high order bit in compare because it toggles with each packet_p2M
    {
     frskyStreaming = frskyStreaming ? FRSKY_TIMEOUT10ms : FRSKY_TIMEOUT_FIRST;
     telemetryData.rssi[0].set(packet_p2M[1]);	// Packet rate 0 to 255 where 255 is 100% packet_p2M rate
     telemetryData.analog[TELEM_ANA_A1].set(packet_p2M[2], g_model.telemetry.channels[TELEM_ANA_A1].type);	// Directly from analog input of receiver, but reduced to 8-bit depth (0 to 255).  Scaling depends on the input to the analog pin of the receiver.
     telemetryData.analog[TELEM_ANA_A2].set(packet_p2M[3], g_model.telemetry.channels[TELEM_ANA_A2].type);	// Directly from analog input of receiver, but reduced to 8-bit depth (0 to 255).  Scaling depends on the input to the analog pin of the receiver.
     receive_seq_p2M++;
    }
  }
 else
  {
   // If no telemetry packet_p2M was received then delay by the typical telemetry packet_p2M processing time
   // This is done to try to keep the sendPacket process timing more consistent. Since the SPI payload read takes some time
   _delay_us(50);
  }
 NRF24L01_SetTxRxMode(TX_EN);
 NRF24L01_FlushRx();
}

static uint16_t CABELL_send_packet(uint8_t bindMode)
{
 uint8_t option = cabell_num_ch_raw | (16 * cabell_sub_protocol);
 if (!bindMode && cabell_telemetry)		// check for incoming packet_p2M and switch radio back to TX mode if we were listening for telemetry
  CABELL_get_telemetry();

 CABELL_RxTxPacket_t TxPacket;

 uint8_t channelReduction = cabell_num_ch;
 if (bindMode)
  channelReduction = 0;	// Send full packet_p2M to bind as higher channels will contain bind info

 packetSize_p2M = sizeof(TxPacket) - ((((channelReduction - (channelReduction%2))/ 2)) * 3);		// reduce 3 bytes per 2 channels, but not last channel if it is odd
 uint8_t maxPayloadValueIndex = sizeof(TxPacket.payloadValue) - (sizeof(TxPacket) - packetSize_p2M);
 if (cabell_ubind && !bindMode)
  {
   TxPacket.RxMode = unBind;
   TxPacket.option = option;
  }
 else
  {
   /*if (sub_protocol == CABELL_SET_FAIL_SAFE && !bindMode)
   	TxPacket.RxMode = CABELL_RxTxPacket_t::setFailSafe;
   else*/
   {
    if (bindMode)
     {
      TxPacket.RxMode = bindFalesafeNoPulse; // use always this mode in OpenAVRc
     }
    else
     {
      if (cabell_telemetry)
       TxPacket.RxMode = normalWithTelemetry;
      else
       TxPacket.RxMode = normal;
     }
   }
   //remove channel reduction if in bind mode
   TxPacket.option = (bindMode) ? (option & (~CABELL_OPTION_MASK_CHANNEL_REDUCTION)) : option;
  }

 TxPacket.reserved = 0;

// Set channel for next transmission
 channel_index_p2M = CABELL_getNextChannel (channel_used_p2M,CABELL_RADIO_CHANNELS, channel_index_p2M);
 TxPacket.reserved |= channel_index_p2M & CABELL_RESERVED_MASK_CHANNEL;

 TxPacket.modelNum = RXNUM;

 uint16_t checkSum = TxPacket.modelNum + TxPacket.option + TxPacket.RxMode  + TxPacket.reserved;		// Start Calculate checksum

 int16_t payloadIndex = 0;
 uint16_t holdValue;

 for (int x = 0; (x < CABELL_NUM_CHANNELS - channelReduction); x++)
  {
   // valid channel values are 1000 to 2000
   holdValue = (FULL_CHANNEL_OUTPUTS(x))/2; // +-1024 to +-612
   holdValue += PPM_CENTER; // + 1500 offset
   limit<int16_t>(1000, holdValue, 2000);

   if (bindMode)
    {
     switch (x)
      {
      //tx address sent for bind
      case 11			:
       holdValue = 1000 + temp_rfid_addr_p2M[0];
       break;
      case 12			:
       holdValue = 1000 + temp_rfid_addr_p2M[1];
       break;
      case 13			:
       holdValue = 1000 + temp_rfid_addr_p2M[2];
       break;
      case 14			:
       holdValue = 1000 + temp_rfid_addr_p2M[3];
       break;
      case 15			:
       holdValue = 1000 + temp_rfid_addr_p2M[0];
       break;
      }
    }

   // use 12 bits per value
   if (x % 2)
    {
     //output channel number is ODD
     holdValue = holdValue<<4;
     payloadIndex--;
    }
   else
    holdValue &= 0x0FFF;
   TxPacket.payloadValue[payloadIndex] |=  (uint8_t)(holdValue & 0x00FF);
   payloadIndex++;
   TxPacket.payloadValue[payloadIndex] |=  (uint8_t)((holdValue>>8) & 0x00FF);
   payloadIndex++;
  }

 for(int x = 0; x < maxPayloadValueIndex ; x++)
  checkSum += TxPacket.payloadValue[x];  // Finish Calculate checksum

 TxPacket.checkSum_MSB = checkSum >> 8;
 TxPacket.checkSum_LSB = checkSum & 0x00FF;

 NRF24L01_WriteReg(NRF24L01_05_RF_CH,channel_index_p2M);

 uint8_t* p = reinterpret_cast<uint8_t*>(&TxPacket.RxMode);
 *p &= 0x7F;                  // Make sure 8th bit is clear
 *p |= (packet_count_p2M++)<<7;   // This causes the 8th bit of the first byte to toggle with each xmit so consecutive payloads are not identical.
// This is a work around for a reported bug in clone NRF24L01 chips that mis-took this case for a re-transmit of the same packet_p2M.

 NRF24L01_ManagePower();
 NRF24L01_WritePayload((uint8_t*)&TxPacket, packetSize_p2M);
 uint16_t packet_period;
 if (!bindMode && cabell_telemetry)
  {
   // switch radio to rx as soon as packet_p2M is sent
   // calculate transmit time based on packet_p2M size and data rate of 1MB per sec
   // This is done because polling the status register during xmit caused issues.
   // bits = packst_size * 8  +  73 bits overhead
   // at 250 Kbs per sec, one bit is 4 uS
   // then add 140 uS which is 130 uS to begin the xmit and 10 uS fudge factor
   uint16_t delay = (((((uint16_t)packetSize_p2M * 8)  +  73) * 4) + 140);
   do
    {
     _delay_us(1);
    }
   while (--delay);
   // increase packet_p2M period by 100 us for each channel over 6
   packet_period = CABELL_PACKET_PERIOD + (100 * limit<int16_t>(0,(CABELL_NUM_CHANNELS - channelReduction - 6 ), 10));

   NRF24L01_WriteReg(NRF24L01_00_CONFIG, 0x0F);  // RX mode with 16 bit CRC
  }
 else
  packet_period = CABELL_PACKET_PERIOD;   // Standard packet_p2M period when not in telemetry mode.
 return packet_period;
}

static uint16_t CABELL_bind_cb()
{
 SCHEDULE_MIXER_END_IN_US(18000); // Schedule next Mixer calculations.
 CABELL_send_packet(1);
 heartbeat |= HEART_TIMER_PULSES;
 CALCULATE_LAT_JIT(); // Calculate latency and jitter.
 return CABELL_PACKET_PERIOD *2;
}

static uint16_t CABELL_cb()
{
 if (++rfState8_p2M >= 4)
  {
   rfState8_p2M = 0;
   SCHEDULE_MIXER_END_IN_US(12000); // Schedule next Mixer calculations.
  }
 uint16_t protocol_period = CABELL_send_packet(0);
 heartbeat |= HEART_TIMER_PULSES;
 CALCULATE_LAT_JIT(); // Calculate latency and jitter.
 return protocol_period *2; // From 3mS to 4mS
}

static void CABELL_initialize(uint8_t bind)
{
 loadrfidaddr();
 channel_index_p2M = 0;
 receive_seq_p2M = 0;
 rfState8_p2M = 0;
 CABELL_init(bind);

 if (bind)
  {
   PROTO_Start_Callback( CABELL_bind_cb);
  }
 else
  {
   PROTO_Start_Callback( CABELL_cb);
  }
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
