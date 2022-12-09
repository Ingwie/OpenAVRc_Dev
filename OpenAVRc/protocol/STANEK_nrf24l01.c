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

//***************************************************************************************************************
// Thanks to "Bracame Ingwie" from the "OpenAVRc" https://github.com/Ingwie/OpenAVRc_Dev team for the code base.
//
// Support for custom Arduino-based DIY receivers with RF24 library from this repository:
// https://github.com/stanekTM/RX_nRF24L01_Telemetry_Motor_Driver_Servo
//
// Included communication nRF24L01P "Stanek". Fixed RF channel, fixed address, channel reduction 2 -> 12ch.
//***************************************************************************************************************


#include "../OpenAVRc.h"

// define pulses2MHz reusable values (13 bytes max)
#define STANEK_RFSTATE             BYTE_P2M(1)
#define STANEK_CH_IDX_P2M          BYTE_P2M(2)
#define STANEK_REC_SEQ_P2M         BYTE_P2M(3)
#define STANEK_PACKET_SIZE_P2M     BYTE_P2M(4)
#define STANEK_TELEM_SAVE_SEQ_P2M  BYTE_P2M(5)

#define STANEK_RF_STATE16_P2M      WORD_P2M(1)
#define STANEK_BIND_COUNTER_16_P2M WORD_P2M(2)
//***********************************************//


const static RfOptionSettingsvar_t RfOpt_STANEK_Ser[] PROGMEM =
{
  /*rfProtoNeed*/PROTO_NEED_SPI /*| BOOL1USED | BOOL2USED*/, // can be PROTO_NEED_SPI | BOOL1USED | BOOL2USED | BOOL3USED
  /*rfSubTypeMax*/ 0,
  /*rfOptionValue1Min*/ 2,  // rc channel min
  /*rfOptionValue1Max*/ 12, // rc channel max
  /*rfOptionValue2Min*/ 0,
  /*rfOptionValue2Max*/ 0,
  /*rfOptionValue3Max*/ 3,  // rf power
};


uint8_t TX_RX_ADDRESS[] = "jirka";      // setting RF channels address (5 bytes number or character)

#define STANEK_RF_CHANNEL      76       // which RF channel to communicate on (0-125, 2.4Ghz + 76 = 2.476Ghz)

#define STANEK_PACKET_PERIOD	 3000     // do not set too low or else next packet may not be finished transmitting before the channel is changed next time around

#define STANEK_RC_CHANNELS     12       // number of RC channels that can be sent in one packet

#define STANEK_RC_PACKET_SIZE  24       // STANEK_RC_PACKET_SIZE = STANEK_RC_CHANNELS * 2

#define STANEK_TELEMETRY_PACKET_SIZE  3 // RSSI, A1, A2

#define stanek_rc_channels_reduction  (12 - g_model.rfOptionValue1)


//**********************************************************************************************************************************
//**********************************************************************************************************************************
//**********************************************************************************************************************************
static void STANEK_setAddress()
{
  STANEK_CH_IDX_P2M = STANEK_RF_CHANNEL; // initialize the channel
  
  NRF24L01_WriteRegisterMulti(NRF24L01_10_TX_ADDR,    (uint8_t*)(&TX_RX_ADDRESS), 5);
  NRF24L01_WriteRegisterMulti(NRF24L01_0A_RX_ADDR_P0, (uint8_t*)(&TX_RX_ADDRESS), 5);
}

