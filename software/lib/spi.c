/* file:          spi.c
 * description:   low level SPI driver
 * date:          09/2015
 * author:        Sergio Johann Filho <sergio.filho@pucrs.br>
 */

#include <hf-risc.h>

/* SPI mode: 0 ~ 3 */
#define SPI_MODE	0

/* SPI interface - EXTIO_OUT pins 0 (chip select), 1 (clock), 2 (master output) and 3 (chip select 2) */
#define SPI_CS		0x01
#define SPI_SCK		0x02
#define SPI_MOSI	0x04
#define SPI_CS2		0x08

/* SPI interface - EXIO_IN pin 3 (master input) */
#define SPI_MISO	0x08

void spi_setup(void){
// setup IO pins ...
	EXTIO_OUT |= SPI_CS | SPI_CS2;

#if SPI_MODE == 0 || SPI_MODE == 1
	EXTIO_OUT &= ~SPI_SCK;
#else
	EXTIO_OUT |= SPI_SCK;
#endif
}

void spi_start(void){
// set CS low
	EXTIO_OUT &= ~SPI_CS;
}

void spi_stop(void){
// set CS high
	EXTIO_OUT |= SPI_CS;
}

void spi_start2(void){
// set CS low
	EXTIO_OUT &= ~SPI_CS2;
}

void spi_stop2(void){
// set CS high
	EXTIO_OUT |= SPI_CS2;
}

int8_t spi_sendrecv(int8_t data){
	int32_t i;
	int8_t newdata = 0;

#if SPI_MODE == 0	
	for (i = 0; i < 8; i++){
		if (data & 0x80){
			EXTIO_OUT |= SPI_MOSI;
		}else{
			EXTIO_OUT &= ~SPI_MOSI;
		}
		data <<= 1;
		EXTIO_OUT |= SPI_SCK;
		newdata <<= 1;
		newdata |= ((EXTIO_IN & SPI_MISO) ? 1 : 0);
		EXTIO_OUT &= ~SPI_SCK;
	}
	return newdata;
#endif
#if SPI_MODE == 1
	for (i = 0; i < 8; i++){
		EXTIO_OUT |= SPI_SCK;
		if (data & 0x80){
			EXTIO_OUT |= SPI_MOSI;
		}else{
			EXTIO_OUT &= ~SPI_MOSI;
		}
		data <<= 1;
		EXTIO_OUT &= ~SPI_SCK;
		newdata <<= 1;
		newdata |= ((EXTIO_IN & SPI_MISO) ? 1 : 0);
	}
	return newdata;
#endif
#if SPI_MODE == 2
	for (i = 0; i < 8; i++){
		if (data & 0x80){
			EXTIO_OUT |= SPI_MOSI;
		}else{
			EXTIO_OUT &= ~SPI_MOSI;
		}
		data <<= 1;
		EXTIO_OUT &= ~SPI_SCK;
		newdata <<= 1;
		newdata |= ((EXTIO_IN & SPI_MISO) ? 1 : 0);
		EXTIO_OUT |= SPI_SCK;
	}
	return newdata;
#endif
#if SPI_MODE == 3
	for (i = 0; i < 8; i++){
		EXTIO_OUT &= ~SPI_SCK;
		if (data & 0x80){
			EXTIO_OUT |= SPI_MOSI;
		}else{
			EXTIO_OUT &= ~SPI_MOSI;
		}
		data <<= 1;
		EXTIO_OUT |= SPI_SCK;
		newdata <<= 1;
		newdata |= ((EXTIO_IN & SPI_MISO) ? 1 : 0);
	}
	return newdata;
#endif
}

