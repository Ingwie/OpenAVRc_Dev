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

//******************************************************************************************************************************************************
// Included communication nRF24L01P "Stanek". Fixed RF channel, fixed address.                                                                         *
// Channel reduction 2 -> 12ch. This is the maximum in the "Servo" library on the Atmega328P processor.                                                *
// Telemetry A1 for measuring 1S Lipo power supply RX and TRSS.                                                                                        *
// Support for Arduino-based receivers and RF24 libraries from this repository https://github.com/stanekTM/RC_RX_nRF24L01_Telemetry_Motor_Driver_Servo *
// Thanks to "Bracame Ingwie" from the "OpenAVRc" https://github.com/Ingwie/OpenAVRc_Dev team for the code base.                                       *
//******************************************************************************************************************************************************


#include "../OpenAVRc.h"


const static RfOptionSettingsvar_t RfOpt_STANEK_Ser[] PROGMEM =
{
 /*rfProtoNeed*/PROTO_NEED_SPI /*| BOOL1USED | BOOL2USED*/, //can be PROTO_NEED_SPI | BOOL1USED | BOOL2USED | BOOL3USED
 /*rfSubTypeMax*/0,
 /*rfOptionValue1Min*/2,  // Channel min
 /*rfOptionValue1Max*/12, // Channel max
 /*rfOptionValue2Min*/0,
 /*rfOptionValue2Max*/0,
 /*rfOptionValue3Max*/3,  // RF power
};


uint8_t TX_RX_ADDRESS[] = "jirka"; // setting RF channels address (5 bytes number or character)

#define STANEK_RF_CHANNEL     76   // which RF channel to communicate on (0-125, 2.4Ghz + 76 = 2.476Ghz)

#define STANEK_PACKET_PERIOD	3000 //3000 Do not set too low or else next packet may not be finished transmitting before the channel is changed next time around

#define STANEK_RC_CHANNELS		12	 // Number of RC channels that can be sent in one packet

#define STANEK_PACKET_SIZE	  24	 // STANEK_PACKET_SIZE = STANEK_RC_CHANNELS * 2

#define STANEK_PAYLOAD_BYTES	24	 // 12 bits per value * 12 channels

#define STANEK_TELEMETRY_PACKET_SIZE	4

#define stanek_telemetry          g_model.rfOptionBool1
#define stanek_reduction    (12 - g_model.rfOptionValue1)


//**********************************************************************************************************************************
//**********************************************************************************************************************************
//**********************************************************************************************************************************
static void STANEK_setAddress()
{
  channel_index_p2M = STANEK_RF_CHANNEL;	// Initialize the channel

  uint8_t RX_P1_ADDRESS = ~TX_RX_ADDRESS[5]; // Invert bits for reading so that telemetry packets have a different address

  NRF24L01_WriteRegisterMulti(NRF24L01_0A_RX_ADDR_P0, (uint8_t*)(&TX_RX_ADDRESS), 5);
  NRF24L01_WriteRegisterMulti(NRF24L01_0B_RX_ADDR_P1, (uint8_t*)(&RX_P1_ADDRESS), 5);
  NRF24L01_WriteRegisterMulti(NRF24L01_10_TX_ADDR,    (uint8_t*)(&TX_RX_ADDRESS), 5);
}