//**********************************************************************************************************************************
//**********************************************************************************************************************************
//**********************************************************************************************************************************
static void STANEK_init()
{
  NRF24L01_Initialize();
  NRF24L01_ManagePower();
  NRF24L01_SetBitrate(NRF24L01_BR_250K);           // NRF24L01_BR_250K (fails for units without +), NRF24L01_BR_1M, NRF24L01_BR_2M
  NRF24L01_WriteReg(NRF24L01_01_EN_AA, 0x00);      // no auto acknowledgment on all data pipes
  NRF24L01_SetTxRxMode(TX_EN);					           // power up and 16 bit CRC

  STANEK_setAddress();

  NRF24L01_WriteReg(NRF24L01_07_STATUS, 0x70);     // reset status
  NRF24L01_FlushTx();
  NRF24L01_FlushRx();
  NRF24L01_WriteReg(NRF24L01_02_EN_RXADDR, 0x03);  // enable only data pipe

  NRF24L01_WriteReg(NRF24L01_03_SETUP_AW, 0x03);   // 5 bytes adress
  NRF24L01_WriteReg(NRF24L01_04_SETUP_RETR, 0x1A); // retransmits
  NRF24L01_Activate(0x73);	                       // activate feature register
  NRF24L01_WriteReg(NRF24L01_1C_DYNPD, 0x3F);      // enable dynamic payload length on all pipes
  NRF24L01_WriteReg(NRF24L01_1D_FEATURE, 0x04);    // enable dynamic payload length
  NRF24L01_Activate(0x73);                         // activate feature register
}

