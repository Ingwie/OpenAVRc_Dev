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



#ifdef PROTO_HAS_CYRF6936
#include "../OpenAVRc.h"
#include "../spi.h"

uint8_t cyrfmfg_id[6];


void CYRF_WriteRegister(uint8_t address, uint8_t data)
{
    RF_CS_CYRF6936_ACTIVE();
    RF_SPI_xfer(0x80 | address);
    RF_SPI_xfer(data);
    RF_CS_CYRF6936_INACTIVE();
}

static void CYRF_WriteRegisterMulti(uint8_t address, const uint8_t data[], uint8_t length)
{
    RF_CS_CYRF6936_ACTIVE();
    RF_SPI_xfer(0x80 | address);
    for(uint8_t i = 0; i < length; i++) {
      RF_SPI_xfer(data[i]);
    }
    RF_CS_CYRF6936_INACTIVE();
}

static void CYRF_ReadRegisterMulti(uint8_t address, uint8_t data[], uint8_t length)
{
    RF_CS_CYRF6936_ACTIVE();
    RF_SPI_xfer(address);
    for(uint8_t i = 0; i < length; i++) {
      data[i] = RF_SPI_xfer(0);
    }
    RF_CS_CYRF6936_INACTIVE();
}

uint8_t CYRF_ReadRegister(uint8_t address)
{
    RF_CS_CYRF6936_ACTIVE();
    RF_SPI_xfer(address);
    uint8_t data = RF_SPI_xfer(0);
    RF_CS_CYRF6936_INACTIVE();
  return data;
}


uint8_t CYRF_Reset()
{
#ifdef CYRF_RST_HI
  CYRF_RST_HI; // Hardware reset
  _delay_us(100);
  CYRF_RST_LO;
  _delay_us(100);
#endif
  CYRF_WriteRegister(CYRF_1D_MODE_OVERRIDE, 0x01); // Software reset
  _delay_us(500);
  CYRF_WriteRegister(CYRF_0C_XTAL_CTRL, 0xC0);     // Enable XOUT as GPIO
  CYRF_WriteRegister(CYRF_0D_IO_CFG, 0x04);        // Enable PACTL as GPIO
  CYRF_SetTxRxMode(TXRX_OFF);
  // Verify the CYRF chip is responding.
  return (CYRF_ReadRegister(CYRF_10_FRAMING_CFG) == 0xA5);
}


void CYRF_GetMfgData(uint8_t data[])
{
#ifndef FORCE_CYRF_ID
  // Fuses power on.
  CYRF_WriteRegister(CYRF_25_MFG_ID, 0xFF);
  CYRF_ReadRegisterMulti(CYRF_25_MFG_ID, data, 6);
  // Fuses power off.
  CYRF_WriteRegister(CYRF_25_MFG_ID, 0x00);
#else
  memcpy(data,FORCE_CYRF_ID,6);
#endif
}


// ToDo Check.
void CYRF_SetTxRxMode(enum TXRX_State mode)
{
  if(mode == TXRX_OFF) {
    CYRF_WriteRegister(CYRF_0F_XACT_CFG, 0x24); // 4=IDLE, 8=Synth(TX), C=Synth(RX)
  } else {
    CYRF_WriteRegister(CYRF_0F_XACT_CFG, mode == TX_EN ? 0x28 : 0x2C); // 4=IDLE, 8=Synth(TX), C=Synth(RX)
  }

  // Set the post tx/rx state.
  if(mode == TX_EN) {
    CYRF_WriteRegister(CYRF_0E_GPIO_CTRL, 0x80); // XOUT=1, PACTL=0
  } else if(mode == RX_EN) {
    CYRF_WriteRegister(CYRF_0E_GPIO_CTRL, 0x20); // XOUT=0, PACTL=1
  } else {
    CYRF_WriteRegister(CYRF_0E_GPIO_CTRL, 0x00); // XOUT=0, PACTL=0
  }
}


void CYRF_ConfigRFChannel(uint8_t ch)
{
  CYRF_WriteRegister(CYRF_00_CHANNEL, ch);
}


void CYRF_SetPower(uint8_t power)
{
//#define NO_POWER_AMP
#if defined(NO_POWER_AMP)
  power =7;
#endif
// uint8_t val = CYRF_ReadRegister(CYRF_03_TX_CFG) & 0xF8; // Not seen via SPI trace on My Devo 7e.
  uint8_t val = CYRF_ReadRegister(CYRF_03_TX_CFG);
  CYRF_WriteRegister(CYRF_03_TX_CFG, (val & 0xF8) | (power & 0x07));
}


