/* file:          hf_risc.h
 * description:   basic C type abstraction and HAL for HF-RISC
 * date:          09/2015
 * author:        Sergio Johann Filho <sergio.filho@pucrs.br>
 */

/* C type extensions */
typedef unsigned char			uint8_t;
typedef char				int8_t;
typedef unsigned short int		uint16_t;
typedef short int			int16_t;
typedef unsigned int			uint32_t;
typedef int				int32_t;
typedef unsigned long long		uint64_t;
typedef long long			int64_t;
typedef unsigned long			size_t;
typedef void				(*funcptr)();

/* disable interrupts, return previous int status / enable interrupts */
#define _di				interrupt_set(0)
#define _ei(S)				interrupt_set(S)

/* memory address map */
#define ADDR_ROM_BASE			0x00000000
#define ADDR_RAM_BASE			0x40000000
#define ADDR_RESERVED_BASE		0x80000000

/* peripheral addresses and irq lines */
#define PERIPHERALS_BASE		0xf0000000
#define IRQ_VECTOR			(*(volatile uint32_t *)(PERIPHERALS_BASE + 0x000))
#define IRQ_CAUSE			(*(volatile uint32_t *)(PERIPHERALS_BASE + 0x010))
#define IRQ_MASK			(*(volatile uint32_t *)(PERIPHERALS_BASE + 0x020))
#define IRQ_STATUS			(*(volatile uint32_t *)(PERIPHERALS_BASE + 0x030))
#define IRQ_EPC				(*(volatile uint32_t *)(PERIPHERALS_BASE + 0x040))
#define COUNTER				(*(volatile uint32_t *)(PERIPHERALS_BASE + 0x050))
#define COMPARE				(*(volatile uint32_t *)(PERIPHERALS_BASE + 0x060))
#define COMPARE2			(*(volatile uint32_t *)(PERIPHERALS_BASE + 0x070))
#define EXTIO_IN			(*(volatile uint32_t *)(PERIPHERALS_BASE + 0x080))
#define EXTIO_OUT			(*(volatile uint32_t *)(PERIPHERALS_BASE + 0x090))
#define EXTIO_DIR			(*(volatile uint32_t *)(PERIPHERALS_BASE + 0x0a0))
#define DEBUG_ADDR			(*(volatile uint32_t *)(PERIPHERALS_BASE + 0x0d0))
#define UART				(*(volatile uint32_t *)(PERIPHERALS_BASE + 0x0e0))
#define UART_DIVISOR			(*(volatile uint32_t *)(PERIPHERALS_BASE + 0x0f0))

#define IRQ_COUNTER			0x00000001
#define IRQ_COUNTER_NOT			0x00000002
#define IRQ_COUNTER2			0x00000004
#define IRQ_COUNTER2_NOT		0x00000008
#define IRQ_COMPARE			0x00000010
#define IRQ_COMPARE2			0x00000020
#define IRQ_UART_READ_AVAILABLE		0x00000040
#define IRQ_UART_WRITE_AVAILABLE	0x00000080

#define EXT_IRQ0			0x00010000
#define EXT_IRQ1			0x00020000
#define EXT_IRQ2			0x00040000
#define EXT_IRQ3			0x00080000
#define EXT_IRQ4			0x00100000
#define EXT_IRQ5			0x00200000
#define EXT_IRQ6			0x00400000
#define EXT_IRQ7			0x00800000
#define EXT_IRQ0_NOT			0x01000000
#define EXT_IRQ1_NOT			0x02000000
#define EXT_IRQ2_NOT			0x04000000
#define EXT_IRQ3_NOT			0x08000000
#define EXT_IRQ4_NOT			0x10000000
#define EXT_IRQ5_NOT			0x20000000
#define EXT_IRQ6_NOT			0x40000000
#define EXT_IRQ7_NOT			0x80000000

#define PERIPHERALS_BASE_EXT		0xf8000000
#define XTEA_BASE			0xfa000000
#define XTEA_CONTROL			(*(volatile uint32_t *)(XTEA_BASE + 0x000))
#define XTEA_KEY0			(*(volatile uint32_t *)(XTEA_BASE + 0x010))
#define XTEA_KEY1			(*(volatile uint32_t *)(XTEA_BASE + 0x020))
#define XTEA_KEY2			(*(volatile uint32_t *)(XTEA_BASE + 0x030))
#define XTEA_KEY3			(*(volatile uint32_t *)(XTEA_BASE + 0x040))
#define XTEA_IN0			(*(volatile uint32_t *)(XTEA_BASE + 0x050))
#define XTEA_IN1			(*(volatile uint32_t *)(XTEA_BASE + 0x060))
#define XTEA_OUT0			(*(volatile uint32_t *)(XTEA_BASE + 0x070))
#define XTEA_OUT1			(*(volatile uint32_t *)(XTEA_BASE + 0x080))

#include <libc.h>