//**********************************************************************************************************************************
//**********************************************************************************************************************************
//**********************************************************************************************************************************
//static void STANEK_get_telemetry()
FORCEINLINE void STANEK_get_telemetry() //*
{
  // calculate TX rssi based on past 250 expected telemetry packets. Cannot use full second count because telemetry_counter is not large enough
  if (++STANEK_RF_STATE16_P2M > 250)
  {
#if defined(FRSKY)
    telemetryData.rssi[1].set(STANEK_REC_SEQ_P2M);
#endif
    STANEK_REC_SEQ_P2M = 0;
    STANEK_RF_STATE16_P2M = 0;
  }
  
  // process incoming telemetry packet of it was received
  //if (NRF24L01_ReadReg(NRF24L01_07_STATUS) & _BV(NRF24L01_07_RX_DR))
  if (NRF24L01_NOP() & _BV(NRF24L01_07_RX_DR)) //*
  {
    // data received from model
    NRF24L01_ReadPayload(telem_save_data_buff, STANEK_TELEMETRY_PACKET_SIZE);
    
#if defined(FRSKY)
    frskyStreaming = frskyStreaming ? FRSKY_TIMEOUT10ms : FRSKY_TIMEOUT_FIRST;
    
    telemetryData.rssi[0].set(telem_save_data_buff[0]); // packet rate 0 to 255 where 255 is 100% packet rate
    telemetryData.analog[TELEM_ANA_A1].set(telem_save_data_buff[1], g_model.telemetry.channels[TELEM_ANA_A1].type); // directly from analog input of receiver, but reduced to 8-bit depth (0 to 255)
    telemetryData.analog[TELEM_ANA_A2].set(telem_save_data_buff[2], g_model.telemetry.channels[TELEM_ANA_A2].type); // Scaling depends on the input to the analog pin of the receiver
#endif
    STANEK_REC_SEQ_P2M++;
  }
  else
  {
    // if no telemetry packet was received then delay by the typical telemetry packet processing time.
    // This is done to try to keep the send packet process timing more consistent. Since the SPI payload read takes some time
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
  STANEK_get_telemetry();
  
  STANEK_PACKET_SIZE_P2M = STANEK_RC_PACKET_SIZE - (stanek_rc_channels_reduction * 2);
  
  uint8_t payloadIndex = 0;
  int16_t holdValue;
  
  memclear(&packet_p2M[0], STANEK_PACKET_SIZE_P2M); // reset values
  
  for (uint8_t x = 0; x < (STANEK_RC_CHANNELS - stanek_rc_channels_reduction); x++)
  {
    // valid channel values are 1000 to 2000
    holdValue = (FULL_CHANNEL_OUTPUTS(x)) / 2; // +-1024 to +-512
    holdValue += PPM_CENTER; // + 1500 offset
    holdValue = limit<int16_t>(1000, holdValue, 2000);
    
    // use 12 bits per value
    holdValue &= 0x0FFF; // 4095
    
    packet_p2M[payloadIndex] |= holdValue & 0xFF; // 255
    payloadIndex++;
    packet_p2M[payloadIndex] |= holdValue >> 8;
    payloadIndex++;
  }
  
  NRF24L01_WriteReg(NRF24L01_05_RF_CH, STANEK_CH_IDX_P2M);   // send channel
  NRF24L01_ManagePower();
  NRF24L01_WritePayload(packet_p2M, STANEK_PACKET_SIZE_P2M); // and payload
}

//**********************************************************************************************************************************
//**********************************************************************************************************************************
//**********************************************************************************************************************************
static uint16_t STANEK_manage_time()
{
  uint16_t packet_period;
  
  // switch radio to rx as soon as packet is sent.
  // Calculate transmit time based on packet size and data rate of 250 Kbs per sec.
  uint16_t rxDelay = /* Variable time air */(4 * 8 * STANEK_PACKET_SIZE_P2M) + /* Fixed */450;
  
  if (!STANEK_TELEM_SAVE_SEQ_P2M)
  {
    STANEK_BIND_COUNTER_16_P2M = PROTOCOL_GetElapsedTime(); // use STANEK_BIND_COUNTER_16_P2M as memory only here
    packet_period = rxDelay + STANEK_BIND_COUNTER_16_P2M;
    STANEK_TELEM_SAVE_SEQ_P2M = 1; // indicate to switch to RX mode next time
  }
  else
  {
    // increase packet period by 100us for each channel over 6
    packet_period = limit<uint16_t>(0, (uint8_t)(g_model.rfOptionValue1 - 6), 10);
    packet_period *= 100;
    packet_period += STANEK_PACKET_PERIOD;
    packet_period -= rxDelay + STANEK_BIND_COUNTER_16_P2M; // remove RX time
    STANEK_TELEM_SAVE_SEQ_P2M = 0; // reset switch to RX
  }
  
  return packet_period;
}

//**********************************************************************************************************************************
//**********************************************************************************************************************************
//**********************************************************************************************************************************
static uint16_t STANEK_cb()
{
  if (STANEK_TELEM_SAVE_SEQ_P2M) // we need to switch to RX mode to read telemetry
  {
    NRF24L01_WriteReg(NRF24L01_00_CONFIG, 0x7F); // RX mode with 16 bit CRC no IRQ
    //NRF24L01_WriteReg(NRF24L01_00_CONFIG, 0x0F); //* RX mode with 16 bit CRC
  }
  else
  {
    if (++STANEK_RFSTATE >= 4)
    {
      STANEK_RFSTATE = 0;
      SCHEDULE_MIXER_END_IN_US(12000); // schedule next mixer calculations
    }
    
    STANEK_send_packet();
  }
  
  uint16_t protocol_period = STANEK_manage_time();
  
  heartbeat |= HEART_TIMER_PULSES;
  
  CALCULATE_LAT_JIT(); // calculate latency and jitter
  
  return protocol_period * 2; // from 3ms to 4ms
}

//**********************************************************************************************************************************
//**********************************************************************************************************************************
//**********************************************************************************************************************************
const void *STANEK_Cmds(enum ProtoCmds cmd)
{
  switch(cmd)
  {
   case PROTOCMD_INIT:
     STANEK_init();
     PROTO_Start_Callback(STANEK_cb);
     return 0;
   case PROTOCMD_RESET:
     PROTO_Stop_Callback();
     NRF24L01_Reset();
     return 0;
   case PROTOCMD_BIND:
     STANEK_init();
     PROTO_Start_Callback(STANEK_cb);
     return 0;
   case PROTOCMD_GETOPTIONS:
     SetRfOptionSettings(pgm_get_far_address(RfOpt_STANEK_Ser),
                         STR_DUMMY,   // sub protocol
                         STR_NUMCH,   // option 1 (int) num channels (2 - 12ch)
                         STR_DUMMY,   // option 2 (int)
                         STR_RFPOWER, // option 3 (uint 0 to 31)
                         STR_DUMMY,   // OptionBool 1
                         STR_DUMMY,   // OptionBool 2
                         STR_DUMMY    // OptionBool 3
                        );
     return 0;
   default:
     break;
  }
  
  return 0;
}
 
