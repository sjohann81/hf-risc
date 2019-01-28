/* file:          mcp23s17.c
 * description:   Microchip mcp23s17 chip driver
 * date:          09/2015
 * author:        Sergio Johann Filho <sergio.filho@pucrs.br>
 */

#include <hf-risc.h>
#include <mcp23s17.h>

void mcp23s17_dir(uint8_t device, uint8_t bank, uint8_t reg){
	spi_start();
	spi_transfer(MCP23x17_ADDR | ((device & 0x07) << 1));
	if (bank)
		spi_transfer(MCP23x17_IODIRB);
	else
		spi_transfer(MCP23x17_IODIRA);
	spi_transfer(reg);
	spi_stop();
}

uint8_t mcp23s17_read(uint8_t device, uint8_t bank){
	uint8_t reg;

	spi_start();
	spi_transfer(MCP23x17_ADDR | ((device & 0x07) << 1) | 0x01);
	if (bank)
		spi_transfer(MCP23x17_GPIOB);
	else
		spi_transfer(MCP23x17_GPIOA);
	reg = spi_transfer(0);
	spi_stop();

	return reg;
}

void mcp23s17_write(uint8_t device, uint8_t bank, uint8_t reg){
	spi_start();
	spi_transfer(MCP23x17_ADDR | ((device & 0x07) << 1));
	if (bank)
		spi_transfer(MCP23x17_GPIOB);
	else
		spi_transfer(MCP23x17_GPIOA);
	spi_transfer(reg);
	spi_stop();
}

void mcp23s17_inten(uint8_t device, uint8_t bank, uint8_t reg){
	spi_start();
	spi_transfer(MCP23x17_ADDR | ((device & 0x07) << 1));
	if (bank)
		spi_transfer(MCP23x17_GPINTENB);
	else
		spi_transfer(MCP23x17_GPINTENA);
	spi_transfer(reg);
	spi_stop();
}

void mcp23s17_init(uint8_t device, uint8_t portdir_a, uint8_t portdir_b){
	uint8_t byte;
	
	spi_setup();

	// configure device, enable device addressing
	spi_start();
	spi_transfer(MCP23x17_ADDR | ((device & 0x07) << 1));
	spi_transfer(MCP23x17_IOCON);
	spi_transfer(0x00
			| (0 << MCP23x17_IOCON_BANK)
			| (0 << MCP23x17_IOCON_MIRROR)
			| (0 << MCP23x17_IOCON_SEQOP)
			| (0 << MCP23x17_IOCON_DISSLW)
			| (1 << MCP23x17_IOCON_HAEN)
			| (0 << MCP23x17_IOCON_ODR)
			| (1 << MCP23x17_IOCON_INTPOL)
	);
	spi_stop();

	// set pin direction on both ports
	mcp23s17_dir(device, 0, portdir_a);
	mcp23s17_dir(device, 1, portdir_b);
	mcp23s17_write(device, 0, 0x00);
	mcp23s17_write(device, 1, 0x00);

	// enable weak internal pull-ups on both ports
	spi_start();
	spi_transfer(MCP23x17_ADDR | ((device & 0x07) << 1));
	spi_transfer(MCP23x17_GPPUA);
	spi_transfer(0xff);
	spi_transfer(0xff);
	spi_stop();
}
