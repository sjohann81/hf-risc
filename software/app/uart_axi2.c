#include <hf-risc.h>

#define UART_AXI_BASE			0xe4000000
#define UART_AXI_STATUS			(*(volatile uint32_t *)(UART_AXI_BASE + 0x010))
#define UART_AXI_SDATA			(*(volatile uint32_t *)(UART_AXI_BASE + 0x020))
#define UART_AXI_MDATA			(*(volatile uint32_t *)(UART_AXI_BASE + 0x030))
#define UART_AXI_DIVISOR		(*(volatile uint32_t *)(UART_AXI_BASE + 0x040))

#define UART_AXI_STREADY		(1 << 0)
#define UART_AXI_STVALID		(1 << 1)
#define UART_AXI_MTREADY		(1 << 2)
#define UART_AXI_MTVALID		(1 << 3)


/* AXIS UART driver functions */

void uart_axi_init(uint32_t baud)
{
	UART_AXI_DIVISOR = CPU_SPEED / baud;
	
	/* set PORTB pin 6 as an output (uart_axi tx) */
	PBDDR |= MASK_P6;
	
	/* set PORTB pin 7 as an input (uart_axi rx) */
	PBDDR &= ~MASK_P7;
}

void uart_axi_tx(uint8_t byte)
{
	UART_AXI_MDATA = byte;
	while (!(UART_AXI_STATUS & UART_AXI_MTREADY));
}

uint32_t uart_axi_rxdata(void)
{
	return (UART_AXI_STATUS & UART_AXI_STVALID);
}

uint8_t uart_axi_rx(void)
{
	while (!uart_axi_rxdata());
	return UART_AXI_SDATA;
}


/* simple uart_write / uart_read functions */

void uart_write(char *s)
{
	while (*s) {
		uart_axi_tx(*s);
		s++;
	}
}

void uart_read(char *s)
{
	char c;

	while ((c = uart_axi_rx()) != '\n')
		*s++ = c;

	*s++ = '\0';
}


int main(void){
	char buf[256];
	
	uart_axi_init(115200);
	
	while (1) {
		uart_write("hello world!\n");
		uart_write("type something: ");
		uart_read(buf);
		uart_write("echo: ");
		uart_write(buf);
	}

	return 0;
}
