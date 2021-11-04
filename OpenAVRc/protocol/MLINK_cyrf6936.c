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


#include "iface_cyrf6936.h"

//#define MLINK_FORCE_ID
#define MLINK_BIND_COUNT	696	// around 20s
#define MLINK_NUM_FREQ		78
#define MLINK_BIND_CHANNEL	0x01
#define MLINK_PACKET_SIZE	8


uint8_t  packet_count;
uint8_t  packetMlink[50];
uint8_t  hopping_frequency[78];
uint8_t  crc8;
uint8_t  crc8_polynomial;
uint8_t  phaseMlink;
uint8_t  hopping_frequency_no=0;
uint8_t  len;
uint16_t bind_counter;
uint8_t  protocol_flags=0;
uint8_t  rx_tx_addr[5];
uint32_t MProtocol_id;//tx id,
uint8_t  RX_num;

//Channel MIN MAX values
#define CHANNEL_MAX_100	1844	//	+100%
#define CHANNEL_MIN_100	204		//	-100%

//Bind flag
#define BIND_IN_PROGRESS	protocol_flags &= ~_BV(7)
#define BIND_DONE			protocol_flags |= _BV(7)
#define IS_BIND_DONE		( ( protocol_flags & _BV(7) ) !=0 )
#define IS_BIND_IN_PROGRESS	( ( protocol_flags & _BV(7) ) ==0 )

#define TELEMETRY_BUFFER_SIZE 32
uint8_t packet_in[TELEMETRY_BUFFER_SIZE];//telemetry receiving packets

const static RfOptionSettingsvar_t RfOpt_MLINK_Ser[] PROGMEM = {
/*rfProtoNeed*/PROTO_NEED_SPI | BOOL1USED, //can be PROTO_NEED_SPI | BOOL1USED | BOOL2USED | BOOL3USED
/*rfSubTypeMax*/0,
/*rfOptionValue1Min*/0,
/*rfOptionValue1Max*/0,
/*rfOptionValue2Min*/0,
/*rfOptionValue2Max*/0,
/*rfOptionValue3Max*/7
};


static void crc8_update(uint8_t byte)
{
	crc8 = crc8 ^ byte;
	for ( uint8_t j = 0; j < 8; j++ )
		if ( crc8 & 0x80 )
			crc8 = (crc8<<1) ^ crc8_polynomial;
		else
			crc8 <<= 1;
}

// Channel value -125%<->125% is scaled to 16bit value with no limit
int16_t convert_channel_16b_nolimit(uint8_t num, int16_t min, int16_t max, bool failsafe)
{
	int32_t val;
	#ifdef FAILSAFE_ENABLE
	if(failsafe)
		val=Failsafe_data[num];				// 0<->2047
	else
	#endif
		val=FULL_CHANNEL_OUTPUTS(num);				// 0<->2047
	val=(val-CHANNEL_MIN_100)*(max-min)/(CHANNEL_MAX_100-CHANNEL_MIN_100)+min;
	return (uint16_t)val;
}

// Convert 32b id to rx_tx_addr
static void set_rx_tx_addr(uint32_t id)
{ // Used by almost all protocols
	rx_tx_addr[0] = (id >> 24) & 0xFF;
	rx_tx_addr[1] = (id >> 16) & 0xFF;
	rx_tx_addr[2] = (id >>  8) & 0xFF;
	rx_tx_addr[3] = (id >>  0) & 0xFF;
	rx_tx_addr[4] = (rx_tx_addr[2]&0xF0)|(rx_tx_addr[3]&0x0F);
}

enum {
	MLINK_BIND_TX=0,
	MLINK_BIND_PREP_RX,
	MLINK_BIND_RX,
	MLINK_PREP_DATA,
	MLINK_SEND1,
	MLINK_SEND2,
	MLINK_SEND3,
	MLINK_CHECK3,
	MLINK_RX,
	MLINK_BUILD4,
};

uint8_t MLINK_Data_Code[16], MLINK_CRC_Init, MLINK_Unk_6_2;

