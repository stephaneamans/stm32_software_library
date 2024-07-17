#include <stdint.h>

#ifndef SPI_REGBASE_H_
#define SPI_REGBASE_H_

/* SPI registers structure        */
struct t_spi_regs
{
    volatile uint32_t CR1;
    volatile uint32_t CR2;
    volatile uint32_t SR;
    volatile uint32_t DR;
    volatile uint32_t CRCPR;
    volatile uint32_t RXCRCR;
    volatile uint32_t TXCRCR;
    volatile uint32_t I2SCFGR;
};

#endif /* SPI_REGBASE_H_ */
