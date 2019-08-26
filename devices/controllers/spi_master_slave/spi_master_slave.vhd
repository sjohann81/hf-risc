-- file:          spi_master_slave.vhd
-- description:   SPI master slave interface
-- date:          01/2019
-- author:        Sergio Johann Filho <sergio.filho@pucrs.br>
--
-- This is a simple SPI master / slave interface that works in SPI MODE 0.
-- Chip select logic is not included and selects the operating mode (spi_ssn_i <= '0'
-- for slave mode and spi_ssn_i <= '1' for master mode). The usual terminology of the
-- clock and data buses (SCK, MISO and MOSI) is changed because the interface can act
-- in both modes. So, the SPI clock bus works in either direction and MISO, MOSI data
-- buses are changed to DI and DO respectively.

library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_unsigned.all;
use ieee.std_logic_arith.all;

entity spi_master_slave is
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
		clk_div_i: in std_logic_vector(8 downto 0);		-- SPI clock divider
		-- SPI interface
		spi_ssn_i: in std_logic;				-- spi slave select negated input
		spi_clk_i: in std_logic;				-- spi slave clock input
		spi_clk_o: out std_logic;				-- spi master clock output
		spi_do_o: out std_logic;				-- spi mosi (master mode) or miso (slave mode)
		spi_di_i: in std_logic					-- spi miso (master mode) or mosi (slave mode)
	);
end spi_master_slave;

architecture spi_master_slave_arch of spi_master_slave is
	type states is (idle, data1, clock1, data2, clock2, sdata1, sdata2, done);
	signal state: states;
	signal data_reg: std_logic_vector(BYTE_SIZE-1 downto 0);
	signal clk_cnt: std_logic_vector(8 downto 0);
	signal counter: std_logic_vector(8 downto 0);
	signal fsm_trigger: std_logic;
begin
	process(clk_i, rst_i)
	begin
		if rst_i = '1' then
			clk_cnt <= (others => '0');
		elsif clk_i'event and clk_i = '1' then
			if (clk_cnt < clk_div_i) then
				clk_cnt <= clk_cnt + 1;
			else
				clk_cnt <= (others => '0');
			end if;
		end if;
	end process;

	fsm_trigger <= '1' when clk_cnt = "000000000" or spi_ssn_i = '0' else '0';

	process(clk_i, rst_i, spi_clk_i, fsm_trigger)
	begin
		if rst_i = '1' then
			data_reg <= (others => '0');
			counter <= (others => '0');
			data_valid_o <= '0';
			spi_clk_o <= '0';
			spi_do_o <= '0';
		elsif clk_i'event and clk_i = '1' then
			if (fsm_trigger = '1') then
				case state is
					when idle =>
						counter <= (others => '0');
						spi_clk_o <= '0';
						spi_do_o <= '0';
						data_valid_o <= '0';
						data_reg <= data_i;
					when data1 =>
						data_valid_o <= '0';
						spi_do_o <= data_reg(BYTE_SIZE-1);
					when clock1 =>
						spi_clk_o <= '1';
					when data2 =>
						data_reg <= data_reg(BYTE_SIZE-2 downto 0) & spi_di_i;
					when clock2 =>
						spi_clk_o <= '0';
						counter <= counter + 1;
					when sdata1 =>
						data_valid_o <= '0';
						spi_do_o <= data_reg(BYTE_SIZE-1);
					when sdata2 =>
						if (spi_clk_i = '0') then
							data_reg <= data_reg(BYTE_SIZE-2 downto 0) & spi_di_i;
							spi_do_o <= data_reg(BYTE_SIZE-1);
							counter <= counter + 1;
						end if;
					when done =>
						counter <= (others => '0');
						data_valid_o <= '1';
						spi_do_o <= '0';
					when others => null;
				end case;
			end if;
		end if;
	end process;

	data_o <= data_reg;

	process(clk_i, rst_i, state, counter, wren_i, spi_ssn_i, spi_clk_i, fsm_trigger)
	begin
		if rst_i = '1' then
			state <= idle;
		elsif clk_i'event and clk_i = '1' then
			if (fsm_trigger = '1') then
				case state is
					when idle =>
						if (spi_ssn_i = '1') then
							if (wren_i = '1') then
								state <= data1;
							end if;
						else
							state <= sdata1;
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
					when sdata1 =>
						if (spi_clk_i = '1') then
							state <= sdata2;
						end if;
					when sdata2 =>
						if (spi_clk_i = '0') then
							if (counter < BYTE_SIZE-1) then
								state <= sdata1;
							else
								state <= done;
							end if;
						end if;
					when done =>
						if (spi_ssn_i = '1') then
							if (wren_i = '0') then
								state <= idle;
							end if;
						else
							if (spi_clk_i = '1') then
								state <= sdata1;
							end if;
						end if;
					when others => null;
				end case;
			end if;
		end if;
	end process;

end spi_master_slave_arch;
