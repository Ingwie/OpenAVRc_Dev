//


#ifdef PROTO_HAS_CC2500
#include "../OpenAVRc.h"
#include "../spi.h"

void CC2500_WriteReg(uint8_t address, uint8_t data)
{
  RF_CS_N_ACTIVE();
  RF_SPI_xfer(address);
  RF_SPI_xfer(data);
  RF_CS_N_INACTIVE();
}

static void ReadRegisterMulti(uint8_t address, uint8_t data[], uint8_t length)
{
  RF_CS_N_ACTIVE();
  RF_SPI_xfer(address);
  for(uint8_t i = 0; i < length; i++) {
    data[i] = RF_SPI_xfer(0);
  }
  RF_CS_N_INACTIVE();
}

uint8_t CC2500_ReadReg(uint8_t address)
{
  RF_CS_N_ACTIVE();
  RF_SPI_xfer(CC2500_READ_SINGLE | address);
  uint8_t data = RF_SPI_xfer(0);
  RF_CS_N_INACTIVE();
  return data;
}

void CC2500_ReadData(uint8_t *dpbuffer, uint8_t len)
{
  ReadRegisterMulti(CC2500_3F_RXFIFO | CC2500_READ_BURST, dpbuffer, len);
}

uint8_t CC2500_Strobe(uint8_t strobe_cmd)
{
  RF_CS_N_ACTIVE();
  uint8_t chip_status = RF_SPI_xfer(strobe_cmd);
  RF_CS_N_INACTIVE();
  return chip_status;
}


void CC2500_WriteRegisterMulti(uint8_t address, const uint8_t data[], uint8_t length)
{
  RF_CS_N_ACTIVE();
  RF_SPI_xfer(CC2500_WRITE_BURST | address);
  for(uint8_t i = 0; i < length; i++) {
  RF_SPI_xfer(data[i]);
  }
  RF_CS_N_INACTIVE();
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
  }
  else if (mode == RX_EN) {
    PA_DISABLE();
    LNA_ENABLE();
  }
  else {
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
    case 0: cc2500_patable = 0; break; //
    case 1: cc2500_patable = 0x46; break; //
    case 2: cc2500_patable = 0x55; break; //
    case 3: cc2500_patable = 0xc6; break; //
    case 4: cc2500_patable = 0x6e; break; //
    case 5: cc2500_patable = 0xa9; break; //
    case 6: cc2500_patable = 0xfe; break; //
    case 7: cc2500_patable = 0xff; break; //
    default: cc2500_patable = 0; break;
  };

  CC2500_WriteReg(CC2500_3E_PATABLE, cc2500_patable);
}

#endif // PROTO_HAS_CC2500

