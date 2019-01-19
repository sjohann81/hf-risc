library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_textio.all;
use ieee.std_logic_unsigned.all;

entity tb is
end tb;

architecture tb of tb is
	signal clock, reset, we, valid, spi_clk, spi_mosi, spi_miso: std_logic := '0';
	signal input, output: std_logic_vector(7 downto 0);
begin

	reset <= '0', '1' after 5 ns, '0' after 500 ns;

	process						--25Mhz system clock
	begin
		clock <= not clock;
		wait for 20 ns;
		clock <= not clock;
		wait for 20 ns;
	end process;

	we <= '0', '1' after 1000 ns, '0' after 1100 ns;
	spi_miso <= '1';
	input <= x"a1";

	spi_core: entity work.spi_master
	generic map(
		BYTE_SIZE => 8
	)
	port map(	clk_i => clock,
			rst_i => reset,
			data_i => input,
			data_o => output,
			data_valid_o => valid,
			wren_i => we,
			spi_clk_o => spi_clk,
			spi_mosi_o => spi_mosi,
			spi_miso_i => spi_miso
	);

end tb;