const uint8_t PROGMEM MLINK_init_vals[][2] = {
	//Init from dump
	{ CYRF_01_TX_LENGTH,	0x08 },	// Length of packet
	{ CYRF_02_TX_CTRL,		0x40 },	// Clear TX Buffer
	{ CYRF_03_TX_CFG,		0x3C }, //0x3E in normal mode, 0x3C in bind mode: SDR 64 chip codes (=8 bytes data code used)
	{ CYRF_05_RX_CTRL,		0x00 },
	{ CYRF_06_RX_CFG,		0x93 },	// AGC enabled, overwrite enable, valid flag enable
	{ CYRF_0B_PWR_CTRL,		0x00 },
	//{ CYRF_0C_XTAL_CTRL,	0x00 },	// Set to GPIO on reset
	//{ CYRF_0D_IO_CFG,		0x00 },	// Set to GPIO on reset
	//{ CYRF_0E_GPIO_CTRL,	0x00 }, // Set by the CYRF_SetTxRxMode function
	{ CYRF_0F_XACT_CFG,		0x04 },	// end state idle
	{ CYRF_10_FRAMING_CFG,	0x00 },	// SOP disabled
	{ CYRF_11_DATA32_THOLD,	0x05 }, // not used???
	{ CYRF_12_DATA64_THOLD,	0x0F }, // 64 Chip Data PN Code Correlator Threshold
	{ CYRF_14_EOP_CTRL,		0x05 }, // 5 consecutive noncorrelations symbol for EOP
	{ CYRF_15_CRC_SEED_LSB,	0x00 }, // not used???
	{ CYRF_16_CRC_SEED_MSB,	0x00 }, // not used???
	{ CYRF_1B_TX_OFFSET_LSB,0x00 },
	{ CYRF_1C_TX_OFFSET_MSB,0x00 },
	{ CYRF_1D_MODE_OVERRIDE,0x00 },
	{ CYRF_1E_RX_OVERRIDE,	0x14 },	// RX CRC16 is disabled and Force Receive Data Rate
	{ CYRF_1F_TX_OVERRIDE,	0x04 }, // TX CRC16 is disabled
	{ CYRF_26_XTAL_CFG,		0x08 },
	{ CYRF_29_RX_ABORT,		0x00 },
	{ CYRF_32_AUTO_CAL_TIME,0x3C },
	{ CYRF_35_AUTOCAL_OFFSET,0x14 },
	{ CYRF_39_ANALOG_CTRL,	0x03 },	// Receive invert and all slow
};

static void MLINK_cyrf_config()
{
	for(uint8_t i = 0; i < sizeof(MLINK_init_vals) / 2; i++)
		CYRF_WriteRegister(pgm_read_byte_near(&MLINK_init_vals[i][0]), pgm_read_byte_near(&MLINK_init_vals[i][1]));
	CYRF_WritePreamble(0x333304);
	CYRF_SetTxRxMode(TX_EN);
}

static void MLINK_send_bind_packet()
{
	uint8_t p_c=packet_count>>1;

	memset(packetMlink, p_c<0x16?0x00:0xFF, MLINK_PACKET_SIZE-1);
	packetMlink[0]=0x0F;	// bind
	packetMlink[1]=p_c;
	switch(p_c)
	{
		case 0x00:
			packetMlink[2]=0x40;			//unknown but seems constant
			packetMlink[4]=0x01;			//unknown but seems constant
			packetMlink[5]=0x03;			//unknown but seems constant
			packetMlink[6]=0xE3;			//unknown but seems constant
			break;
		case 0x05:
			packetMlink[6]=MLINK_CRC_Init;	//CRC init value
			break;
		case 0x06:
			packetMlink[2]=MLINK_Unk_6_2;	//unknown and different
			//Start of hopping frequencies
			for(uint8_t i=0;i<4;i++)
				packetMlink[i+3]=hopping_frequency[i];
			break;
		case 0x15:
			packetMlink[6]=0x51;			//unknown but seems constant
			break;
		case 0x16:
			packetMlink[2]=0x51;			//unknown but seems constant
			packetMlink[3]=0xEC;			//unknown but seems constant
			packetMlink[4]=0x05;			//unknown but seems constant
			break;
		case 0x1A:
			packetMlink[1]=0xFF;
			memset(&packetMlink[2],0x00,5);
			break;
	}
	if(p_c>=0x01 && p_c<=0x04)
	{//DATA_CODE
		uint8_t p_c_5=(p_c-1)*5;
		for(uint8_t i=0;i<5;i++)
			if(i+p_c_5<16)
				packetMlink[i+2]=MLINK_Data_Code[i+p_c_5];
	}
	else
		if(p_c>=0x07 && p_c<=0x15)
		{//Hopping frequencies
			uint8_t p_c_5=5*(p_c-6)-1;
			for(uint8_t i=0;i<5;i++)
				if(i+p_c_5<MLINK_NUM_FREQ)
					packetMlink[i+2]=hopping_frequency[i+p_c_5];
		}
		else
			if(p_c>0x19)
			{
				packetMlink[1]=0xFF;
				memset(&packetMlink[2], 0x00, MLINK_PACKET_SIZE-3);
			}

	//Calculate CRC
	crc8=0xFF;	// Init = 0xFF
	for(uint8_t i=0;i<MLINK_PACKET_SIZE-1;i++)
		crc8_update(bit_reverse(packetMlink[i]));
	packetMlink[7] = bit_reverse(crc8);			// CRC reflected out

	//Debug
	#if 0
		//debug("P(%02d):",p_c);
		for(uint8_t i=0;i<8;i++)
			//debug(" %02X",packetMlink[i]);
		//debugln("");
	#endif

	//Send packet
	CYRF_WriteDataPacketLen(packetMlink, MLINK_PACKET_SIZE);
}