void CYRF_ConfigCRCSeed(uint16_t crc)
{
  CYRF_WriteRegister(CYRF_15_CRC_SEED_LSB, crc & 0xff);
  CYRF_WriteRegister(CYRF_16_CRC_SEED_MSB, crc >> 8);
}


void CYRF_ConfigSOPCode(uint8_t *sopcodes)
{
  // NOTE: This can also be implemented as:
  // for(uint32_t i = 0; i < 8; i++) WriteRegister(CYRF_22_SOP_CODE, sopcodes[i]);
  CYRF_WriteRegisterMulti(CYRF_22_SOP_CODE, sopcodes, 8);
}


void CYRF_ConfigDataCode(uint8_t *datacodes, uint8_t len)
{
  // NOTE: This can also be implemented as:
  // for(uint32_t i = 0; i < len; i++) WriteRegister(CYRF_23_DATA_CODE, datacodes[i]);
  CYRF_WriteRegisterMulti(CYRF_23_DATA_CODE, datacodes, len);
}


void CYRF_WritePreamble(uint32_t preamble)
{
    RF_CS_CYRF6936_ACTIVE();
    RF_SPI_xfer(0x80 | 0x24);
    RF_SPI_xfer(preamble & 0xff);
    RF_SPI_xfer((preamble >> 8) & 0xff);
    RF_SPI_xfer((preamble >> 16) & 0xff);
    RF_CS_CYRF6936_INACTIVE();
}


void CYRF_StartReceive()
{
  CYRF_WriteRegister(CYRF_05_RX_CTRL, 0x80);
}


void CYRF_ReadDataPacketLen(uint8_t dpbuffer[], uint8_t length)
{
  CYRF_ReadRegisterMulti(CYRF_21_RX_BUFFER, dpbuffer, length);
}


void CYRF_WriteDataPacketLen(const uint8_t dpbuffer[], uint8_t len)
{
  CYRF_WriteRegister(CYRF_01_TX_LENGTH, len);
  CYRF_WriteRegister(CYRF_02_TX_CTRL, 0x40); // Clear the transmit buffer.
  CYRF_WriteRegisterMulti(CYRF_20_TX_BUFFER, dpbuffer, len);
  CYRF_WriteRegister(CYRF_02_TX_CTRL, 0x80); // Start transmission.
  // CYRF_WriteRegister(CYRF_02_TX_CTRL, 0xBF); // Pascallanger
}


void CYRF_WriteDataPacket(const uint8_t dpbuffer[])
{
  CYRF_WriteDataPacketLen(dpbuffer, 16);
}


uint8_t CYRF_ReadRSSI(uint8_t dodummyread)
{
  if(dodummyread) {
    CYRF_ReadRegister(CYRF_13_RSSI);
  }
  uint8_t result = CYRF_ReadRegister(CYRF_13_RSSI);

  if(result & 0x80) {
    result = CYRF_ReadRegister(CYRF_13_RSSI);
  }
  return (result & 0x1F); // Pascallanger is different but whole function is commented out.
}


// NOTE: This routine will reset the CRC Seed
void CYRF_FindBestChannels(uint8_t *channels, uint8_t len, uint8_t minspace, uint8_t min, uint8_t max)
{
#define NUM_FREQ 80
#define FREQ_OFFSET 4
  uint8_t rssi[NUM_FREQ];

  if (min < FREQ_OFFSET) min = FREQ_OFFSET;
  if (max > NUM_FREQ) max = NUM_FREQ;

  memset(channels, 0, sizeof(uint8_t) * len);
  CYRF_ConfigCRCSeed(0x0000);
  CYRF_SetTxRxMode(RX_EN);
  // Wait for pre-amp to switch from send to receive
  _delay_ms(1);
  for(uint8_t i = 0; i < NUM_FREQ; i++) {
    CYRF_ConfigRFChannel(i);
    _delay_us(270); // Slow channel require 270usec for synthesizer to settle
    if(! (CYRF_ReadRegister(CYRF_05_RX_CTRL) & 0x80)) {
      CYRF_WriteRegister(CYRF_05_RX_CTRL, 0x80); // Prepare to receive
      _delay_us(15);
      CYRF_ReadRegister(CYRF_13_RSSI);//dummy read
      _delay_us(15); // The conversion can occur as often as once every 12us
    }
    rssi[i] = CYRF_ReadRegister(CYRF_13_RSSI) & 0x1F;
  }

  for (uint8_t i = 0; i < len; i++) {
    channels[i] = min;
    for (int8_t j = min; j < max; j++)
      if (rssi[j] < rssi[channels[i]])
        channels[i] = j;
    for (int8_t j = channels[i] - minspace; j < channels[i] + minspace; j++) {
      // Ensure we don't reuse any channels within minspace of the selected channel again
      if (j < 0 || j >= NUM_FREQ)
        continue;
      rssi[j] = 0xff;
    }
  }

  CYRF_WriteRegister(CYRF_29_RX_ABORT, 0x20); // Abort RX operation - Pascallanger addition.
  CYRF_SetTxRxMode(TX_EN);
  CYRF_WriteRegister(CYRF_29_RX_ABORT, 0x20); // Clear abort RX - Pascallanger addition.
}


