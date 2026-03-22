#include <hf-risc.h>
#include <uart_hfrisc.h>

/* UART configuration */
static const hfrisc_uart_config_t uart0_config = {
    .base_addr  = UART0_BASE,
    .baud_rate  = BAUD_57600,
    .clock_hz   = CPU_SPEED,
    .tx_mask    = UART0_TXBUSY,
    .rx_mask    = UART0_RXDATA,
    .irq_mask   = UART0_RXDATA,     // comment this line for polling mode
};

static hfrisc_uart_dev_t uart0_dev = {
    .config  = &uart0_config,
};

uart_t uart_console = {
    .ops = &hfrisc_uart_ops,
    .dev = (uart_dev_t *)&uart0_dev,
};

/* UART irq handler -> call generic driver handler from any UART */
void uart0rx_handler(void)
{
    uart_irq_handle(&uart_console);
}

int main(void)
{
    char *hey = "hello from UART!\n";
    char *hey2 = "each 10 typed chars, I will echo them back..\n";
    char buf[128];
    
    putchar('!');
    putchar('\n');
    printf("hello world!\n");

    /* select UART0 pins */
   	PAALTCFG0 |= (MASK_UART0_TX | MASK_UART0_RX);
    
    uart_init(&uart_console);
    uart_write(&uart_console, hey, strlen(hey));
    uart_write(&uart_console, hey2, strlen(hey2));
    while (1) {
        uart_read(&uart_console, buf, 10);
        uart_write(&uart_console, buf, 10);
    }

    return 0;
}
