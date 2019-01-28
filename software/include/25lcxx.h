uint8_t spi_eeprom_readbyte(uint16_t addr);
void spi_eeprom_read(uint16_t addr, uint8_t *buf, uint16_t size);
void spi_eeprom_writepage(uint16_t page, uint8_t *data);
