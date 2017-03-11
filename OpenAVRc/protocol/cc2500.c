//


#ifdef PROTO_HAS_CC2500
#include <util/delay.h>
#include "common.h"
#include "spi.h"
#include "interface.h"

void CC2500_WriteReg(uint8_t address, uint8_t data)
{
    CS_LO();
    spi_xfer(address);
    spi_xfer(data);
    CS_HI();
}

static void ReadRegisterMulti(uint8_t address, uint8_t data[], uint8_t length)
{
    unsigned char i;

    CS_LO();
    spi_xfer(address);
    for(i = 0; i < length; i++)
    {
        data[i] = spi_xfer(0);
    }
    CS_HI();
}

uint8_t CC2500_ReadReg(uint8_t address)
{
    CS_LO();
    spi_xfer(CC2500_READ_SINGLE | address);
    uint8_t data = spi_xfer(0);
    CS_HI();
    return data;
}

void CC2500_ReadData(uint8_t *dpbuffer, int len)
{
    ReadRegisterMulti(CC2500_3F_RXFIFO | CC2500_READ_BURST, dpbuffer, len);
}

uint8_t CC2500_Strobe(uint8_t strobe_cmd)
{
    CS_LO();
    uint8_t chip_status = spi_xfer(strobe_cmd);
    CS_HI();
return chip_status;
}


void CC2500_WriteRegisterMulti(uint8_t address, const uint8_t data[], uint8_t length)
{
    CS_LO();
    spi_xfer(CC2500_WRITE_BURST | address);
    for(int i = 0; i < length; i++)
    {
        spi_xfer(data[i]);
    }
    CS_HI();
}

void CC2500_WriteData(uint8_t *dpbuffer, uint8_t len)
{
//    CC2500_Strobe(CC2500_SFTX);
    CC2500_WriteRegisterMulti(CC2500_3F_TXFIFO, dpbuffer, len);
//    CC2500_Strobe(CC2500_STX);
}


void CC2500_SetTxRxMode(enum TXRX_State mode)
{

    if(mode == TX_EN)
	{
        CC2500_WriteReg(CC2500_02_IOCFG0, 0x2F | 0x40);//set
        CC2500_WriteReg(CC2500_00_IOCFG2, 0x2F);//clear
    }
	else if (mode == RX_EN)
	{
        CC2500_WriteReg(CC2500_02_IOCFG0, 0x2F);//clear
        CC2500_WriteReg(CC2500_00_IOCFG2, 0x2F | 0x40);//set
    }
	else
	{
        CC2500_WriteReg(CC2500_02_IOCFG0, 0x2F);//clear
        CC2500_WriteReg(CC2500_00_IOCFG2, 0x2F);//clear
    }
}

void CC2500_Reset()
{
    CC2500_Strobe(CC2500_SRES);
	_delay_us(100); // Should be > 50us. see datasheet.
}


void CC2500_SetPower(uint8_t power)
{
/*
// CC2500 Power Output

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

    switch(power)
	{
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

