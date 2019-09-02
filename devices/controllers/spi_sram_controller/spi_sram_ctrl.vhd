-- file:          spi_sram_ctrl.vhd
-- description:   Microchip SPI SRAM and EEPROM chip controller
-- date:          03/2018
-- author:        Sergio Johann Filho <sergio.filho@pucrs.br>
--
-- A memory controller compatible with the 23lc1024 and similar SRAM chips.
-- For the 23lc512 chip, only two address bytes should be transmitted, and
-- this controller needs some small changes. The controller assumes that
-- the chip is configured to burst (sequential) data mode by default (which
-- is not true for the 23x256 family of chips.
-- In short address mode (sddr_i = '1'), this controller is also compatible with
-- EEPRROMs, which operate in burst mode by default but use the 16 bit address
-- format. Warning: this controller doesn't use page writes, so EEPROMs may
-- wear out faster than normal. To avoid this, hook the write protection pin.
-- Traffic to / from SRAM / EEPROM is performed in 32 bit words (4 SPI 8 bit words)
-- or multiple (burst) of 32 bit words. For single word access, a half word or
-- byte can be individually accessed. In this case burst mode must be deselected.

library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_unsigned.all;
use ieee.std_logic_arith.all;

entity spi_sram_ctrl is
	generic (
		BURST_SIZE: integer := 4
	);
	port (	-- core interface
		clk_i: in std_logic;
		rst_i: in std_logic;
		addr_i: in std_logic_vector(23 downto 0);
		data_i: in std_logic_vector(31 downto 0);
		data_o: out std_logic_vector(31 downto 0);
		burst_i: in std_logic;					-- data in burst mode
		bmode_i: in std_logic;					-- byte access
		hmode_i: in std_logic;					-- half word access
		wr_i: in std_logic;
		rd_i: in std_logic;
		saddr_i: in std_logic;					-- 16 bit short address select
		wren_i: in std_logic;					-- EEPROM write enable latch
		data_ack_o: out std_logic;				-- signals the last byte of a word
		cpu_stall_o: out std_logic;
		-- SPI interface
		spi_cs_n_o: out std_logic;
		spi_clk_o: out std_logic;				-- spi bus sck
		spi_mosi_o: out std_logic;				-- spi bus mosi output
		spi_miso_i: in std_logic				-- spi bus miso input
	);
end spi_sram_ctrl;

architecture spi_sram_ctrl_arch of spi_sram_ctrl is
	type states is (start, cmd_wren, cmd_write, cmd_read, addr_phase, data_phase, ready);
	signal state: states;
	signal data_o_reg: std_logic_vector(31 downto 0);
	signal cmd_counter: std_logic;
	signal addr_counter: std_logic_vector(1 downto 0);
	signal data_counter: std_logic_vector(2**BURST_SIZE downto 0);
	signal data_in, data_out: std_logic_vector(7 downto 0);
	signal wren, data_valid, bmode, hmode, cpu_stall: std_logic;

begin
	spi_core: entity work.spi_master
	generic map(
		BYTE_SIZE => 8
	)
	port map(	clk_i => clk_i,
			rst_i => rst_i,
			data_i => data_in,
			data_o => data_out,
			wren_i => wren,
			data_valid_o => data_valid,
			spi_clk_o => spi_clk_o,
			spi_mosi_o => spi_mosi_o,
			spi_miso_i => spi_miso_i
	);

	data_o <= data_o_reg;
	cpu_stall_o <= cpu_stall or wr_i or rd_i;

	process(clk_i, rst_i, data_valid)
	begin
		if rst_i = '1' then
			data_o_reg <= (others => '0');
			cmd_counter <= '0';
			addr_counter <= (others => '0');
			data_counter <= (others => '0');
			data_in <= (others => '0');
			wren <= '0';
			spi_cs_n_o <= '1';
			data_ack_o <= '0';
			cpu_stall <= '0';
			bmode <= '0';
			hmode <= '0';
		elsif clk_i'event and clk_i = '1' then
			case state is
				when start =>
					cmd_counter <= '0';
					if (saddr_i = '0') then
						addr_counter <= "00";
					else
						addr_counter <= "01";
					end if;

					data_o_reg <= (others => '0');
					if (hmode_i = '1' and bmode_i = '0' and burst_i = '0') then
						data_counter(1 downto 0) <= "10";
						data_counter(2**BURST_SIZE downto 2) <= (others => '0');
						bmode <= '0';
						hmode <= '1';
					elsif (hmode_i = '0' and bmode_i = '1' and burst_i = '0') then
						data_counter(1 downto 0) <= "11";
						data_counter(2**BURST_SIZE downto 2) <= (others => '0');
						bmode <= '1';
						hmode <= '0';
					else
						data_counter <= (others => '0');
						bmode <= '0';
						hmode <= '0';
					end if;
					wren <= '0';
					spi_cs_n_o <= '0';
					data_ack_o <= '0';
					cpu_stall <= '1';
				when cmd_wren =>
					if (data_valid = '0') then
						data_in <= x"06";
						wren <= '1';
					else
						cmd_counter <= '1';
						wren <= '0';
					end if;
				when cmd_write =>
					if (data_valid = '0') then
						data_in <= x"02";
						wren <= '1';
					else
						cmd_counter <= '1';
						wren <= '0';
					end if;
				when cmd_read =>
					if (data_valid = '0') then
						data_in <= x"03";
						wren <= '1';
					else
						cmd_counter <= '1';
						wren <= '0';
					end if;
				when addr_phase =>
					if (data_valid = '0') then
						case addr_counter is
							when "00" => data_in <= addr_i(23 downto 16);
							when "01" => data_in <= addr_i(15 downto 8);
							when "10" => data_in <= addr_i(7 downto 0);
							when others => null;
						end case;
						wren <= '1';
					else
						if (wren = '1') then
							addr_counter <= addr_counter + 1;
						end if;
						wren <= '0';
					end if;
				when data_phase =>
					if (data_valid = '0') then
						case data_counter(1 downto 0) is
							when "00" => 	data_in <= data_i(31 downto 24);
									data_o_reg(31 downto 24) <= data_out;
									data_ack_o <= '0';
							when "01" =>	data_in <= data_i(23 downto 16);
									data_o_reg(23 downto 16) <= data_out;
							when "10" => 	data_in <= data_i(15 downto 8);
									data_o_reg(15 downto 8) <= data_out;
							when "11" => 	data_in <= data_i(7 downto 0);
									if bmode = '1' then
										data_o_reg <= data_out & data_out & data_out & data_out;
									elsif hmode = '1' then
										data_o_reg <= data_o_reg(15 downto 8) & data_out & data_o_reg(15 downto 8) & data_out;
									else
										data_o_reg(7 downto 0) <= data_out;
									end if;
									data_ack_o <= '1';
							when others => null;
						end case;
						wren <= '1';
					else
						if (wren = '1') then
							data_counter <= data_counter + 1;
						end if;
						wren <= '0';
					end if;
				when ready =>
					spi_cs_n_o <= '1';
					cpu_stall <= '0';
				when others => null;
			end case;
		end if;
	end process;

	process(clk_i, rst_i, state, cmd_counter, addr_counter, data_counter, wr_i, rd_i)
	begin
		if rst_i = '1' then
			state <= ready;
		elsif clk_i'event and clk_i = '1' then
			case state is
				when start =>
					if (wr_i = '1') then
						if (wren_i = '1') then
							state <= cmd_wren;
						else
							state <= cmd_write;
						end if;
					elsif (rd_i = '1') then
						state <= cmd_read;
					else
						state <= start;
					end if;
				when cmd_wren =>
					if (cmd_counter = '1') then
						state <= ready;
					else
						state <= cmd_wren;
					end if;
				when cmd_write =>
					if (cmd_counter = '1') then
						state <= addr_phase;
					else
						state <= cmd_write;
					end if;
				when cmd_read =>
					if (cmd_counter = '1') then
						state <= addr_phase;
					else
						state <= cmd_read;
					end if;
				when addr_phase =>
					if (addr_counter < 3) then
						state <= addr_phase;
					else
						state <= data_phase;
					end if;
				when data_phase =>
					if (burst_i = '1') then
						if (data_counter < (4 * BURST_SIZE)) then
							state <= data_phase;
						else
							state <= ready;
						end if;
					else
						if (data_counter < 4) then
							state <= data_phase;
						else
							state <= ready;
						end if;
					end if;
				when ready =>
					if (wr_i = '1' or rd_i = '1') then
						state <= start;
					else
						state <= ready;
					end if;
				when others => null;
			end case;
		end if;
	end process;

end spi_sram_ctrl_arch;
