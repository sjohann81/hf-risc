/*
 * HF-RISC UART is a very poor implementation of an UART. It does not
 * have a way to change data bits, stop bits and more flexible interrupt
 * system. The registers of each device are very limited (only a shared
 * TX/RX data register and a baud rate divisor). The way UARTs are wired
 * on the platform is very limiting, because the status of UARTs is not
 * defined on a per peripheral status register mapped on each device
 * address space.
 * 
 * To make matters worse, there is no TX or RX FIFO in hardware,
 * so TX interrupts are useless. We will only use a RX software FIFO
 * when operating this driver in interrupt mode, and hope for the best.
 * TX will be handled in polling mode only.
 */

#ifndef UART_HFRISC_H
#define UART_HFRISC_H

#include "uart_hal.h"

/* 
 * redefinition of UART base addresses and masks for HF-RISC
 * check hf-risc.h and interrupt.c for reference
 */
#define UART0_BASE              (UART_BASE + 0x4000)
#define UART1_BASE              (UART_BASE + 0x4400)
#define UART2_BASE              (UART_BASE + 0x4800)
#define UART3_BASE              (UART_BASE + 0x4c00)

/* bit masks for UARTCAUSE, UARTCAUSE_INV and UARTMASK */
#define UART0_RXDATA		    (1 << 0)
#define UART0_TXBUSY		    (1 << 1)
#define UART1_RXDATA		    (1 << 2)
#define UART1_TXBUSY		    (1 << 3)
#define UART2_RXDATA		    (1 << 4)
#define UART2_TXBUSY		    (1 << 5)
#define UART3_RXDATA		    (1 << 6)
#define UART3_RXBUSY		    (1 << 7)

#define UART_FIFO_SIZE          128

/* HF-RISC UART registers */
typedef struct {
    union {
        volatile uint32_t RXR;          // receive register
        volatile uint32_t TXR;          // transmit register
    };
    volatile const uint32_t _reserved[3];
    volatile uint32_t DIV;              // baud rate divisor
} hfrisc_uart_regs_t;

/* HF-RISC UART configuration */
typedef struct {
    uintptr_t base_addr;
    uint32_t baud_rate;
    uint32_t clock_hz;
    uint32_t tx_mask;                   // status not on UART regs =(
    uint32_t rx_mask;                   // status not on UART regs =(
    uint32_t irq_mask;                  // status not on UART regs =(
} hfrisc_uart_config_t;

/* software FIFO data type */
typedef struct {
    char data[UART_FIFO_SIZE];
    volatile uint16_t head, tail;
} hfrisc_uart_fifo_t;

/* HF-RISC UART device */
typedef struct uart_dev {
    const hfrisc_uart_config_t *config;
    hfrisc_uart_fifo_t rx_fifo;
} hfrisc_uart_dev_t;

extern const uart_ops_t hfrisc_uart_ops;

#endif
