#include "uart_hal.h"

uart_status_t uart_read(uart_t *u, void *buf, size_t count)
{
    size_t i;
    char ch, *p;
    uart_status_t val = UART_OK;

    p = (char *)buf;

    if (count == 0)
        return uart_rx_data(u);

    for (i = 0; i < count; i++) {
        val = uart_rx(u, &ch);

        if (val != UART_OK)
            break;

        p[i] = ch;
    }

    return val;
}

uart_status_t uart_write(uart_t *u, void *buf, size_t count)
{
    size_t i;
    char *p;
    uart_status_t val = UART_OK;

    p = (char *)buf;

    if (count == 0)
        return uart_tx_busy(u);

    for (i = 0; i < count; i++) {
        val = uart_tx(u, p[i]);

        if (val != UART_OK)
            break;
    }

    return val;
}
