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



#ifdef PROTO_HAS_A7105
#include "../OpenAVRc.h"

#define A7105_Enable_HWSPI() \
 SET_RF_MOSI_IS_OUTPUT();   \
 WAKEUP_RF_SPI();

#if defined(CPUM2560)
void SPI_WRITE_3WIRES(uint8_t data) // Bitbang write
{
 for (uint8_t mask=0x80; (mask); mask>>=1)
  {
   if(data & mask)
    {
     RF_MOSI_ON();
    }
   else
    {
     RF_MOSI_OFF();
    }
   RF_XCK_ON();
   //_NOP();
   RF_XCK_OFF();
  }
}

uint8_t SPI_READ_3WIRES() // Bitbang read
{
 uint8_t result=0;
 for (uint8_t mask=0x80; (mask); mask>>=1)
  {
   if(IS_RF_MOSI_ON)
    {
     result |= mask;
    }
   RF_XCK_ON();
   //_NOP();
   RF_XCK_OFF();
  }
 return result;
}
#endif

void A7105_Strobe(uint8_t address)
{
 RF_CS_A7105_ACTIVE();
 RF_SPI_xfer(address);
 RF_CS_A7105_INACTIVE();
}

void A7105_WriteReg(uint8_t address, uint8_t data)
{
 RF_CS_A7105_ACTIVE();
 RF_SPI_xfer(address);
 _NOP();
 RF_SPI_xfer(data);
 RF_CS_A7105_INACTIVE();
}

void A7105_SetTxRxMode(uint8_t mode)
{
 if(mode == TX_EN)
  {
   A7105_WriteReg(A7105_0B_GPIO1_PIN1, 0x33);
   A7105_WriteReg(A7105_0C_GPIO2_PIN_II, 0x31);
  }
 else if (mode == RX_EN)
  {
   A7105_WriteReg(A7105_0B_GPIO1_PIN1, 0x31);
   A7105_WriteReg(A7105_0C_GPIO2_PIN_II, 0x33);
  }
 else
  {
  // ToDo
    /* Power Amp / Low Noise Amp devices like Skyworks RFX2401C, RFX2402E have a shutdown state of
    * RXEN = 0, TXEN = 0. Others may not.
    */
    A7105_WriteReg(A7105_0B_GPIO1_PIN1, 0x33);   // GIO1 = 1.
    A7105_WriteReg(A7105_0C_GPIO2_PIN_II, 0x33); // GIO2 = 1.
  }
}

void A7105_WriteData(uint8_t len, uint8_t channel)
{
 RF_CS_A7105_ACTIVE();
 RF_SPI_xfer(A7105_RST_WRPTR);
 RF_SPI_xfer(A7105_05_FIFO_DATA);
 for (uint8_t i = 0; i < len; i++)
  RF_SPI_xfer(packet_p2M[i]);
 RF_CS_A7105_INACTIVE();
 if(g_model.rfProtocol != PROTOCOL_FLYSKY)
  {
   A7105_Strobe(A7105_STANDBY);	//Force standby mode, ie cancel any TX or RX...
   A7105_SetTxRxMode(TX_EN);		//Switch to PA
  }
 A7105_WriteReg(A7105_0F_PLL_I, channel);
 A7105_Strobe(A7105_TX);
}

void A7105_ReadData(uint8_t len)
{
 A7105_Strobe(A7105_RST_RDPTR);
#if defined(PCBMEGAMINI)
 SUSPEND_RF_SPI();
 RF_CS_A7105_ACTIVE();
 SPI_WRITE_3WIRES(0x40 | A7105_05_FIFO_DATA);	//bit 6 =1 for reading (bitbang mode)
 SET_RF_MOSI_IS_INPUT();
#else // other
 RF_CS_A7105_ACTIVE();
 RF_SPI_xfer(0x40 | A7105_05_FIFO_DATA);	//bit 6 =1 for reading
 SUSPEND_RF_SPI();
 SET_RF_MOSI_IS_INPUT();
#endif // MEGAMINI (uart2 MSPI mode)
 for (uint8_t i = 0; i < len; i++)
  packet_p2M[i]=SPI_READ_3WIRES();
 RF_CS_A7105_INACTIVE();
 A7105_Enable_HWSPI();
}

uint8_t A7105_ReadReg(uint8_t address)
{
 uint8_t result;
#if defined(PCBMEGAMINI)
 SUSPEND_RF_SPI();
 RF_CS_A7105_ACTIVE();
 SPI_WRITE_3WIRES(address |= 0x40);	//bit 6 =1 for reading (bitbang mode)
 SET_RF_MOSI_IS_INPUT();
#else // other
 RF_CS_A7105_ACTIVE();
 RF_SPI_xfer(address |= 0x40);				//bit 6 =1 for reading
 SUSPEND_RF_SPI();
 SET_RF_MOSI_IS_INPUT();
#endif // MEGAMINI (uart2 MSPI mode)
 result = SPI_READ_3WIRES();
 RF_CS_A7105_INACTIVE();
 A7105_Enable_HWSPI();
 return(result);
}

