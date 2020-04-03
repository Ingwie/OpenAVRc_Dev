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



#ifdef PROTO_HAS_NRF24L01
#include "../OpenAVRc.h"
#include "../spi.h"
#if defined(SIMU)
  //#include <math.h>
#endif


uint8_t rf_setup;

void NRF24L01_Initialize()
{
  rf_setup = 0x0f;
}

uint8_t NRF24L01_WriteReg(uint8_t reg, uint8_t data)
{
  RF_CS_NRF24L01_ACTIVE();
  uint8_t res = RF_SPI_xfer(W_REGISTER | (REGISTER_MASK & reg));
  RF_SPI_xfer(data);
  RF_CS_NRF24L01_INACTIVE();
  return res;
}

uint8_t NRF24L01_WriteRegisterMulti(uint8_t reg, uint8_t data[], uint8_t length)
{
  RF_CS_NRF24L01_ACTIVE();
  uint8_t res = RF_SPI_xfer(W_REGISTER | ( REGISTER_MASK & reg));
  for (uint8_t i = 0; i < length; i++)
    {
      RF_SPI_xfer(data[i]);
    }
  RF_CS_NRF24L01_INACTIVE();
  return res;
}

uint8_t NRF24L01_WritePayload(uint8_t *data, uint8_t length)
{
  RF_CS_NRF24L01_ACTIVE();
  uint8_t res = RF_SPI_xfer(W_TX_PAYLOAD);
  for (uint8_t i = 0; i < length; i++)
    {
      RF_SPI_xfer(data[i]);
    }
  RF_CS_NRF24L01_INACTIVE();
  return res;
}

uint8_t NRF24L01_ReadReg(uint8_t reg)
{
  RF_CS_NRF24L01_ACTIVE();
  RF_SPI_xfer(R_REGISTER | (REGISTER_MASK & reg));
  uint8_t data = RF_SPI_xfer(0);
  RF_CS_NRF24L01_INACTIVE();
  return data;
}

uint8_t NRF24L01_ReadRegisterMulti(uint8_t reg, uint8_t data[], uint8_t length)
{
  RF_CS_NRF24L01_ACTIVE();
  uint8_t res = RF_SPI_xfer(R_REGISTER | (REGISTER_MASK & reg));
  for(uint8_t i = 0; i < length; i++)
    {
      data[i] = RF_SPI_xfer(0);
    }
  RF_CS_NRF24L01_INACTIVE();
  return res;
}

uint8_t NRF24L01_ReadPayload(uint8_t *data, uint8_t length)
{
  RF_CS_NRF24L01_ACTIVE();
  uint8_t res = RF_SPI_xfer(R_RX_PAYLOAD);
  for(uint8_t i = 0; i < length; i++)
    {
      data[i] = RF_SPI_xfer(0);
    }
  RF_CS_NRF24L01_INACTIVE();
  return res;
}

static uint8_t Strobe(uint8_t state)
{
  RF_CS_NRF24L01_ACTIVE();
  uint8_t res = RF_SPI_xfer(state);
  RF_CS_NRF24L01_INACTIVE();
  return res;
}

uint8_t NRF24L01_FlushTx()
{
  return Strobe(FLUSH_TX);
}

uint8_t NRF24L01_FlushRx()
{
  return Strobe(FLUSH_RX);
}

uint8_t NRF24L01_NOP()
{
  return Strobe(NOP);
}

uint8_t NRF24L01_Activate(uint8_t code)
{
  RF_CS_NRF24L01_ACTIVE();
  uint8_t res = RF_SPI_xfer(ACTIVATE);
  RF_SPI_xfer(code);
  RF_CS_NRF24L01_INACTIVE();
  return res;
}

uint8_t NRF24L01_SetBitrate(uint8_t bitrate)
{

  uint8_t temp = NRF24L01_ReadReg(NRF24L01_06_RF_SETUP);
  temp = (temp & 0xF7) | ((bitrate & 0x01) << 3);
  return NRF24L01_WriteReg(NRF24L01_06_RF_SETUP, temp);
}



