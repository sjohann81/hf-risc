library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_textio.all;
use ieee.std_logic_unsigned.all;

entity tb is
end tb;

architecture tb of tb is
	signal clock, reset, burst, bmode, hmode, wr, rd, data_ack, stall, spi_cs, spi_clk, spi_mosi, spi_miso, hold_n: std_logic := '0';
	signal addr_in: std_logic_vector(23 downto 0);
	signal data_in, data_out: std_logic_vector(31 downto 0);
begin

	reset <= '0', '1' after 5 ns, '0' after 500 ns;

	process						--25Mhz system clock
	begin
		clock <= not clock;
		wait for 20 ns;
		clock <= not clock;
		wait for 20 ns;
	end process;

	wr <= '0', '1' after 1000 ns, '0' after 1100 ns;
	rd <= '0', '1' after 15000 ns, '0' after 15200 ns;

	addr_in <= x"001000";
	data_in <= x"aa556699";
	burst <= '0';
	bmode <= '0';
	hmode <= '0';

	hold_n <= '1';

	sram_ctrl_core: entity work.spi_sram_ctrl
	port map(	clk_i => clock,
			rst_i => reset,
			addr_i => addr_in,
			data_i => data_in,
			data_o => data_out,
			burst_i => burst,
			bmode_i => bmode,
			hmode_i => hmode,
			wr_i => wr,
			rd_i => rd,
			data_ack_o => data_ack,
			cpu_stall_o => stall,
			spi_cs_n_o => spi_cs,
			spi_clk_o => spi_clk,
			spi_mosi_o => spi_mosi,
			spi_miso_i => spi_miso
	);


	spi_sram: entity work.M23LC1024
	port map(	SI_SIO0 => spi_mosi,
			SO_SIO1 => spi_miso,
			SCK => spi_clk,
			CS_N => spi_cs,
			SIO2 => open,
			HOLD_N_SIO3 => hold_n,
			RESET => reset
	);

end tb;