//------------------------
void A7105_Reset()
{
  A7105_Enable_HWSPI();
  A7105_WriteReg(A7105_00_MODE, 0x00); // Software reset.

  uint8_t temp = 10;
  while(temp--) {
    if(A7105_ReadReg(A7105_10_PLL_II) == 0x9E) break; // Check if register is reset to default.
    _delay_us(100);
  }

  A7105_SetTxRxMode(TXRX_OFF); // Set both GPIO states. Initialisation of registers ( 0x0B, 0x0C ) should not overwrite this.
  A7105_Strobe(A7105_STANDBY);
}

void A7105_WriteID(uint32_t ida)
{
 RF_CS_A7105_ACTIVE();
 RF_SPI_xfer(A7105_06_ID_DATA);
 RF_SPI_xfer((ida>>24)&0xff);
 RF_SPI_xfer((ida>>16)&0xff);
 RF_SPI_xfer((ida>>8)&0xff);
 RF_SPI_xfer((ida>>0)&0xff);
 RF_CS_A7105_INACTIVE();
}

static void A7105_SetPower(uint8_t Apower)
{
 /*
 -23.3dBm == PAC=0 TBG=0
 -19.6dBm == PAC=0 TBG=1
 -16.6dBm == PAC=0 TBG=2
 -10.4dBm == PAC=1 TBG=3
 -6.3dBm  == PAC=1 TBG=5
 -2.0dBm  == PAC=0 TBG=7
 0.1dBm  == PAC=2 TBG=7
 1.3dBm  == PAC=3 TBG=7
 */

#if (A7105PA_GAIN == 20)
 const static uint16_t zzA7105_Powers[] PROGMEM = {47,110,219,912,2344,6309,10233,13490};
#endif
#if (A7105PA_GAIN == 22)
 const static uint16_t zzA7105_Powers[] PROGMEM = {74,174,347,1445,3715,10000,16218,21380};
#endif
 uint8_t pac, tbg;

 switch(Apower)
  {
  case 0:
   pac = 0;
   tbg = 0;
   break;
  case 1:
   pac = 0;
   tbg = 1;
   break;
  case 2:
   pac = 0;
   tbg = 2;
   break;
  case 3:
   pac = 1;
   tbg = 3;
   break;
  case 4:
   pac = 1;
   tbg = 5;
   break;
  case 5:
   pac = 0;
   tbg = 7;
   break;
  case 6:
   pac = 2;
   tbg = 7;
   break;
  case 7:
   pac = 3;
   tbg = 7;
   break;
  default:
   pac = 0;
   tbg = 0;
   break;
  };

 RFPowerOut = pgm_read_word_far(pgm_get_far_address(zzA7105_Powers) + (2*Apower)); // Gui value
 RF_POWER_MEM_P2M = Apower;

 A7105_WriteReg(0x28, (pac << 3) | tbg);
}

void A7105_ManagePower()
{
 if (systemBolls.rangeModeIsOn)
  RF_POWER_P2M = TXPOWER_1;
 else
  RF_POWER_P2M = g_model.rfOptionValue3;
 if (RF_POWER_P2M != RF_POWER_MEM_P2M)
  {
   A7105_SetPower(RF_POWER_P2M);
  }
}

// Fine tune A7105 LO base frequency
// this is required for some A7105 modules and/or RXs with inaccurate crystal oscillator
void A7105_AdjustLOBaseFreq()
{
 if (FREQ_FINE_MEM_P2M != g_model.rfOptionValue1)
  {
   FREQ_FINE_MEM_P2M = g_model.rfOptionValue1;
   int16_t offset = (FREQ_FINE_MEM_P2M*236)/100; // 2.36Khz/step
   // LO base frequency = 32e6*(bip+(bfp/(2^16)))
   uint8_t bip;	// LO base frequency integer part
   uint16_t bfp;	// LO base frequency fractional part
   offset++;		// as per datasheet, not sure why recommended, but that's a +1kHz drift only ...
   offset<<=1;
   if(offset < 0)
    {
     bip = 0x4a;	// 2368 MHz
     bfp = 0xffff + offset;
    }
   else
    {
     bip = 0x4b;	// 2400 MHz (default)
     bfp = offset;
    }
   A7105_WriteReg( A7105_11_PLL_III, bip);
   A7105_WriteReg( A7105_12_PLL_IV, (bfp >> 8) & 0xff);
   A7105_WriteReg( A7105_13_PLL_V, bfp & 0xff);
  }
}


const uint8_t HUBSAN_A7105_regs[] PROGMEM =
{//0     1     2     3     4     5     6     7     8     9     a     b     c     d     e     f
  0xFF, 0x63, 0xFF, 0x0F, 0xFF, 0xFF, 0xFF ,0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x05, 0x04, 0xFF, // 0x
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x2B, 0xFF, 0xFF, 0x62, 0x80, 0xFF, 0xFF, 0x0A, 0xFF, 0xFF, 0x07, // 1x
  0x17, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x47, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, // 2x
  0xFF, 0xFF // 3x
};


