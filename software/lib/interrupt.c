#include <hf-risc.h>

/* defined interrupt handlers */
void dummy_handler(void);
void irq0_handler(void);

/* user defined interrupt handlers */
void irq1_handler(void) __attribute__ ((weak, alias ("dummy_handler")));
void irq2_handler(void) __attribute__ ((weak, alias ("dummy_handler")));
void irq3_handler(void) __attribute__ ((weak, alias ("dummy_handler")));
void irq4_handler(void) __attribute__ ((weak, alias ("dummy_handler")));
void irq5_handler(void) __attribute__ ((weak, alias ("dummy_handler")));
void irq6_handler(void) __attribute__ ((weak, alias ("dummy_handler")));
void irq7_handler(void) __attribute__ ((weak, alias ("dummy_handler")));

void porta_handler(void) __attribute__ ((weak, alias ("dummy_handler")));
void portb_handler(void) __attribute__ ((weak, alias ("dummy_handler")));
void portc_handler(void) __attribute__ ((weak, alias ("dummy_handler")));
void portd_handler(void) __attribute__ ((weak, alias ("dummy_handler")));

void timer0a_handler(void) __attribute__ ((weak, alias ("dummy_handler")));
void timer0b_handler(void) __attribute__ ((weak, alias ("dummy_handler")));
void timer1ctc_handler(void) __attribute__ ((weak, alias ("dummy_handler")));
void timer1ocr_handler(void) __attribute__ ((weak, alias ("dummy_handler")));
void timer2ctc_handler(void) __attribute__ ((weak, alias ("dummy_handler")));
void timer2ocr_handler(void) __attribute__ ((weak, alias ("dummy_handler")));
void timer3ctc_handler(void) __attribute__ ((weak, alias ("dummy_handler")));
void timer3ocr_handler(void) __attribute__ ((weak, alias ("dummy_handler")));
void timer4ctc_handler(void) __attribute__ ((weak, alias ("dummy_handler")));
void timer4ocr_handler(void) __attribute__ ((weak, alias ("dummy_handler")));
void timer5ctc_handler(void) __attribute__ ((weak, alias ("dummy_handler")));
void timer5ocr_handler(void) __attribute__ ((weak, alias ("dummy_handler")));
void timer6ctc_handler(void) __attribute__ ((weak, alias ("dummy_handler")));
void timer6ocr_handler(void) __attribute__ ((weak, alias ("dummy_handler")));
void timer7ctc_handler(void) __attribute__ ((weak, alias ("dummy_handler")));
void timer7ocr_handler(void) __attribute__ ((weak, alias ("dummy_handler")));

void uart0rx_handler(void) __attribute__ ((weak, alias ("dummy_handler")));
void uart0tx_handler(void) __attribute__ ((weak, alias ("dummy_handler")));
void uart1rx_handler(void) __attribute__ ((weak, alias ("dummy_handler")));
void uart1tx_handler(void) __attribute__ ((weak, alias ("dummy_handler")));
void uart2rx_handler(void) __attribute__ ((weak, alias ("dummy_handler")));
void uart2tx_handler(void) __attribute__ ((weak, alias ("dummy_handler")));
void uart3rx_handler(void) __attribute__ ((weak, alias ("dummy_handler")));
void uart3tx_handler(void) __attribute__ ((weak, alias ("dummy_handler")));
void uart4rx_handler(void) __attribute__ ((weak, alias ("dummy_handler")));
void uart4tx_handler(void) __attribute__ ((weak, alias ("dummy_handler")));
void uart5rx_handler(void) __attribute__ ((weak, alias ("dummy_handler")));
void uart5tx_handler(void) __attribute__ ((weak, alias ("dummy_handler")));
void uart6rx_handler(void) __attribute__ ((weak, alias ("dummy_handler")));
void uart6tx_handler(void) __attribute__ ((weak, alias ("dummy_handler")));
void uart7rx_handler(void) __attribute__ ((weak, alias ("dummy_handler")));
void uart7tx_handler(void) __attribute__ ((weak, alias ("dummy_handler")));

void spi0_handler(void) __attribute__ ((weak, alias ("dummy_handler")));
void spi1_handler(void) __attribute__ ((weak, alias ("dummy_handler")));
void spi2_handler(void) __attribute__ ((weak, alias ("dummy_handler")));
void spi3_handler(void) __attribute__ ((weak, alias ("dummy_handler")));

void i2c0_handler(void) __attribute__ ((weak, alias ("dummy_handler")));
void i2c1_handler(void) __attribute__ ((weak, alias ("dummy_handler")));
void i2c2_handler(void) __attribute__ ((weak, alias ("dummy_handler")));
void i2c3_handler(void) __attribute__ ((weak, alias ("dummy_handler")));

void adc0_handler(void) __attribute__ ((weak, alias ("dummy_handler")));
void adc1_handler(void) __attribute__ ((weak, alias ("dummy_handler")));
void adc2_handler(void) __attribute__ ((weak, alias ("dummy_handler")));
void adc3_handler(void) __attribute__ ((weak, alias ("dummy_handler")));

