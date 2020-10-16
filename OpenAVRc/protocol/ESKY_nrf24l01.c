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


const static int8_t RfOpt_ESKY_Ser[] PROGMEM = {
  /*rfProtoNeed*/PROTO_NEED_SPI, //can be PROTO_NEED_SPI | BOOL1USED | BOOL2USED | BOOL3USED
  /*rfSubTypeMax*/0,
  /*rfOptionValue1Min*/0,
  /*rfOptionValue1Max*/0,
  /*rfOptionValue2Min*/0,
  /*rfOptionValue2Max*/0,
  /*rfOptionValue3Max*/4,    // RF POWER
};


const static uint8_t ZZ_ESKYInitSequence[] PROGMEM = {
};

static void ESKY_init()
{
}


static void ESKY_send_data_packet()
{
}

static void ESKY_send_bind_packet()
{
}

static uint16_t ESKY_bind_cb()
{
  SCHEDULE_MIXER_END_IN_US(18000); // Schedule next Mixer calculations.
  send_bind_packet();
  heartbeat |= HEART_TIMER_PULSES;
  CALCULATE_LAT_JIT(); // Calculate latency and jitter.
  return 18000U *2;
}

static uint16_t ESKY_cb()
{
  SCHEDULE_MIXER_END_IN_US(12000); // Schedule next Mixer calculations.
  ESKY_send_data_packet();
  heartbeat |= HEART_TIMER_PULSES;
  CALCULATE_LAT_JIT(); // Calculate latency and jitter.
  return 12000U *2;
}


static void ESKY_initialize(uint8_t bind)
{
  ESKY_init();
  if (bind) {
  PROTO_Start_Callback( ESKY_bind_cb);
  } else {
  PROTO_Start_Callback( ESKY_cb);
  }
}

const void *ESKY_Cmds(enum ProtoCmds cmd)
{
  switch(cmd) {
  case PROTOCMD_INIT:
    ESKY_initialize(0);
    return 0;
  case PROTOCMD_RESET:
    PROTO_Stop_Callback();
    NRF24L01_Reset();
    return 0;
  case PROTOCMD_BIND:
    ESKY_initialize(1);
    return 0;
  case PROTOCMD_GETOPTIONS:
          SetRfOptionSettings(pgm_get_far_address(RfOpt_ESKY_Ser),
                        STR_DUMMY,      //Sub proto
                        STR_DUMMY,      //Option 1 (int)
                        STR_DUMMY,      //Option 2 (int)
                        STR_RFPOWER,    //Option 3 (uint 0 to 31)
                        STR_DUMMY,      //OptionBool 1
                        STR_DUMMY,      //OptionBool 2
                        STR_DUMMY       //OptionBool 3
                        );
    return 0;
  default:
    break;
  }
  return 0;
}










#define ESKY_BIND_COUNT		1000
#define ESKY_PACKET_PERIOD	3333
#define ESKY_PAYLOAD_SIZE	13
#define ESKY_PACKET_CHKTIME	100 // Time to wait for packet to be sent (no ACK, so very short)

static void ESKY_set_data_address()
{
	NRF24L01_WriteReg(NRF24L01_03_SETUP_AW, 0x02);     // 4-byte RX/TX address for regular packets
	NRF24L01_WriteRegisterMulti(NRF24L01_0A_RX_ADDR_P0, rx_tx_addr, 4);
	NRF24L01_WriteRegisterMulti(NRF24L01_10_TX_ADDR,    rx_tx_addr, 4);
}

static void ESKY_init(uint8_t bind)
{
	// 2-bytes CRC, radio off
	NRF24L01_WriteReg(NRF24L01_00_CONFIG, _BV(NRF24L01_00_EN_CRC) | _BV(NRF24L01_00_CRCO));
	NRF24L01_WriteReg(NRF24L01_01_EN_AA, 0x00);            // No Auto Acknowledgement
	NRF24L01_WriteReg(NRF24L01_02_EN_RXADDR, 0x01);        // Enable data pipe 0
	if (bind)
	{
		NRF24L01_WriteReg(NRF24L01_03_SETUP_AW, 0x01);     // 3-byte RX/TX address for bind packets
		NRF24L01_WriteRegisterMulti(NRF24L01_0A_RX_ADDR_P0, (uint8_t*)"\x00\x00\x00", 3);
		NRF24L01_WriteRegisterMulti(NRF24L01_10_TX_ADDR,    (uint8_t*)"\x00\x00\x00", 3);
	}
	else
		ESKY_set_data_address();
	NRF24L01_WriteReg(NRF24L01_04_SETUP_RETR, 0);          // No auto retransmission
	NRF24L01_WriteReg(NRF24L01_05_RF_CH, 50);              // Channel 50 for bind packets
	NRF24L01_SetBitrate(NRF24L01_BR_1M);                   // 1Mbps
	NRF24L01_SetPower();
	NRF24L01_WriteReg(NRF24L01_07_STATUS, 0x70);           // Clear data ready, data sent, and retransmit
	NRF24L01_WriteReg(NRF24L01_11_RX_PW_P0, ESKY_PAYLOAD_SIZE);  // bytes of data payload for pipe 0
	NRF24L01_WriteReg(NRF24L01_12_RX_PW_P1, ESKY_PAYLOAD_SIZE);
	NRF24L01_WriteReg(NRF24L01_13_RX_PW_P2, ESKY_PAYLOAD_SIZE);
	NRF24L01_WriteReg(NRF24L01_14_RX_PW_P3, ESKY_PAYLOAD_SIZE);
	NRF24L01_WriteReg(NRF24L01_15_RX_PW_P4, ESKY_PAYLOAD_SIZE);
	NRF24L01_WriteReg(NRF24L01_16_RX_PW_P5, ESKY_PAYLOAD_SIZE);
	NRF24L01_WriteReg(NRF24L01_17_FIFO_STATUS, 0x00);      // Just in case, no real bits to write here
}

