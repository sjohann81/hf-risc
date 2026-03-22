#ifndef UART_H
#define UART_H

#include <hf-risc.h>

/* the abstract UART device type */
typedef struct uart_dev uart_dev_t;

/* common baud rates */
typedef enum {
    BAUD_110 = 110,
    BAUD_300 = 300,
    BAUD_1200 = 1200,
    BAUD_2400 = 2400,
    BAUD_4800 = 4800,
    BAUD_9600 = 9600,
    BAUD_19200 = 19200,
    BAUD_38400 = 38400,
    BAUD_57600 = 57600,
    BAUD_115200 = 115200,
    BAUD_128000 = 128000,
    BAUD_230400 = 230400,
    BAUD_460800 = 460800,
    BAUD_921600 = 921600,
} uart_rate_t;

/* data bits */
typedef enum {
    DATA_BITS_5 = 5,
    DATA_BITS_6 = 6,
    DATA_BITS_7 = 7,
    DATA_BITS_8 = 8,
    DATA_BITS_9 = 9,
} uart_bits_t;

/* stop bits */
typedef enum {
    STOP_BITS_1 = 1,
    STOP_BITS_2 = 2,
} uart_stop_t;

/* parity mode */
typedef enum {
    PARITY_NONE = 0,
    PARITY_ODD,
    PARITY_EVEN,
    PARITY_MARK,
    PARITY_SPACE,
} uart_parity_t;

/* error codes */
typedef enum {
    UART_OK      =  0,
    UART_ERR     = -1,
    UART_TIMEOUT = -2,
    UART_TX_BUSY = -3,
    UART_RX_DATA = -4,
} uart_status_t;

/* low level driver callbacks */
typedef struct {
    uart_status_t (*init)(uart_dev_t *dev);
    uart_status_t (*tx_busy)(uart_dev_t *dev);
    uart_status_t (*rx_data)(uart_dev_t *dev);
    uart_status_t (*tx)(uart_dev_t *dev, char ch);
    uart_status_t (*rx)(uart_dev_t *dev, char *ch);
    uart_status_t (*irq_handler)(uart_dev_t *dev);
} uart_ops_t;

/* abstract UART handle */
typedef struct {
    const uart_ops_t *ops;
    uart_dev_t *dev;
} uart_t;

/* HAL functions */
static inline uart_status_t uart_init(uart_t *u)
{
    return u->ops->init(u->dev);
}

static inline uart_status_t uart_tx_busy(uart_t *u)
{
    return u->ops->tx_busy(u->dev);
}

static inline uart_status_t uart_rx_data(uart_t *u)
{
    return u->ops->rx_data(u->dev);
}

static inline uart_status_t uart_tx(uart_t *u, char ch)
{
    return u->ops->tx(u->dev, ch);
}

static inline uart_status_t uart_rx(uart_t *u, char *ch)
{
    return u->ops->rx(u->dev, ch);
}

static inline uart_status_t uart_irq_handle(uart_t *u)
{
    return u->ops->irq_handler(u->dev);
}

uart_status_t uart_read(uart_t *u, void *buf, size_t count);
uart_status_t uart_write(uart_t *u, void *buf, size_t count);

#endif /* UART_H */