static void CYRF_PROGMEM_Config_DEVO_J6PRO_sopcodes(uint8_t sopidx)
{
  static const uint8_t DEVO_J6PRO_sopcodes[][8] PROGMEM = {
    /* Note these are in order transmitted (LSB 1st) */
    {0x3C, 0x37, 0xCC, 0x91, 0xE2, 0xF8, 0xCC, 0x91},
    {0x9B, 0xC5, 0xA1, 0x0F, 0xAD, 0x39, 0xA2, 0x0F},
    {0xEF, 0x64, 0xB0, 0x2A, 0xD2, 0x8F, 0xB1, 0x2A},
    {0x66, 0xCD, 0x7C, 0x50, 0xDD, 0x26, 0x7C, 0x50},
    {0x5C, 0xE1, 0xF6, 0x44, 0xAD, 0x16, 0xF6, 0x44},
    {0x5A, 0xCC, 0xAE, 0x46, 0xB6, 0x31, 0xAE, 0x46},
    {0xA1, 0x78, 0xDC, 0x3C, 0x9E, 0x82, 0xDC, 0x3C},
    {0xB9, 0x8E, 0x19, 0x74, 0x6F, 0x65, 0x18, 0x74},
    {0xDF, 0xB1, 0xC0, 0x49, 0x62, 0xDF, 0xC1, 0x49},
    {0x97, 0xE5, 0x14, 0x72, 0x7F, 0x1A, 0x14, 0x72},
//#if defined(J6PRO_CYRF6936_INO)
    {0x82, 0xC7, 0x90, 0x36, 0x21, 0x03, 0xFF, 0x17},
    {0xE2, 0xF8, 0xCC, 0x91, 0x3C, 0x37, 0xCC, 0x91}, //Note: the '03' was '9E' in the Cypress recommended table
    {0xAD, 0x39, 0xA2, 0x0F, 0x9B, 0xC5, 0xA1, 0x0F}, //The following are the same as the 1st 8 above,
    {0xD2, 0x8F, 0xB1, 0x2A, 0xEF, 0x64, 0xB0, 0x2A}, //but with the upper and lower word swapped
    {0xDD, 0x26, 0x7C, 0x50, 0x66, 0xCD, 0x7C, 0x50},
    {0xAD, 0x16, 0xF6, 0x44, 0x5C, 0xE1, 0xF6, 0x44},
    {0xB6, 0x31, 0xAE, 0x46, 0x5A, 0xCC, 0xAE, 0x46},
    {0x9E, 0x82, 0xDC, 0x3C, 0xA1, 0x78, 0xDC, 0x3C},
    {0x6F, 0x65, 0x18, 0x74, 0xB9, 0x8E, 0x19, 0x74},
//#endif
  };

  uint8_t code[8];
  uint_farptr_t pdata;
  pdata =  pgm_get_far_address(DEVO_J6PRO_sopcodes);

  for(uint8_t i=0; i<8; i++) {
//  code[i] = pgm_read_byte_far(&DEVO_J6PRO_sopcodes[sopidx][i]); // This works too.
    code[i] = pgm_read_byte_far(pdata +(sopidx *8) +i);
  }
  CYRF_ConfigSOPCode(code);
}


#endif // defined(PROTO_HAS_CYRF6936)