static void MLINK_send_data_packet()
{
	static uint8_t tog=0;
	uint8_t start;

#ifdef FAILSAFE_ENABLE
	static uint8_t fs=0;
	if(IS_FAILSAFE_VALUES_on && phaseMlink==MLINK_SEND1)
	{
		fs=10;	// Original radio is sending 70 packets
		FAILSAFE_VALUES_off;
	}
	if(fs)
	{// Failsafe packets
		switch(phaseMlink)
		{
			case MLINK_SEND2:
				packetMlink[0]=0x06;
				start=17;
				break;
			case MLINK_SEND3:
				packetMlink[0]=0x84;
				start=5;
				fs--;
				break;
			default: //MLINK_SEND1:
				packetMlink[0]=0x05;
				start=11;
				break;
		}
		//Pack 6 channels per packet
		for(uint8_t i=0;i<6;i++)
		{
			uint8_t val=start<16 ? convert_channel_16b_nolimit(start,426 >> 4,3448 >> 4,true) : 0x00;
			start--;	// switch to next channel
			packetMlink[i+1]=val;
		}
	}
	else
#endif
	{// Normal packets
		if(hopping_frequency_no==0)
			tog=1;
		//Channels to be sent
		if(phaseMlink==MLINK_SEND1 || ((hopping_frequency_no%5==0) && (phaseMlink==MLINK_SEND2)))
		{
			if((hopping_frequency_no&1)==0)
				packetMlink[0] = 0x09;	//10,8,6
			else
				packetMlink[0] = 0x01;	//11,9,7
		}
		else
			if(phaseMlink==MLINK_SEND2)
			{
				if(tog)
					packetMlink[0] = 0x02;	//x,15,13
				else
					packetMlink[0] = 0x0A;	//x,14,12
				tog^=1;
			}
			else
			{//phaseMlink==MLINK_SEND3
				if((hopping_frequency_no&1)==0)
					packetMlink[0] = 0x88;	//4,2,0
				else
					packetMlink[0] = 0x80;	//5,3,1
			}

		//Start channel
		start=4+6*(packetMlink[0]&3);
		if((packetMlink[0]&0x08)==0)
			start++;

		//Channels 426..1937..3448
		for(uint8_t i=0;i<3;i++)
		{
			uint16_t val=start<16 ? convert_channel_16b_nolimit(start,426,3448,false) : 0x0000;
			start-=2;	// switch to next channel
			packetMlink[i*2+1]=val>>8;
			packetMlink[i*2+2]=val;
		}
	}

	//Calculate CRC
	crc8=bit_reverse(hopping_frequency_no + MLINK_CRC_Init);	// Init = relected freq index + offset
	for(uint8_t i=0;i<MLINK_PACKET_SIZE-1;i++)
		crc8_update(bit_reverse(packetMlink[i]));
	packetMlink[7] = bit_reverse(crc8);			// CRC reflected out

	//Send
	CYRF_WriteDataPacketLen(packetMlink, MLINK_PACKET_SIZE);

	//Debug
	#if 0
		//debug("P(%02d):",hopping_frequency_no);
		for(uint8_t i=0;i<8;i++)
			//debug(" %02X",packetMlink[i]);
		//debugln("");
	#endif
}

