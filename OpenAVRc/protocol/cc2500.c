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



#ifdef PROTO_HAS_CC2500
#include "../OpenAVRc.h"
#include "../spi.h"
#if defined(SIMU)
#include <math.h>
#endif

void CC2500_WriteReg(uint8_t address, uint8_t data)
{
  RF_CS_CC2500_ACTIVE();
  RF_SPI_xfer(address);
  RF_SPI_xfer(data);
  RF_CS_CC2500_INACTIVE();
}

static void CC2500_ReadRegisterMulti(uint8_t address, uint8_t data[], uint8_t length)
{
  RF_CS_CC2500_ACTIVE();
  RF_SPI_xfer(address);
  for(uint8_t i = 0; i < length; i++)
    {
      data[i] = RF_SPI_xfer(0);
    }
  RF_CS_CC2500_INACTIVE();
}

uint8_t CC2500_ReadReg(uint8_t address)
{
  RF_CS_CC2500_ACTIVE();
  RF_SPI_xfer(CC2500_READ_SINGLE | address);
  uint8_t data = RF_SPI_xfer(0);
  RF_CS_CC2500_INACTIVE();
  return data;
}

void CC2500_ReadData(uint8_t *dpbuffer, uint8_t len)
{
  CC2500_ReadRegisterMulti(CC2500_3F_RXFIFO | CC2500_READ_BURST, dpbuffer, len);
}

uint8_t CC2500_Strobe(uint8_t strobe_cmd)
{
  RF_CS_CC2500_ACTIVE();
  uint8_t chip_status = RF_SPI_xfer(strobe_cmd);
  RF_CS_CC2500_INACTIVE();
  return chip_status;
}


void CC2500_WriteRegisterMulti(uint8_t address, const uint8_t data[], uint8_t length)
{
  RF_CS_CC2500_ACTIVE();
  RF_SPI_xfer(CC2500_WRITE_BURST | address);
  for(uint8_t i = 0; i < length; i++)
    {
      RF_SPI_xfer(data[i]);
    }
  RF_CS_CC2500_INACTIVE();
}

void CC2500_WriteData(uint8_t *dpbuffer, uint8_t len)
{
  CC2500_Strobe(CC2500_SFTX); // Flush Tx FIFO.
  CC2500_WriteRegisterMulti(CC2500_3F_TXFIFO, dpbuffer, len);
  CC2500_Strobe(CC2500_STX); //TX
}


void CC2500_SetTxRxMode(enum TXRX_State mode)
{
  /* The Skyartec CC2500 Module uses a RF Axis RFX2402E Power Amplifier and Low Noise Amplifier PA/LNA.
   * Deviation uses the CC2500 GDO0 and GDO2 to control the PA / LNA. GDO0 -> PA(TX Enable), GDO2 -> LNA(RX Enable).
   * Both the PA & LNA are positive logic.
   * Just to confuse things, GDO0 is accessed via register IOCFG0 @ 0x02, GDO2 via IOCFG2 @ 0x00 !!!.
   * It is best to disable the PA before enabling the LNA and vice versa, although the RFX2402E does not care
   * as it has an antenna switch.
   */

#define PA_ENABLE()    CC2500_WriteReg(CC2500_02_IOCFG0, 0x2F | 0x40);//set
#define LNA_ENABLE()   CC2500_WriteReg(CC2500_00_IOCFG2, 0x2F | 0x40);//set
#define PA_DISABLE()   CC2500_WriteReg(CC2500_02_IOCFG0, 0x2F);//clear
#define LNA_DISABLE()  CC2500_WriteReg(CC2500_00_IOCFG2, 0x2F);//clear

  if(mode == TX_EN)
    {
      LNA_DISABLE();
      PA_ENABLE();
    }
  else if (mode == RX_EN)
    {
      PA_DISABLE();
      LNA_ENABLE();
    }
  else
    {
      PA_DISABLE();
      LNA_DISABLE();
    }
}

void CC2500_Reset()
{
  CC2500_Strobe(CC2500_SRES);
  _delay_us(500);
  CC2500_SetTxRxMode(TXRX_OFF);
  CC2500_Strobe(CC2500_SIDLE);
}

#define CC2500_POWER_CHOICE  7

void CC2500_SetPower(uint8_t CCPower)
{
  /*
    CC2500 Power Output

    P(dBm) = 10Xlog10( P(mW) / 1mW)
    P(mW) = 1mWX10^(P(dBm)/ 10)

  */

// Power Amp (RDA T212).

#if (CC2500PA_GAIN == 20)
  const static uint16_t CC2500_Powers[] PROGMEM = {0,100,1000,1585,3981,6310,10000,12589};
#endif
#if (CC2500PA_GAIN == 22)
  const static uint16_t CC2500_Powers[] PROGMEM = {0,158,1585,2512,6310,10000,15849,19953};
#endif

  uint8_t cc2500_patable = 0;

  switch(CCPower)
    {
    case 0:
      cc2500_patable = 0x00;
      break; //
    case 1:
      cc2500_patable = 0x46;
      break; //
    case 2:
      cc2500_patable = 0x97;
      break; //
    case 3:
      cc2500_patable = 0x6E;
      break; //
    case 4:
      cc2500_patable = 0xA9;
      break; //
    case 5:
      cc2500_patable = 0xBB;
      break; //
    case 6:
      cc2500_patable = 0xFE;
      break; //
    case 7:
      cc2500_patable = 0xFF;
      break; //
    default:
      cc2500_patable = 0x00;
      break;
    };
  uint_farptr_t powerdata = pgm_get_far_address(CC2500_Powers);
  RFPowerOut = pgm_read_word_far(powerdata + (2*CCPower)); // Gui value
  rf_power_mem = CCPower;
  CC2500_WriteReg(CC2500_3E_PATABLE, cc2500_patable);
}

void CC2500_ManagePower()
{
  if (rangeModeIsOn) rf_power = TXPOWER_1;
  else rf_power = g_model.rfOptionValue3;
  if (rf_power != rf_power_mem)
  {
    CC2500_SetPower(rf_power);
  }
}

void CC2500_ManageFreq()
{
  if (freq_fine_mem != g_model.rfOptionValue1)
  {
    freq_fine_mem = g_model.rfOptionValue1;
    CC2500_WriteReg(CC2500_0C_FSCTRL0, freq_fine_mem);
  }
}

#endif // PROTO_HAS_CC2500

