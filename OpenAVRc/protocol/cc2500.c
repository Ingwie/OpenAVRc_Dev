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

void CC2500_WriteReg(uint8_t address, uint8_t data)
{
  NONATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    RF_CS_CC2500_ACTIVE();
    RF_SPI_xfer(address);
    RF_SPI_xfer(data);
    RF_CS_CC2500_INACTIVE();
  }
}

static void CC2500_ReadRegisterMulti(uint8_t address, uint8_t data[], uint8_t length)
{
  NONATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    RF_CS_CC2500_ACTIVE();
    RF_SPI_xfer(address);
    for(uint8_t i = 0; i < length; i++) {
      data[i] = RF_SPI_xfer(0);
    }
    RF_CS_CC2500_INACTIVE();
  }
}

uint8_t CC2500_ReadReg(uint8_t address)
{
  uint8_t data;
  NONATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    RF_CS_CC2500_ACTIVE();
    RF_SPI_xfer(CC2500_READ_SINGLE | address);
    data = RF_SPI_xfer(0);
    RF_CS_CC2500_INACTIVE();
  }
  return data;
}

void CC2500_ReadData(uint8_t *dpbuffer, uint8_t len)
{
  CC2500_ReadRegisterMulti(CC2500_3F_RXFIFO | CC2500_READ_BURST, dpbuffer, len);
}

uint8_t CC2500_Strobe(uint8_t strobe_cmd)
{
  uint8_t chip_status;
  NONATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    RF_CS_CC2500_ACTIVE();
    chip_status = RF_SPI_xfer(strobe_cmd);
    RF_CS_CC2500_INACTIVE();
  }
  return chip_status;
}


void CC2500_WriteRegisterMulti(uint8_t address, const uint8_t data[], uint8_t length)
{
  NONATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    RF_CS_CC2500_ACTIVE();
  RF_SPI_xfer(CC2500_WRITE_BURST | address);
  for(uint8_t i = 0; i < length; i++) {
    RF_SPI_xfer(data[i]);
  }
  RF_CS_CC2500_INACTIVE();
  }
}

void CC2500_WriteData(uint8_t *dpbuffer, uint8_t len)
{
//  CC2500_Strobe(CC2500_SFTX);
  CC2500_WriteRegisterMulti(CC2500_3F_TXFIFO, dpbuffer, len);
//  CC2500_Strobe(CC2500_STX);
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

  if(mode == TX_EN) {
    LNA_DISABLE();
    PA_ENABLE();
  } else if (mode == RX_EN) {
    PA_DISABLE();
    LNA_ENABLE();
  } else {
    PA_DISABLE();
    LNA_DISABLE();
  }
}

void CC2500_Reset(void)
{
  CC2500_Strobe(CC2500_SRES);
  _delay_us(100); // Should be > 50us. see datasheet.
}


void CC2500_SetPower(uint8_t Power)
{
  /*
    CC2500 Power Output

    <-55 dBm == 0x00
    -20 dBm	== 0x46
    -16 dBm	== 0x55
    -12 dBm	== 0xC6
    -8 dBm	== 0x6E
    -4 dBm  == 0xA9
    +0 dBm	== 0xFE
    +1 dBm	== 0xFF
  */

// Power Amp (RDA T212).

  uint8_t cc2500_patable = 0;

  switch(Power) {
  case 0:
    cc2500_patable = 0;
    break; //
  case 1:
    cc2500_patable = 0x46;
    break; //
  case 2:
    cc2500_patable = 0x55;
    break; //
  case 3:
    cc2500_patable = 0xc6;
    break; //
  case 4:
    cc2500_patable = 0x6e;
    break; //
  case 5:
    cc2500_patable = 0xa9;
    break; //
  case 6:
    cc2500_patable = 0xfe;
    break; //
  case 7:
    cc2500_patable = 0xff;
    break; //
  default:
    cc2500_patable = 0;
    break;
  };

  CC2500_WriteReg(CC2500_3E_PATABLE, cc2500_patable);
}

#endif // PROTO_HAS_CC2500