#ifdef MLINK_HUB_TELEMETRY
	static void MLINK_Send_Telemetry()
	{ // not sure how MLINK telemetry works, the 2 RXs I have are sending something completly different...
		telemetry_counter += 2;				// TX LQI counter
		telemetry_link = 1;

		if(packet_in[0]==0x13)
		{ // RX-9-DR : 13 1A C8 00 01 64 00
			uint8_t id;
			for(uint8_t i=1; i<5; i+=3)
			{//2 sensors per packet
				id=0x00;
				switch(packet_in[i]&0x0F)
				{
					case 1: //voltage
						if((packet_in[i]&0xF0) == 0x00)
							v_lipo1 = packet_in[i+1];		// Rx_Batt*20
						else
							v_lipo2 = packet_in[i+1];
						break;
					case 2: //current
						id = 0x28;
						break;
					case 3: //vario
						id = 0x30;
						break;
					case 5: //rpm
						id = 0x03;
						break;
					case 6: //temp
						id = 0x02;
						break;
					case 10: //lqi
						RX_RSSI=RX_LQI=packet_in[i+1]>>1;
						break;
				}
				#if defined HUB_TELEMETRY
					if(id)
					{
						uint16_t val=((packet_in[i+2]&0x80)<<8)|((packet_in[i+2]&0x7F)<<7)|(packet_in[i+1]>>1);	//remove the alarm LSB bit, move the sign bit to MSB
						frsky_send_user_frame(id, val, val>>8);
					}
				#endif
			}
		}
		else
			if(packet_in[0]==0x03)
			{ // RX-5 :    03 15 23 00 00 01 02
				//Incoming packet values
				RX_RSSI = packet_in[2]<<1;	// Looks to be the RX RSSI value
				RX_LQI  = packet_in[5];		// Looks to be connection lost
			}
			else
				RX_RSSI = TX_LQI;

		// Read TX RSSI
		TX_RSSI = CYRF_ReadRegister(CYRF_13_RSSI)&0x1F;

		if(telemetry_lost)
		{
			telemetry_lost = 0;
			packet_count = 50;
			telemetry_counter = 100;
		}
	}
#endif

#ifdef MLINK_FW_TELEMETRY
	static void MLINK_Send_Telemetry()
	{
		telemetry_counter += 2;				// TX LQI counter
		telemetry_link = 4;

		// Read TX RSSI
		TX_RSSI = CYRF_ReadRegister(CYRF_13_RSSI)&0x1F;

		if(telemetry_lost)
		{
			telemetry_lost = 0;
			packet_count = 50;
			telemetry_counter = 100;
		}
	}
#endif

