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

uint8_t uart_axi_rx(void)
{
	while (!(UART_AXI_STATUS & UART_AXI_STVALID));
	return UART_AXI_SDATA;
}


int main(void){
	uint8_t data;
	
	uart_axi_init(115200);
	
	/* read a byte (AXI uart rx) */
	data = uart_axi_rx();
	
	/* write two bytes (AXI uart tx) */
	uart_axi_tx(0x61);
	uart_axi_tx(0x62);
	
	/* write received data (AXI uart tx) */
	uart_axi_tx(data);

	return 0;
}
