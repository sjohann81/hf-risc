#include "uart_hfrisc.h"

/* port registers */
static inline hfrisc_uart_regs_t *get_regs(uart_dev_t *dev)
{
    return (hfrisc_uart_regs_t *)dev->config->base_addr;
}

/* software FIFO control */
static int fifo_full(hfrisc_uart_fifo_t *fifo)
{
    return (((fifo->tail + 1) & (UART_FIFO_SIZE - 1)) == fifo->head);
}

static int fifo_empty(hfrisc_uart_fifo_t *fifo)
{
    return fifo->head == fifo->tail;
}

static void fifo_put(hfrisc_uart_fifo_t *fifo, char c)
{
    if (!fifo_full(fifo)) {
        fifo->data[fifo->tail] = c;
        fifo->tail = (fifo->tail + 1) & (UART_FIFO_SIZE - 1);
    }
}

static char fifo_get(hfrisc_uart_fifo_t *fifo)
{
    char c = 0;
    
    if (!fifo_empty(fifo)) {
        c = fifo->data[fifo->head];
        fifo->head = (fifo->head + 1) & (UART_FIFO_SIZE - 1);
    }
    
    return c;
}

/* generic interrupt handler */
static uart_status_t uirq_handler(uart_dev_t *dev)
{
    hfrisc_uart_regs_t *regs = get_regs(dev);
    char ch;
    
    /* if RX interrupts are enabled and RX FIFO is not full,
     * take one char received from the wire and put it on FIFO */
    if (dev->config->irq_mask & dev->config->rx_mask) {
        if (!fifo_full(&dev->rx_fifo)) {
            ch = regs->RXR;
            fifo_put(&dev->rx_fifo, ch);
        }
    }
    
    return UART_OK;
}

/* low level driver API implementation */
static uart_status_t uinit(uart_dev_t *dev)
{
    hfrisc_uart_regs_t *regs = get_regs(dev);
    
	regs->DIV = dev->config->clock_hz / dev->config->baud_rate;
	regs->TXR = 0;

    dev->rx_fifo.head = 0;
    dev->rx_fifo.tail = 0;
    
    // enable RX interrupts
    if (dev->config->irq_mask)
        UARTMASK |= dev->config->irq_mask & dev->config->rx_mask;
        
    return UART_OK;
}

static uart_status_t utx_busy(uart_dev_t *dev)
{
    if (UARTCAUSE & dev->config->tx_mask)
            return UART_TX_BUSY;

    return UART_OK;
}

static uart_status_t urx_data(uart_dev_t *dev)
{
    if (dev->config->irq_mask & dev->config->rx_mask) {
        if (!fifo_empty(&dev->rx_fifo))
            return UART_RX_DATA;
    } else {
        if (UARTCAUSE & dev->config->rx_mask)
            return UART_RX_DATA;
    }

    return UART_OK;
}

static uart_status_t utx(uart_dev_t *dev, char ch)
{
    hfrisc_uart_regs_t *regs = get_regs(dev);
    
    while (UARTCAUSE & dev->config->tx_mask);
    regs->TXR = ch;
    
    return UART_OK;
}

static uart_status_t urx(uart_dev_t *dev, char *ch)
{
    hfrisc_uart_regs_t *regs = get_regs(dev);
    
    if (dev->config->irq_mask & dev->config->rx_mask) {
        while (fifo_empty(&dev->rx_fifo));
        *ch = fifo_get(&dev->rx_fifo);
    } else {
        while (!(UARTCAUSE & dev->config->rx_mask));
        *ch = regs->RXR;
    }
    
    return UART_OK;
}

/* low level driver callbacks */
const uart_ops_t hfrisc_uart_ops = {
    .init = uinit,
    .tx_busy = utx_busy,
    .rx_data = urx_data,
    .tx = utx,
    .rx = urx,
    .irq_handler = uirq_handler,
};