void dac0_handler(void) __attribute__ ((weak, alias ("dummy_handler")));
void dac1_handler(void) __attribute__ ((weak, alias ("dummy_handler")));
void dac2_handler(void) __attribute__ ((weak, alias ("dummy_handler")));
void dac3_handler(void) __attribute__ ((weak, alias ("dummy_handler")));

/* interrupt vectors */
void (*irq_vector[])(void) = {
	irq0_handler,
	irq1_handler,
	irq2_handler,
	irq3_handler,
	irq4_handler,
	irq5_handler,
	irq6_handler,
	irq7_handler,
};

void (*gpio_vector[])(void) = {
	porta_handler,
	portb_handler,
	portc_handler,
	portd_handler
};

void (*timer_vector[])(void) = {
	timer0a_handler,
	timer0b_handler,
	timer1ctc_handler,
	timer1ocr_handler,
	timer2ctc_handler,
	timer2ocr_handler,
	timer3ctc_handler,
	timer3ocr_handler,
	timer4ctc_handler,
	timer4ocr_handler,
	timer5ctc_handler,
	timer5ocr_handler,
	timer6ctc_handler,
	timer6ocr_handler,
	timer7ctc_handler,
	timer7ocr_handler,
};

void (*uart_vector[])(void) = {
	uart0rx_handler,
	uart0tx_handler,
	uart1rx_handler,
	uart1tx_handler,
	uart2rx_handler,
	uart2tx_handler,
	uart3rx_handler,
	uart3tx_handler,
	uart4rx_handler,
	uart4tx_handler,
	uart5rx_handler,
	uart5tx_handler,
	uart6rx_handler,
	uart6tx_handler,
	uart7rx_handler,
	uart7tx_handler
};

void (*spi_vector[])(void) = {
	spi0_handler,
	spi1_handler,
	spi2_handler,
	spi3_handler
};

void (*i2c_vector[])(void) = {
	i2c0_handler,
	i2c1_handler,
	i2c2_handler,
	i2c3_handler
};

void (*adc_vector[])(void) = {
	adc0_handler,
	adc1_handler,
	adc2_handler,
	adc3_handler
};

void (*dac_vector[])(void) = {
	dac0_handler,
	dac1_handler,
	dac2_handler,
	dac3_handler
};

void dummy_handler(void)
{
	printf("irq!");
}

void irq0_handler(void)
{
	uint32_t irq, k, i, pins;

	k = 1;
	do {
		i = 0;
		switch (S0CAUSE & k) {
		case 0:
			break;
		case MASK_S0CAUSE_BASE:
			break;
		case MASK_S0CAUSE_GPIO:
			irq = (GPIOCAUSE ^ GPIOCAUSEINV) & GPIOMASK;

			do {
				if (irq & 0x1) {
					/* toggle interrupt cause */
					switch (i) {
					case 0:
						pins = (PAIN ^ PAININV) & PAINMASK;
						PAININV ^= pins;
						break;
					case 1:
						pins = (PBIN ^ PBININV) & PBINMASK;
						PBININV ^= pins;
						break;
					case 2:
						pins = (PCIN ^ PCININV) & PCINMASK;
						PCININV ^= pins;
						break;
					case 3:
						pins = (PDIN ^ PDININV) & PDINMASK;
						PDININV ^= pins;
						break;
					default:
						break;
					};
					/* call irq handler */
					gpio_vector[i]();
				}
				irq >>= 1;
				++i;
			} while (irq);
			break;
		case MASK_S0CAUSE_TIMER:
			irq = (TIMERCAUSE ^ TIMERCAUSEINV) & TIMERMASK;

			do {
				if (irq & 0x1) {
					/* toggle interrupt cause */
					TIMERCAUSEINV ^= (irq & 0x1) << i;
					/* call irq handler */
					timer_vector[i]();
				}
				irq >>= 1;
				++i;
			} while (irq);
			break;
		case MASK_S0CAUSE_UART:
			break;
		case MASK_S0CAUSE_SPI:
			break;
		case MASK_S0CAUSE_I2C:
			break;
		default:
			printf("unknown irq source for S0CAUSE %08x\n", S0CAUSE);
			break;
		}
		k <<= 1;
	} while (S0CAUSE && k);
}

void irq_enable(void)
{
#ifndef DEBUG_PORT
	uint16_t d;

	d = (uint16_t)(CPU_SPEED / 57600);
	UART0DIV = d;
	UART0 = 0;

	PAALTCFG0 |= MASK_UART0;
#endif
	/* enable mask for Segment 0 (tied to IRQ0 line) */
	IRQ_MASK = MASK_IRQ0;
	/* global interrupts enable */
	IRQ_STATUS = 1;
}

void irq_handler(uint32_t cause, uint32_t *stack)
{
	int32_t i = 0;

	do {
		if (cause & 0x1) {
			if (irq_vector[i]) {
				irq_vector[i]();
			}
		}
		cause >>= 1;
		++i;
	} while (cause);
}

int32_t exception_handler(int32_t service, int32_t arg0, int32_t arg1, int32_t arg2)
{
	/* for testing purposes, right now */
	printf("syscall: %d, [arg0: %d, arg1: %d, arg2: %d]", service, arg0, arg1, arg2);
	
	return -1;
}
