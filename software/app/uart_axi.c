#include <hf-risc.h>

#define UART_AXI_BASE			0xe4000000
#define UART_AXI_DIVISOR		(*(volatile uint32_t *)(UART_AXI_BASE + 0x000))
#define UART_AXI_STATUS			(*(volatile uint32_t *)(UART_AXI_BASE + 0x010))
#define UART_AXI_MDATA			(*(volatile uint32_t *)(UART_AXI_BASE + 0x020))
#define UART_AXI_SDATA			(*(volatile uint32_t *)(UART_AXI_BASE + 0x030))

#define UART_AXI_MTREADY		(1 << 0)
#define UART_AXI_MTVALID		(1 << 1)
#define UART_AXI_STREADY		(1 << 2)
#define UART_AXI_STVALID		(1 << 3)


int main(void){
	char data;
	
	UART_AXI_DIVISOR = CPU_SPEED / 115200;
	
	/* read a byte (AXI uart rx) */
	while (!(UART_AXI_STATUS & UART_AXI_MTVALID));
	data = UART_AXI_MDATA;
	
	/* write two bytes (AXI uart tx) */
	UART_AXI_SDATA = 0x61;
	while (!(UART_AXI_STATUS & UART_AXI_STREADY));
	UART_AXI_SDATA = 0x62;
	while (!(UART_AXI_STATUS & UART_AXI_STREADY));
	
	/* write received data (AXI uart tx) */
	UART_AXI_SDATA = data;
	while (!(UART_AXI_STATUS & UART_AXI_STREADY));

	return 0;
}
