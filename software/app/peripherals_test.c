#include <hf-risc.h>

void porta_handler(void)
{
	printf("PAIN: %04x\n", PAIN);
}

void portb_handler(void)
{
	printf("PBIN: %04x\n", PBIN);
}

void timer0a_handler(void)
{
	printf("TIMER0A\n");
}

void timer0b_handler(void)
{
	printf("TIMER0B\n");
}

void timer1ctc_handler(void)
{
	printf("TIMER1CTC\n");
}

void timer1ocr_handler(void)
{
	printf("TIMER1OCR\n");
}

void test_spi0_master(uint16_t clock_div)
{
	char *str = "hello world!";
	int i;

	SPI0CTRL = clock_div;
	delay_us(5);

	for (i = 0; i < strlen(str); i++) {
		SPI0 = str[i];
		while (!(SPI0CTRL & SPI_DATA_VALID));
		SPI0CTRL &= ~SPI_DATA_XFER;
	}
}

void test_spi1_master(uint16_t clock_div)
{
	char *str = "hello world!";
	int i;

	SPI1CTRL = clock_div;
	delay_us(5);

	for (i = 0; i < strlen(str); i++) {
		SPI1 = str[i];
		while (!(SPI1CTRL & SPI_DATA_VALID));
		SPI1CTRL &= ~SPI_DATA_XFER;
	}
}

int main(void){
	// setup pin direction for SPI0 (SSN, CLK and MOSI are outputs, MISO is input)
	PADDR |= MASK_P4 | MASK_P5 | MASK_P6;
	PADDR &= ~MASK_P7;

	// setup SPI0, master mode. SPI0_SSN is needed for slave mode.
	PAALTCFG0 |= SPI0_CLK | SPI0_MOSI | SPI0_MISO;

	// setup pin direction for SPI1 (SSN, CLK and MOSI are outputs, MISO is input)
	PADDR |= MASK_P0 | MASK_P1 | MASK_P2;
	PADDR &= ~MASK_P3;

	// setup SPI0, master mode. SPI0_SSN is needed for slave mode.
	PAALTCFG0 |= SPI1_CLK | SPI1_MOSI | SPI1_MISO;

//	for (;;) {
		test_spi0_master(SPI_CLK_DIV4);
		test_spi0_master(SPI_CLK_DIV8);
		test_spi0_master(SPI_CLK_DIV16);
		test_spi0_master(SPI_CLK_DIV64);

		test_spi1_master(SPI_CLK_DIV4);
		test_spi1_master(SPI_CLK_DIV8);
		test_spi1_master(SPI_CLK_DIV16);
		test_spi1_master(SPI_CLK_DIV64);
//	}
}