uint16_t MLINK_callback()
{
	uint8_t status;
	uint16_t start;

	switch(phaseMlink)
	{
		case MLINK_BIND_RX:
			////debugln("RX");
			status=CYRF_ReadRegister(CYRF_05_RX_CTRL);
			if( (status&0x80) == 0 )
			{//Packet received
				len=CYRF_ReadRegister(CYRF_09_RX_COUNT);
				//debugln("L=%02X",len)
				if( len==8 )
				{
					CYRF_ReadDataPacketLen(packetMlink, len*2);
					//debug("RX=");
					for(uint8_t i=0;i<8;i++)
						//debug(" %02X",packetMlink[i*2]);
					//debugln("");
					//Check CRC
					crc8=0xFF;	// Init = 0xFF
					for(uint8_t i=0;i<MLINK_PACKET_SIZE-1;i++)
						crc8_update(bit_reverse(packetMlink[i<<1]));
					if(packetMlink[14] == bit_reverse(crc8))
					{// CRC is ok
						//debugln("CRC ok");
						if(packetMlink[0]==0x7F)
							packet_count=3;					// Start sending bind payload
						else if(packet_count > 0x19*2)
						{
							if(packetMlink[0] == 0x8F)
								packet_count++;
							else if(packetMlink[0] == 0x9F)
								packet_count=0x80;			// End bind
							else
								packet_count=0;				// Restart bind...
						}
					}
				}
			}
			else
				packet_count=0;
			CYRF_WriteRegister(CYRF_29_RX_ABORT, 0x20);		// Enable RX abort
			CYRF_WriteRegister(CYRF_0F_XACT_CFG, 0x24);		// Force end state
			CYRF_WriteRegister(CYRF_29_RX_ABORT, 0x00);		// Disable RX abort
			phaseMlink=MLINK_BIND_TX;							// Retry sending bind packet
			CYRF_SetTxRxMode(TX_EN);						// Transmit mode
			if(packet_count)
				return 18136;
		case MLINK_BIND_TX:
			if(--bind_counter==0 || packet_count>=0x1B*2)
			{ // Switch to normal mode
				BIND_DONE;
				phaseMlink=MLINK_PREP_DATA;
				return 22720;
			}
			MLINK_send_bind_packet();
			if(packet_count == 0 || packet_count > 0x19*2)
			{
				phaseMlink++;		// MLINK_BIND_PREP_RX
				return 4700;	// Original is 4900
			}
			packet_count++;
			if(packet_count&1)
				return 6000;
			return 22720;
		case MLINK_BIND_PREP_RX:
			/*start=micros();
			while ((uint16_t)((uint16_t)micros()-(uint16_t)start) < 200)	// Wait max 200µs for TX to finish
				if((CYRF_ReadRegister(CYRF_02_TX_CTRL) & 0x80) == 0x00)
					break;*/										// Packet transmission complete
			CYRF_SetTxRxMode(RX_EN);							// Receive mode
			CYRF_WriteRegister(CYRF_05_RX_CTRL, 0x82);			// Prepare to receive
			phaseMlink++;	//MLINK_BIND_RX
			if(packet_count > 0x19*2)
				return 28712;									// Give more time to the RX to confirm that the bind is ok...
			return 28712-4700;


		case MLINK_PREP_DATA:
			CYRF_ConfigDataCode(MLINK_Data_Code,16);
			MLINK_CRC_Init += 0xED;
			hopping_frequency_no = 0x00;
			CYRF_ConfigRFChannel(hopping_frequency[hopping_frequency_no]);
			CYRF_SetPower(0x38);
			#if defined(MLINK_HUB_TELEMETRY) || defined(MLINK_FW_TELEMETRY)
				packet_count = 0;
				telemetry_lost = 1;
			#endif
			phaseMlink++;


		case MLINK_SEND1:
			MLINK_send_data_packet();
			phaseMlink++;
			return 4880+1111;
		case MLINK_SEND2:
			MLINK_send_data_packet();
			phaseMlink++;
			if(hopping_frequency_no%5==0)
				return 4617+1017;
			return 4617+1422;
		case MLINK_SEND3:
			MLINK_send_data_packet();
			phaseMlink++;
			return 4611;
		case MLINK_CHECK3:
			//Switch to next channel
			hopping_frequency_no++;
			if(hopping_frequency_no>=MLINK_NUM_FREQ)
				hopping_frequency_no=0;
			CYRF_ConfigRFChannel(hopping_frequency[hopping_frequency_no]);

			//Receive telemetry
			if(hopping_frequency_no%5==0)
			{//Receive telemetry
				CYRF_SetTxRxMode(RX_EN);							// Receive mode
				CYRF_WriteRegister(CYRF_05_RX_CTRL, 0x82);			// Prepare to receive
				phaseMlink++;	//MLINK_RX
				return 8038+2434+410-1000;
			}
			else
				CYRF_SetPower(0x38);
			phaseMlink=MLINK_SEND1;
			return 4470;
		case MLINK_RX:
			#if defined(MLINK_HUB_TELEMETRY) || defined(MLINK_FW_TELEMETRY)
				//TX LQI calculation
				packet_count++;
				if(packet_count>=50)
				{
					packet_count=0;
					TX_LQI=telemetry_counter;
					if(telemetry_counter==0)
						telemetry_lost = 1;
					telemetry_counter = 0;
				}
			#endif
			status=CYRF_ReadRegister(CYRF_05_RX_CTRL);
			//debug("T(%02X):",status);
			if( (status&0x80) == 0 )
			{//Packet received
				len=CYRF_ReadRegister(CYRF_09_RX_COUNT);
				//debug("(%X)",len)
				if( len && len <= MLINK_PACKET_SIZE )
				{
					CYRF_ReadDataPacketLen(packet_in, len*2);
					#if defined(MLINK_HUB_TELEMETRY) || defined(MLINK_FW_TELEMETRY)
						if(len==MLINK_PACKET_SIZE)
						{
							for(uint8_t i=0;i<8;i++)
							//Check CRC
							crc8=bit_reverse(MLINK_CRC_Init);
							for(uint8_t i=0;i<MLINK_PACKET_SIZE-1;i++)
							{
								packet_in[i]=packet_in[i<<1];
								crc8_update(bit_reverse(packet_in[i]));
								//debug(" %02X",packet_in[i]);
							}
							if(packet_in[14] == bit_reverse(crc8))	// Packet CRC is ok
								MLINK_Send_Telemetry();
							else
								//debug(" NOK");
						}
					#endif
				}
			}
			//debugln("");
			CYRF_WriteRegister(CYRF_29_RX_ABORT, 0x20);		// Enable RX abort
			CYRF_WriteRegister(CYRF_0F_XACT_CFG, 0x24);		// Force end state
			CYRF_WriteRegister(CYRF_29_RX_ABORT, 0x00);		// Disable RX abort
			CYRF_SetTxRxMode(TX_EN);						// Transmit mode
			phaseMlink=MLINK_SEND2;
			return 1000;
	}
	return 1000;
}

