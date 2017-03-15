//


#include <avr/interrupt.h>

#include "../../protocol/common.h"

//---------------------------
// AVR SPI functions
//---------------------------

void spi_disable(void)
{
  SPCR &= ~(1<<SPE);
}

//----------------------

void spi_enable_master_mode(void)
{
CS_HI();
// Set MOSI as input.
DDRB &= ~(1<<DDB3); // MOSI.
// Disable pullup
PORTB &= ~(1<<PORTB3);
// Set MOSI and SCK as output.
DDRB |= (1<<DDB3) | (1<<DDB5);

/*
// Set MOSI and SCK as output.
DDRB |= (1<<MOSI)|(1<<SCK);
*/

// Enable SPI as Master, MSB first.
SPCR = (1<<SPE) | (1<<MSTR) | (0<<DORD);
// Set clock rate Fosc/2
SPSR |= (1<<SPI2X);
// Note : Make sure Slave Select pin is output or input pullup.
}

//----------------------

#if 0
// Was for A7105. Deprecated / Obsolete.
Use ...

#define spi_tx(value)  (void)spi_xfer(value)
#define spi_rx()  spi_xfer(0)

void spi_tx(uint8_t value)
{
// Half duplex mode (MOSI connected to MISO via 1K Ohm).
/* Start transmission */
SPDR = value;
/* Wait for transmission to complete */
while (!(SPSR & (1<<SPIF))); // Wait for SPIF bit set
}

//----------------------

uint8_t spi_rx(void)
{
// Half duplex mode (MOSI connected to MISO via 1K Ohm).
// Clock Slave by sending dummy data.
SPDR = 0;
/* Wait for reception to complete */
while (!(SPSR & (1<<SPIF)));
return SPDR;
}
#endif

//----------------------

uint8_t spi_xfer(uint8_t value)
{
  // Full Duplex (4 wire) spi for nrf24l01+
  SPDR = value;
  /* Wait for transfer to complete */
  while (!(SPSR & (1<<SPIF)));
  return SPDR;
}

//----------------------