//**********************************************************************************************************************************
//**********************************************************************************************************************************
//**********************************************************************************************************************************
static void STANEK_init()
{
 NRF24L01_Initialize();
 NRF24L01_ManagePower();
 NRF24L01_SetBitrate(NRF24L01_BR_250K);			    	// NRF24L01_BR_250K (fails for units without +), NRF24L01_BR_1M, NRF24L01_BR_2M
 NRF24L01_WriteReg(NRF24L01_01_EN_AA, 0x00);  	  //0x00 No Auto Acknowledgment on all data pipes nechat
 NRF24L01_SetTxRxMode(TX_EN);					          	// Power up and 16 bit CRC

 STANEK_setAddress();

 NRF24L01_WriteReg(NRF24L01_07_STATUS, 0x70);     //0x70 Reset status
 NRF24L01_FlushTx();
 NRF24L01_FlushRx();
 NRF24L01_WriteReg(NRF24L01_02_EN_RXADDR, 0x03);  //0x03 Enable only data pipe nechat
 NRF24L01_WriteReg(NRF24L01_11_RX_PW_P0, 0x20);		//0x20 32 byte packet length
 NRF24L01_WriteReg(NRF24L01_12_RX_PW_P1, 0x20);		//0x20 32 byte packet length

 NRF24L01_WriteReg(NRF24L01_03_SETUP_AW, 0x03);   //0x03 5 bytes adress
 NRF24L01_WriteReg(NRF24L01_04_SETUP_RETR, 0x1A);	//0x1A ,0xFF ,0x5F no retransmits
 NRF24L01_Activate(0x73);					            		// Activate feature register
 NRF24L01_WriteReg(NRF24L01_1C_DYNPD, 0x3F);			//0x3F Enable dynamic payload length on all pipes
 NRF24L01_WriteReg(NRF24L01_1D_FEATURE, 0x04);		//0x04 Enable dynamic Payload Length
 NRF24L01_Activate(0x73);                         // Activate feature register
}