static void MLINK_shuffle_freqs(uint32_t seed, uint8_t *hop)
{
	srandom((uint32_t)seed);//randomSeed(seed);
/*
	for(uint8_t i=0; i < MLINK_NUM_FREQ/2; i++)
	{
		uint8_t r   = random(0xfefefefe) % (MLINK_NUM_FREQ/2);
		uint8_t tmp = hop[r];
		hop[r] = hop[i];
		hop[i] = tmp;
	}
	*/
}

void MLINK_init()
{
	MLINK_cyrf_config();

	//Init ID and RF freqs
	for(uint8_t i=0; i < MLINK_NUM_FREQ/2; i++)
	{
		hopping_frequency[i                 ] = (i<<1) + 3;
		hopping_frequency[i+MLINK_NUM_FREQ/2] = (i<<1) + 3;
	}
	// part1
	memcpy(MLINK_Data_Code  ,rx_tx_addr,4);
	MLINK_shuffle_freqs(MProtocol_id, hopping_frequency);

	// part2
	MProtocol_id ^= 0x6FBE3201;
	set_rx_tx_addr(MProtocol_id);
	memcpy(MLINK_Data_Code+4,rx_tx_addr,4);
	MLINK_shuffle_freqs(MProtocol_id, &hopping_frequency[MLINK_NUM_FREQ/2]);

	// part3
	MLINK_CRC_Init = rx_tx_addr[3];		//value sent during bind then used to init the CRC
	MLINK_Unk_6_2  = 0x3A;		//unknown value sent during bind but doesn't seem to matter

	#ifdef MLINK_FORCE_ID
		if(RX_num)
		{
			//Cockpit SX
			memcpy(MLINK_Data_Code,"\x4C\x97\x9D\xBF\xB8\x3D\xB5\xBE",8);
			memcpy(hopping_frequency,"\x0D\x41\x09\x43\x17\x2D\x05\x31\x13\x3B\x1B\x3D\x0B\x41\x11\x45\x09\x2B\x17\x4D\x19\x3F\x03\x3F\x0F\x37\x1F\x47\x1B\x49\x07\x35\x27\x2F\x15\x33\x23\x39\x1F\x33\x19\x45\x0D\x2D\x11\x35\x0B\x47\x25\x3D\x21\x37\x1D\x3B\x05\x2F\x21\x39\x23\x4B\x03\x31\x25\x29\x07\x4F\x1D\x4B\x15\x4D\x13\x4F\x0F\x49\x29\x2B\x27\x43",MLINK_NUM_FREQ);
			MLINK_Unk_6_2  = 0x3A;		//unknown value sent during bind but doesn't seem to matter
			MLINK_CRC_Init = 0x07;		//value sent during bind then used to init the CRC
		}
		else
		{
			//HFM3
			memcpy(MLINK_Data_Code,"\xC0\x90\x8F\xBB\x7C\x8E\x2B\x8E",8);
			memcpy(hopping_frequency,"\x05\x41\x27\x4B\x17\x33\x11\x39\x0F\x3F\x05\x2F\x13\x2D\x25\x31\x1F\x2D\x25\x35\x03\x41\x1B\x43\x09\x3D\x1F\x29\x1D\x35\x0D\x3B\x19\x49\x23\x3B\x17\x47\x1D\x2B\x13\x37\x0B\x31\x23\x33\x29\x3F\x07\x37\x07\x43\x11\x2B\x1B\x39\x0B\x4B\x03\x4F\x21\x47\x0F\x4D\x15\x45\x21\x4F\x09\x3D\x19\x2F\x15\x45\x0D\x49\x27\x4D",MLINK_NUM_FREQ);
			MLINK_Unk_6_2  = 0x02;		//unknown value but doesn't seem to matter
			MLINK_CRC_Init = 0x3E;		//value sent during bind then used to init the CRC
		}
		//Other TX
		//MLINK_Unk_6_2  = 0x7e;		//unknown value but doesn't seem to matter
		//MLINK_CRC_Init = 0xA2;		//value sent during bind then used to init the CRC
	#endif

	for(uint8_t i=0;i<8;i++)
		MLINK_Data_Code[i+8]=MLINK_Data_Code[7-i];

	//debug("ID:")
	for(uint8_t i=0;i<16;i++)
		//debug(" %02X", MLINK_Data_Code[i]);
	//debugln("");

	//debugln("CRC init: %02X", MLINK_CRC_Init)

	//debug("RF:")
	for(uint8_t i=0;i<MLINK_NUM_FREQ;i++)
		//debug(" %02X", hopping_frequency[i]);
	//debugln("");

	if(IS_BIND_IN_PROGRESS)
	{
		packet_count = 0;
		bind_counter = MLINK_BIND_COUNT;
		CYRF_ConfigDataCode((uint8_t*)"\x6F\xBE\x32\x01\xDB\xF1\x2B\x01\xE3\x5C\xFA\x02\x97\x93\xF9\x02",16); //Bind data code
		CYRF_ConfigRFChannel(MLINK_BIND_CHANNEL);
		phaseMlink = MLINK_BIND_TX;
	}
	else
		phaseMlink = MLINK_PREP_DATA;
}