const uint8_t FLYSKY_A7105_regs[] PROGMEM =
{//0     1     2     3     4     5     6     7     8     9     a     b     c     d     e     f
  0xff, 0x42, 0x00, 0x14, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x05, 0x00, 0x50,
  0x9e, 0x4b, 0x00, 0x02, 0x16, 0x2b, 0x12, 0x00, 0x62, 0x80, 0x80, 0x00, 0x0a, 0x32, 0xc3, 0x0f,
  0x13, 0xc3, 0x00, 0xff, 0x00, 0x00, 0x3b, 0x00, 0x17, 0x47, 0x80, 0x03, 0x01, 0x45, 0x18, 0x00,
  0x01, 0x0f
};


const uint8_t AFHDS2A_A7105_regs[] PROGMEM =
{
  0xFF, 0x42 | (1<<5), 0x00, 0x25, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x05, 0x00, 0x50,
  0x9e, 0x4b, 0x00, 0x02, 0x16, 0x2b, 0x12, 0x4f, 0x62, 0x80, 0xFF, 0xFF, 0x2a, 0x32, 0xc3, 0x1f,
  0x1e, 0xFF, 0x00, 0xFF, 0x00, 0x00, 0x3b, 0x00, 0x17, 0x47, 0x80, 0x03, 0x01, 0x45, 0x18, 0x00,
  0x01, 0x0f
};

#define ID_NORMAL 0x55201041
#define ID_PLUS   0xAA201041

void A7105_Init(void)
{
 A7105_Reset();
 uint_farptr_t A7105_Regs = 0;

 if(g_model.rfProtocol==PROTOCOL_HUBSAN)
 {
   A7105_WriteID(ID_NORMAL);
   A7105_Regs=pgm_get_far_address(HUBSAN_A7105_regs);
 }
 else
 {
  A7105_WriteID(0x5475C52A);//0x2Ac57554

  if(g_model.rfProtocol==PROTOCOL_FLYSKY)
   {
    A7105_Regs=pgm_get_far_address(FLYSKY_A7105_regs);
   }
  else if(g_model.rfProtocol==PROTOCOL_AFHDS2A)
   {
    A7105_Regs=pgm_get_far_address(AFHDS2A_A7105_regs);
   }
 }

 for (uint8_t i = 0; i < 0x32; i++)
  {
   uint8_t val = pgm_read_byte_far(A7105_Regs + i);
   if((g_model.rfProtocol==PROTOCOL_FLYSKY) && (g_model.rfSubType==4/*CX20*/))
    {
     if(i==0x0E)
      val=0x01;
     if(i==0x1F)
      val=0x1F;
     if(i==0x20)
      val=0x1E;
    }
   if( val != 0xFF)
    A7105_WriteReg(i, val);
  }

  // A7105 CALIBRATION.
  A7105_Strobe(A7105_PLL); // All calibration done in PLL mode, See Datasheet.

  // IF Filter Bank Calibration.
  A7105_WriteReg(A7105_02_CALC,1);

  uint8_t cal = 24;
  while(cal--) {
    if(! (A7105_ReadReg(A7105_02_CALC))) break; // Check if calibration complete.
   // Can take up to 4ms if register A7105_17_DELAY_II is set to 0 as Flysky protocol does.
    _delay_us(250);
  }

  //if(g_model.rfProtocol!=PROTOCOL_HUBSAN)
  {
    //VCO Current Calibration
    A7105_WriteReg(A7105_24_VCO_CURCAL,0x13);	// Recommended calibration from A7105 Datasheet
    //VCO Bank Calibration
    A7105_WriteReg(A7105_26_VCO_SBCAL_II,0x3b);	// Recommended calibration from A7105 Datasheet
  }


  // VCO Bank Calibrate channel 0.
  A7105_WriteReg(A7105_0F_CHANNEL, 0);

  A7105_WriteReg(A7105_02_CALC,2);
  cal = 4;
  while(cal--) {
    if(! (A7105_ReadReg(A7105_02_CALC))) break; // Check if calibration complete.
    _delay_us(250);
  }

  // VCO Bank Calibrate channel A0.
  A7105_WriteReg(A7105_0F_CHANNEL, 0xa0);

  A7105_WriteReg(A7105_02_CALC, 2);
  cal = 4;
  while(cal--) {
    if(! (A7105_ReadReg(A7105_02_CALC))) break; // Check if calibration complete.
    _delay_us(250);
  }

  // Reset VCO Band calibration
  //if(g_model.rfProtocol!=PROTOCOL_HUBSAN)
  {
    A7105_WriteReg(A7105_25_VCO_SBCAL_I,(g_model.rfProtocol==PROTOCOL_FLYSKY)?0x08:0x0A);
  }

 A7105_SetTxRxMode(TX_EN);
 A7105_SetPower(TXPOWER_1);
 A7105_AdjustLOBaseFreq();
}
#endif

