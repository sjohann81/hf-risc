/* file:          spi.c
 * description:   bitbang SPI master driver
 * date:          09/2015
 * author:        Sergio Johann Filho <sergio.filho@pucrs.br>
 */

#include <hf-risc.h>

/* SPI mode: 0 ~ 3 */
#define SPI_MODE	0

/* SPI interface - PORTA output pins 4 (chip select), 5 (clock) and 6 (master output) */
#define IN		PAIN
#define SPI_CS		MASK_P4
#define SPI_SCK		MASK_P5
#define SPI_MOSI	MASK_P6

/* SPI interface - PORTA pin 7 (master input) */
#define OUT		PAOUT
#define DDR		PADDR
#define SPI_MISO	MASK_P7

void spi_setup(void){
// setup IO pins ...
	DDR |= SPI_CS | SPI_SCK | SPI_MOSI;
	OUT |= SPI_CS;

#if SPI_MODE == 0 || SPI_MODE == 1
	OUT &= ~SPI_SCK;
#else
	OUT |= SPI_SCK;
#endif
}

void spi_start(void){
// set CS low
	OUT &= ~SPI_CS;
}

void spi_stop(void){
// set CS high
	OUT |= SPI_CS;
}

int8_t spi_transfer(int8_t data){
	int32_t i;
	int8_t newdata = 0;

#if SPI_MODE == 0	
	for (i = 0; i < 8; i++){
		if (data & 0x80){
			OUT |= SPI_MOSI;
		}else{
			OUT &= ~SPI_MOSI;
		}
		data <<= 1;
		OUT |= SPI_SCK;
		newdata <<= 1;
		newdata |= ((IN & SPI_MISO) ? 1 : 0);
		OUT &= ~SPI_SCK;
	}
	return newdata;
#endif
#if SPI_MODE == 1
	for (i = 0; i < 8; i++){
		OUT |= SPI_SCK;
		if (data & 0x80){
			OUT |= SPI_MOSI;
		}else{
			OUT &= ~SPI_MOSI;
		}
		data <<= 1;
		OUT &= ~SPI_SCK;
		newdata <<= 1;
		newdata |= ((IN & SPI_MISO) ? 1 : 0);
	}
	return newdata;
#endif
#if SPI_MODE == 2
	for (i = 0; i < 8; i++){
		if (data & 0x80){
			OUT |= SPI_MOSI;
		}else{
			OUT &= ~SPI_MOSI;
		}
		data <<= 1;
		OUT &= ~SPI_SCK;
		newdata <<= 1;
		newdata |= ((IN & SPI_MISO) ? 1 : 0);
		OUT |= SPI_SCK;
	}
	return newdata;
#endif
#if SPI_MODE == 3
	for (i = 0; i < 8; i++){
		OUT &= ~SPI_SCK;
		if (data & 0x80){
			OUT |= SPI_MOSI;
		}else{
			OUT &= ~SPI_MOSI;
		}
		data <<= 1;
		OUT |= SPI_SCK;
		newdata <<= 1;
		newdata |= ((IN & SPI_MISO) ? 1 : 0);
	}
	return newdata;
#endif
}