static uint16_t MLINK_bind_cb()
{
  SCHEDULE_MIXER_END_IN_US(18000); // Schedule next Mixer calculations.
  MLINK_send_bind_packet();
  heartbeat |= HEART_TIMER_PULSES;
  CALCULATE_LAT_JIT(); // Calculate latency and jitter.
  return 18000U *2;
}

static uint16_t MLINK_cb()
{
  SCHEDULE_MIXER_END_IN_US(12000); // Schedule next Mixer calculations.
  MLINK_send_data_packet();
  heartbeat |= HEART_TIMER_PULSES;
  CALCULATE_LAT_JIT(); // Calculate latency and jitter.
  return 12000U *2;
}

static void MLINK_initialize(uint8_t bind)
{
  MLINK_init();
  phaseMlink = bind;
  if (bind) {
  PROTO_Start_Callback( MLINK_bind_cb);
  } else {
  PROTO_Start_Callback( MLINK_cb);
  }
}

const void *MLINK_Cmds(enum ProtoCmds cmd)
{
  switch(cmd)
    {
    case PROTOCMD_INIT:
      MLINK_initialize(0);
      return 0;
    case PROTOCMD_RESET:
      PROTO_Stop_Callback();
      CYRF_Reset();
      CYRF_SetTxRxMode(TXRX_OFF);
      return 0;
    case PROTOCMD_BIND:
      MLINK_initialize(1);
      return 0;
    case PROTOCMD_GETOPTIONS:
      SetRfOptionSettings(pgm_get_far_address(RfOpt_MLINK_Ser),
                          STR_DUMMY,//Sub proto
                          STR_DUMMY,         //Num channels STR_CH
                          STR_DUMMY,         //Option 2 (int)
                          STR_RFPOWER,       //Rf power
                          STR_TELEMETRY,      //OptionBool 1
                          STR_DUMMY,         //OptionBool 2
                          STR_DUMMY          //OptionBool 3
                         );
      return 0;
    default:
      break;
    }
  return 0;
}