//**********************************************************************************************************************************
//**********************************************************************************************************************************
//**********************************************************************************************************************************
static void STANEK_get_telemetry()
{
  // calculate TX rssi based on past 250 expected telemetry packets. Cannot use full second count because telemetry_counter is not large enough
  if (++rfState16_p2M > 250)
  {
   telemetryData.rssi[1].set(receive_seq_p2M);
   receive_seq_p2M = 0;
   rfState16_p2M = 0;
  }

  // Process incoming telemetry packet of it was received
  if (NRF24L01_ReadReg(NRF24L01_07_STATUS) & _BV(NRF24L01_07_RX_DR)) // todo try NRF24L01_NOP
  {
   // data received from model
   NRF24L01_ReadPayload(telem_save_data_p2M, STANEK_TELEMETRY_PACKET_SIZE);

   frskyStreaming = frskyStreaming ? FRSKY_TIMEOUT10ms : FRSKY_TIMEOUT_FIRST;
   telemetryData.rssi[0].set(telem_save_data_p2M[1]); //1 Packet rate 0 to 255 where 255 is 100% packet rate
   telemetryData.analog[TELEM_ANA_A1].set(telem_save_data_p2M[0], g_model.telemetry.channels[TELEM_ANA_A1].type); //0 Directly from analog input of receiver, but reduced to 8-bit depth (0 to 255).
   telemetryData.analog[TELEM_ANA_A2].set(telem_save_data_p2M[2], g_model.telemetry.channels[TELEM_ANA_A2].type); //2 Scaling depends on the input to the analog pin of the receiver.
   receive_seq_p2M++;
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

//**********************************************************************************************************************************
//**********************************************************************************************************************************
//**********************************************************************************************************************************
static void STANEK_send_packet()
{
  if (stanek_telemetry) // check for incoming packet and switch radio back to TX mode if we were listening for telemetry
  {
    STANEK_get_telemetry();
  }

 uint8_t channelReduction = stanek_reduction;

 packetSize_p2M = STANEK_PACKET_SIZE - (channelReduction * 2);

 uint8_t maxPayloadValueIndex = STANEK_PAYLOAD_BYTES - (STANEK_PACKET_SIZE - packetSize_p2M);


 uint8_t payloadIndex = 0;
 int16_t holdValue;

 memclear(&packet_p2M[0], maxPayloadValueIndex);  // Reset values

 for (uint8_t x = 0; x < (STANEK_RC_CHANNELS - channelReduction); x++)
  {
   // valid channel values are 1000 to 2000
   holdValue = (FULL_CHANNEL_OUTPUTS(x))/2; // +-1024 to +-512
   holdValue += PPM_CENTER; // + 1500 offset
   holdValue = limit<int16_t>(1000, holdValue, 2000);

   // use 12 bits per value
   if (x % 2)
    holdValue &= 0x0FFF; //4095

   packet_p2M[0 + payloadIndex] |= holdValue & 0xFF; //255
   payloadIndex++;
   packet_p2M[0 + payloadIndex] |= holdValue >> 8;
   payloadIndex++;
  }

 uint16_t checkSum; // Start Calculate checksum

 for(uint8_t x = 0; x < maxPayloadValueIndex; x++)
  checkSum += packet_p2M[0 + x];  // Finish Calculate checksum

 NRF24L01_WriteReg(NRF24L01_05_RF_CH, channel_index_p2M); // send channel
 NRF24L01_ManagePower();
 NRF24L01_WritePayload(packet_p2M, packetSize_p2M);       // and payload
}

//**********************************************************************************************************************************
//**********************************************************************************************************************************
//**********************************************************************************************************************************
static uint16_t STANEK_manage_time()
{
 uint16_t packet_period;

 if (stanek_telemetry)
  {
    // switch radio to rx as soon as packet is sent
    // calculate transmit time based on packet size and data rate of 250 Kbs per sec
    // This is done because polling the status register during xmit caused issues.
    // then add 140 uS which is 130 uS to begin the xmit and 10 uS fudge factor
    // then add 460 uS -> Time to compute CABELL_send_packet()
    uint16_t rxDelay = (/* Time air */(4 * 8 *(1 + 5 + packetSize_p2M + 2)) + 9) + 140 + 25;


   if (!telem_save_seq_p2M)
    {
     bind_counter_p2M = PROTOCOL_GetElapsedTime(); // use bind_counter_p2M as memory only here
     packet_period = rxDelay + bind_counter_p2M;
     telem_save_seq_p2M = 1; // indicate to switch to RX mode next time
    }
   else
    {
     // increase packet period by 100 us for each channel over 6
     packet_period = limit<int16_t>(0, (g_model.rfOptionValue1 - 6), 10); //0, 6, 10
     packet_period *= 100;
     packet_period += STANEK_PACKET_PERIOD;
     packet_period -= rxDelay + bind_counter_p2M; // remove RX time
     telem_save_seq_p2M = 0; // reset switch to RX
    }
  }
 else
  packet_period = STANEK_PACKET_PERIOD; // Standard packet period when not in telemetry mode

 return packet_period;
}

//**********************************************************************************************************************************
//**********************************************************************************************************************************
//**********************************************************************************************************************************
static uint16_t STANEK_cb()
{
 if (stanek_telemetry && telem_save_seq_p2M) // we need to switch to RX mode to read telemetry
  {
   NRF24L01_WriteReg(NRF24L01_00_CONFIG, 0x7F); //0x7F RX mode with 16 bit CRC no IRQ
  }
 else
  {
   if (++rfState8_p2M >= 4)
    {
     rfState8_p2M = 0;
     SCHEDULE_MIXER_END_IN_US(12000); // Schedule next Mixer calculations
    }
   STANEK_send_packet();
  }
 uint16_t protocol_period = STANEK_manage_time();
 heartbeat |= HEART_TIMER_PULSES;
 CALCULATE_LAT_JIT();        // Calculate latency and jitter
 return protocol_period * 2; // From 3mS to 4mS
}

//**********************************************************************************************************************************
//**********************************************************************************************************************************
//**********************************************************************************************************************************
static void STANEK_initialize()
{
  STANEK_init();
  PROTO_Start_Callback(STANEK_cb);
}

//**********************************************************************************************************************************
//**********************************************************************************************************************************
//**********************************************************************************************************************************
const void *STANEK_Cmds(enum ProtoCmds cmd)
{
  switch(cmd)
  {
   case PROTOCMD_INIT:
    STANEK_initialize();
    return 0;
   case PROTOCMD_RESET:
    PROTO_Stop_Callback();
    NRF24L01_Reset();
    return 0;
   case PROTOCMD_BIND:
    STANEK_initialize();
    return 0;
   case PROTOCMD_GETOPTIONS:
    SetRfOptionSettings(pgm_get_far_address(RfOpt_STANEK_Ser),
                        STR_DUMMY,   //Sub proto
                        STR_NUMCH,   //Option 1 (int) Num channels (4-16Ch : -> 12 to 0)
                        STR_DUMMY,   //Option 2 (int)
                        STR_RFPOWER, //Option 3 (uint 0 to 31)
                        STR_DUMMY,   //OptionBool 1
                        STR_DUMMY,   //OptionBool 2
                        STR_DUMMY    //OptionBool 3
                       );
    return 0;
   default:
    break;
  }
  return 0;
}