static void ESKY_init2()
{
	NRF24L01_FlushTx();
	packet_sent = 0;
	hopping_frequency_no = 0;
	uint16_t channel_ord = rx_tx_addr[0] % 74;
	hopping_frequency[12] = 10 + (uint8_t)channel_ord;	//channel_code
	uint8_t channel1, channel2;
	channel1 = 10 + (uint8_t)((37 + channel_ord*5) % 74);
	channel2 = 10 + (uint8_t)((     channel_ord*5) % 74) ;

	hopping_frequency[0] = channel1;
	hopping_frequency[1] = channel1;
	hopping_frequency[2] = channel1;
	hopping_frequency[3] = channel2;
	hopping_frequency[4] = channel2;
	hopping_frequency[5] = channel2;

	//end_bytes
	hopping_frequency[6] = 6;
	hopping_frequency[7] = channel1*2;
	hopping_frequency[8] = channel2*2;
	hopping_frequency[9] = 6;
	hopping_frequency[10] = channel1*2;
	hopping_frequency[11] = channel2*2;

	// Turn radio power on
	NRF24L01_SetTxRxMode(TX_EN);
}

static void ESKY_send_packet(uint8_t bind)
{
	uint8_t rf_ch = 50; // bind channel
	if (bind)
	{
		// Bind packet
		packet_p2M[0]  = rx_tx_addr[2];
		packet_p2M[1]  = rx_tx_addr[1];
		packet_p2M[2]  = rx_tx_addr[0];
		packet_p2M[3]  = hopping_frequency[12]; // channel_code encodes pair of channels to transmit on
		packet_p2M[4]  = 0x18;
		packet_p2M[5]  = 0x29;
		packet_p2M[6]  = 0;
		packet_p2M[7]  = 0;
		packet_p2M[8]  = 0;
		packet_p2M[9]  = 0;
		packet_p2M[10] = 0;
		packet_p2M[11] = 0;
		packet_p2M[12] = 0;
	}
	else
	{
		// Regular packet
		// Each data packet is repeated 3 times on one channel, and 3 times on another channel
		// For arithmetic simplicity, channels are repeated in rf_channels array
		if (hopping_frequency_no == 0)
		{
			for (uint8_t i = 0; i < 6; i++)
			{
				uint16_t val=convert_channel_ppm(CH_AETR[i]);
				packet_p2M[i*2]   = val>>8;		//high byte of servo timing(1000-2000us)
				packet_p2M[i*2+1] = val&0xFF;	//low byte of servo timing(1000-2000us)
			}
		}
		rf_ch = hopping_frequency[hopping_frequency_no];
		packet_p2M[12] = hopping_frequency[hopping_frequency_no+6];	// end_bytes
		hopping_frequency_no++;
		if (hopping_frequency_no > 6) hopping_frequency_no = 0;
	}
	NRF24L01_WriteReg(NRF24L01_05_RF_CH, rf_ch);
	NRF24L01_FlushTx();
	NRF24L01_WritePayload(packet, ESKY_PAYLOAD_SIZE);
	packet_sent = 1;
	if (! rf_ch_num)
		NRF24L01_SetPower();	//Keep transmit power updated
}

uint16_t ESKY_callback()
{
	if(IS_BIND_DONE)
	{
		if (packet_sent && NRF24L01_packet_ack() != PKT_ACKED)
			return ESKY_PACKET_CHKTIME;
		ESKY_send_packet(0);
	}
	else
	{
		if (packet_sent && NRF24L01_packet_ack() != PKT_ACKED)
			return ESKY_PACKET_CHKTIME;
		ESKY_send_packet(1);
		if (--bind_counter_p2M == 0)
		{
			ESKY_set_data_address();
			BIND_DONE;
		}
	}
	return ESKY_PACKET_PERIOD;
}

uint16_t initESKY(void)
{
	bind_counter_p2M = ESKY_BIND_COUNT;
	rx_tx_addr[3] = 0xBB;
	ESKY_init(IS_BIND_IN_PROGRESS);
	ESKY_init2();
	return 50000;
}
