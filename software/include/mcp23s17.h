/* file:          mcp23s17.h
 * description:   Microchip mcp23s17 chip driver
 * date:          09/2015
 * author:        Sergio Johann Filho <sergio.filho@pucrs.br>
 */

#define MCP23x17_ADDR		0x40
#define ALL_OUTPUTS		0x00
#define ALL_INPUTS		0xff

#define MCP23x17_IODIRA		0x00
#define MCP23x17_IODIRB		0x01
#define MCP23x17_IOPOLA		0x02
#define MCP23x17_IOPOLB		0x03
#define MCP23x17_GPINTENA	0x04
#define MCP23x17_GPINTENB	0x05
#define MCP23x17_DEFVALA	0x06
#define MCP23x17_DEFVALB	0x07
#define MCP23x17_INTCONA	0x08
#define MCP23x17_INTCONB	0x09
#define MCP23x17_IOCON		0x0A
#define MCP23x17_GPPUA		0x0C
#define MCP23x17_GPPUB		0x0D
#define MCP23x17_INTFA		0x0E
#define MCP23x17_INTFB		0x0F
#define MCP23x17_INTCAPA	0x10
#define MCP23x17_INTCAPB	0x11
#define MCP23x17_GPIOA		0x12
#define MCP23x17_GPIOB		0x13
#define MCP23x17_OPLATA		0x14
#define MCP23x17_OPLATB		0x15

#define MCP23x17_IOCON_BANK	7
#define MCP23x17_IOCON_MIRROR	6
#define MCP23x17_IOCON_SEQOP	5
#define MCP23x17_IOCON_DISSLW	4
#define MCP23x17_IOCON_HAEN	3
#define MCP23x17_IOCON_ODR	2
#define MCP23x17_IOCON_INTPOL	1

void mcp23s17_dir(uint8_t device, uint8_t bank, uint8_t reg);
uint8_t mcp23s17_read(uint8_t device, uint8_t bank);
void mcp23s17_write(uint8_t device, uint8_t bank, uint8_t reg);
void mcp23s17_inten(uint8_t device, uint8_t bank, uint8_t reg);
void mcp23s17_init(uint8_t device, uint8_t portdir_a, uint8_t portdir_b);

