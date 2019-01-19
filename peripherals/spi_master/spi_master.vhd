-- file:          spi_master.vhd
-- description:   SPI master interface
-- date:          03/2018
-- author:        Sergio Johann Filho <sergio.filho@pucrs.br>
--
-- This is a simple SPI master interface that works in SPI MODE 0.
-- Chip select logic is not included in this design and should be
-- implemented externally.

library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_unsigned.all;
use ieee.std_logic_arith.all;

entity spi_master is
	generic (
		BYTE_SIZE: integer := 8
	);
	port (	-- core interface
		clk_i: in std_logic;
		rst_i: in std_logic;
		data_i: in std_logic_vector(BYTE_SIZE-1 downto 0);	-- parallel data in (clocked on rising spi_clk after last bit)
		data_o: out std_logic_vector(BYTE_SIZE-1 downto 0);	-- parallel data output (clocked on rising spi_clk after last bit)
		data_valid_o: out std_logic;				-- data valid (read / write finished)
		wren_i: in std_logic;					-- data write enable, starts transmission when interface is idle
		-- SPI interface
		spi_clk_o: out std_logic;				-- spi bus sck
		spi_mosi_o: out std_logic;				-- spi bus mosi output
		spi_miso_i: in std_logic				-- spi bus miso input
	);
end spi_master;

architecture spi_master_arch of spi_master is
	type states is (idle, data1, clock1, data2, clock2, done);
	signal state: states;
	signal data_reg: std_logic_vector(BYTE_SIZE-1 downto 0);
	signal counter: std_logic_vector(BYTE_SIZE-1 downto 0);
begin
	process(clk_i, rst_i)
	begin
		if rst_i = '1' then
			data_reg <= (others => '0');
			counter <= (others => '0');
			data_valid_o <= '0';
			spi_clk_o <= '0';
			spi_mosi_o <= '0';
		elsif clk_i'event and clk_i = '1' then
			case state is
				when idle =>
					counter <= (others => '0');
					spi_clk_o <= '0';
					spi_mosi_o <= '0';
					data_valid_o <= '0';
					data_reg <= data_i;
				when data1 =>
					data_valid_o <= '0';
					spi_mosi_o <= data_reg(BYTE_SIZE-1);
				when clock1 =>
					spi_clk_o <= '1';
				when data2 =>
					data_reg <= data_reg(BYTE_SIZE-2 downto 0) & spi_miso_i;
				when clock2 =>
					spi_clk_o <= '0';
					counter <= counter + 1;
				when done =>
					counter <= (others => '0');
					data_valid_o <= '1';
					spi_mosi_o <= '0';
				when others => null;
			end case;
		end if;
	end process;

	data_o <= data_reg;

	process(clk_i, rst_i, state, counter, wren_i)
	begin
		if rst_i = '1' then
			state <= idle;
		elsif clk_i'event and clk_i = '1' then
			case state is
				when idle =>
					if (wren_i = '1') then
						state <= data1;
					end if;
				when data1 =>
					state <= clock1;
				when clock1 =>
					state <= data2;
				when data2 =>
					state <= clock2;
				when clock2 =>
					if (counter < BYTE_SIZE-1) then
						state <= data1;
					else
						state <= done;
					end if;
				when done =>
					if (wren_i = '0') then
						state <= idle;
					end if;
				when others => null;
			end case;
		end if;
	end process;

end spi_master_arch;
