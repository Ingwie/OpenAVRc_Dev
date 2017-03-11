//

#ifndef _SPI_H_
#define _SPI_H_

void spi_disable(void);
void spi_enable_master_mode(void);
void spi_tx(uint8_t value);
uint8_t spi_rx(void);
uint8_t spi_xfer(uint8_t value);

#endif
