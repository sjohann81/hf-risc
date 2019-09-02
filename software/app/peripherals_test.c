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

void test_eeprom(void)
{
	int i;
	uint32_t *ptr;
	uint8_t *wren;

	/* test EEPROM writes / reads */
	wren = (uint8_t *)EXT_SPI2_WREN;
	ptr = (uint32_t *)EXT_SPI2_16BA;

	for (i = 0; i < 4; i++) {
		*wren = 0x00;
		ptr[i] = 0xdeadbeef;
		delay_ms(5);
	}

	for (i = 0; i < 4; i++) {
		printf("%x\n", ptr[i]);
	}
}

void test_sram(void)
{
	int i;
	uint32_t *ptr;
	uint16_t *ptrh;
	uint8_t *ptrb;

	/* test SRAM writes / reads (word mode) */
	ptr = (uint32_t *)EXT_SPI_24BA;

	for (i = 0; i < 4; i++) {
		ptr[i] = 0xb16b00b5;
	}

	for (i = 0; i < 4; i++) {
		printf("%x\n", ptr[i]);
	}

	/* test SRAM writes / reads (half word mode) */
	ptrh = (uint16_t *)EXT_SPI_24BA;

	for (i = 0; i < 4; i++) {
		ptrh[i] = 0xdead;
	}

	for (i = 0; i < 4; i++) {
		printf("%x\n", ptrh[i]);
	}

	/* test SRAM writes / reads (byte word mode) */
	ptrb = (uint8_t *)EXT_SPI_24BA;

	for (i = 0; i < 4; i++) {
		ptrb[i] = 0xaa;
	}

	for (i = 0; i < 4; i++) {
		printf("%x\n", ptrb[i]);
	}
}

int main(void){
	// setup pin direction for SPI0 (SSN, CLK and MOSI are outputs, MISO is input)
	PADDR |= MASK_P4 | MASK_P5 | MASK_P6;
	PADDR &= ~MASK_P7;

	// setup SPI0, master mode. SPI0_SSN is needed for slave mode.
	PAALTCFG0 |= SPI0_CLK | SPI0_MOSI | SPI0_MISO;

	// setup pin direction for SPI1 (SSN, CLK and MOSI are outputs, MISO is input)
	PADDR |= MASK_P9 | MASK_P10 | MASK_P11;
	PADDR &= ~MASK_P8;

	// setup SPI0, master mode. SPI0_SSN is needed for slave mode.
	PAALTCFG0 |= SPI1H_CLK | SPI1H_MOSI | SPI1H_MISO;

	test_spi0_master(SPI_CLK_DIV4);
	test_spi0_master(SPI_CLK_DIV8);
	test_spi0_master(SPI_CLK_DIV16);
	test_spi0_master(SPI_CLK_DIV64);

	test_spi1_master(SPI_CLK_DIV4);
	test_spi1_master(SPI_CLK_DIV8);
	test_spi1_master(SPI_CLK_DIV16);
	test_spi1_master(SPI_CLK_DIV64);

	test_eeprom();
	test_sram();

	return 0;
}