uint8_t NRF24L01_SetPower(uint8_t power)
{
  /*
  // nRF24L01+ Power Output
       Raw       * 20dBm PA
  0 : -18dBm    2dBm (1.6mW)
  1 : -12dBm    8dBm   (6mW)
  2 :  -6dBm   14dBm  (25mW)
  3 :   0dBm   20dBm (100mW)
  */
#if (NRF24L01PA_GAIN == 20)
  const static uint16_t NRF24L01_Powers[] PROGMEM = {158,630,2511,10000};
#endif
#if (NRF24L01_GAIN == 22)
  const static uint16_t NRF24L01_Powers[] PROGMEM = {251,1000,3981,15848};
#endif

  uint8_t nrf_power = 0;

  switch(power)
    {
    case 0:
    case 1:
      nrf_power = 0;
      break; //
    case 2:
    case 3:
      nrf_power = 1;
      break; //
    case 4:
    case 5:
      nrf_power = 2;
      break; //
    case 6:
    case 7:
      nrf_power = 3;
      break; //
    default:
      nrf_power = 0;
      break;
    };
  // Power is in range 0..3 for nRF24L01

  uint_farptr_t powerdata = pgm_get_far_address(NRF24L01_Powers);
  RFPowerOut = pgm_read_word_far(powerdata + (2*nrf_power)); // Gui value
  rf_power_mem = nrf_power;

  uint8_t temp = NRF24L01_ReadReg(NRF24L01_06_RF_SETUP);
  temp = (temp & 0xF9) | ((nrf_power & 0x03) << 1);
  return NRF24L01_WriteReg(NRF24L01_06_RF_SETUP, temp);
}

void NRF24L01_SetTxRxMode(enum TXRX_State mode) // TODO : Watch CE pin
{
  if(mode == TX_EN)
    {
      NRF24L01_WriteReg(NRF24L01_07_STATUS, (1 << NRF24L01_07_RX_DR)    //reset the flag(s)
                        | (1 << NRF24L01_07_TX_DS)
                        | (1 << NRF24L01_07_MAX_RT));
      NRF24L01_WriteReg(NRF24L01_00_CONFIG, (1 << NRF24L01_00_EN_CRC)   // switch to TX mode
                        | (1 << NRF24L01_00_CRCO)
                        | (1 << NRF24L01_00_PWR_UP));
      _delay_us(130);
    }
  else if (mode == RX_EN)
    {
      NRF24L01_WriteReg(NRF24L01_07_STATUS, 0x70);        // reset the flag(s)
      NRF24L01_WriteReg(NRF24L01_00_CONFIG, 0x0F);        // switch to RX mode
      NRF24L01_WriteReg(NRF24L01_07_STATUS, (1 << NRF24L01_07_RX_DR)    //reset the flag(s)
                        | (1 << NRF24L01_07_TX_DS)
                        | (1 << NRF24L01_07_MAX_RT));
      NRF24L01_WriteReg(NRF24L01_00_CONFIG, (1 << NRF24L01_00_EN_CRC)   // switch to RX mode
                        | (1 << NRF24L01_00_CRCO)
                        | (1 << NRF24L01_00_PWR_UP)
                        | (1 << NRF24L01_00_PRIM_RX));
      _delay_us(130);
    }
  else
    {
      NRF24L01_WriteReg(NRF24L01_00_CONFIG, (1 << NRF24L01_00_EN_CRC)); //PowerDown
    }
}

int16_t NRF24L01_Reset()
{
  NRF24L01_FlushTx();
  NRF24L01_FlushRx();
  uint8_t status1 = Strobe(NOP);
  uint8_t status2 = NRF24L01_ReadReg(0x07);
  NRF24L01_SetTxRxMode(TXRX_OFF);
  return (status1 == status2 && (status1 & 0x0f) == 0x0e);
}

#endif // PROTO_HAS_NRF24L01

