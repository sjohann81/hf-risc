library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_textio.all;
use ieee.std_logic_unsigned.all;
use std.textio.all;
use ieee.numeric_std.all;

entity tb is
	generic(
		address_width: integer := 14;
		memory_file : string := "code.txt";
		log_file: string := "out.txt";
		uart_support : string := "no"
	);
end tb;

architecture tb of tb is
	signal clock_in, reset, data, stall, stall_sig: std_logic := '0';
	signal uart_read, uart_write: std_logic;
	signal boot_enable_n, ram_enable_n, ram_dly: std_logic;
	signal address, data_read, data_write, data_read_boot, data_read_ram: std_logic_vector(31 downto 0);
	signal ext_irq: std_logic_vector(7 downto 0);
	signal data_we, data_w_n_ram: std_logic_vector(3 downto 0);

	signal periph, periph_dly, periph_wr, periph_irq: std_logic;
	signal data_read_periph, data_read_periph_s, data_write_periph: std_logic_vector(31 downto 0);
	signal gpioa_in, gpioa_out, gpioa_ddr: std_logic_vector(15 downto 0);
	signal gpiob_in, gpiob_out, gpiob_ddr: std_logic_vector(15 downto 0);
	signal gpio_sig, gpio_sig2, gpio_sig3: std_logic := '0';

	signal data_read_spi: std_logic_vector(31 downto 0);
	signal data_mode: std_logic_vector(2 downto 0);
	signal burst, wr, rd, we, stall_dly, stall_dly2, stall_spi, spi_sel, spi_cs, spi_cs2, spi_clk, spi_mosi, spi_miso, hold_n, spi_cs_n_s: std_logic := '0';

	signal sync_mac_out, sync_mac_in: std_logic_vector(31 downto 0) := x"12345678";
	signal sync_mac_en, sync_mac_rst: std_logic;
	signal async_mac_out, async_mac_in: std_logic_vector(31 downto 0) := x"87654321";
	signal async_mac_en, async_mac_rst: std_logic;
	signal de_pause: std_logic_vector(1 downto 0);
	signal de_config: std_logic_vector(4 downto 0);
	signal de_cde_sel, de_mde_sel_l, de_mde_sel_b: std_logic_vector(3 downto 0);
	signal de_cde_ctrl: std_logic_vector(15 downto 0);

begin

	process						--25Mhz system clock
	begin
		clock_in <= not clock_in;
		wait for 20 ns;
		clock_in <= not clock_in;
		wait for 20 ns;
	end process;

	process
	begin
		wait for 4 ms;
		gpio_sig <= not gpio_sig;
		gpio_sig2 <= not gpio_sig2;
		wait for 100 us;
		gpio_sig <= not gpio_sig;
		gpio_sig2 <= not gpio_sig2;
	end process;

	process
	begin
		wait for 5 ms;
		gpio_sig3 <= not gpio_sig3;
		wait for 5 ms;
		gpio_sig3 <= not gpio_sig3;
	end process;

	gpioa_in <= x"00" & "0000" & gpio_sig & "000";
	gpiob_in <= "10000" & gpio_sig3 & "00" & "00000" & gpio_sig2 & "00";

	process
	begin
		stall <= not stall;
		wait for 123 ns;
		stall <= not stall;
		wait for 123 ns;
	end process;

	reset <= '0', '1' after 5 ns, '0' after 500 ns;
	ext_irq <= "0000000" & periph_irq;

	boot_enable_n <= '0' when (address(31 downto 28) = "0000" and stall_sig = '0') or reset = '1' else '1';
	ram_enable_n <= '0' when (address(31 downto 28) = "0100" and stall_sig = '0') or reset = '1' else '1';
	spi_sel <= '1' when address(31 downto 28) = "0011" else '0';
	rd <= '1' when (spi_sel = '1' and data_we = "0000" and stall_dly2 = '0') else '0';
	wr <= '1' when (spi_sel = '1' and data_we /= "0000" and stall_dly2 = '0') else '0';
	data_read <= data_read_periph when periph = '1' or periph_dly = '1' else data_read_spi when spi_sel = '1' or stall_dly2 = '1' else
			data_read_boot when address(31 downto 28) = "0000" and ram_dly = '0' else data_read_ram;
	data_w_n_ram <= not data_we;
	hold_n <= '1';
	burst <= '0';
	stall_sig <= stall_spi;
	-- external SPI SRAM/EEPROM, 0x30000000 (26,25 - spi select, 24 - short address mode, 23 - EEPROM write enable latch)
	spi_cs <= spi_cs_n_s when spi_sel = '1' and address(25) = '0' else '1';
	-- external SPI SRAM/EEPROM, 0x32000000
	spi_cs2 <= spi_cs_n_s when spi_sel = '1' and address(25) = '1' else '1';
	we <= address(24) and address(23);

	process(clock_in, reset, stall_spi)
	begin
		if reset = '1' then
			ram_dly <= '0';
			periph_dly <= '0';
			stall_dly <= '0';
			stall_dly2 <= '0';
		elsif clock_in'event and clock_in = '1' then
			ram_dly <= not ram_enable_n;
			periph_dly <= periph;
			stall_dly <= stall_spi;
			stall_dly2 <= stall_dly;
		end if;
	end process;

	-- HF-RISCV core
	processor: entity work.processor
	port map(	clk_i => clock_in,
			rst_i => reset,
			stall_i => stall_sig,
			addr_o => address,
			data_i => data_read,
			data_o => data_write,
			data_w_o => data_we,
			data_mode_o => data_mode,
			extio_in => ext_irq,
			extio_out => open
	);

	data_read_periph <= data_read_periph_s(7 downto 0) & data_read_periph_s(15 downto 8) & data_read_periph_s(23 downto 16) & data_read_periph_s(31 downto 24);
	data_write_periph <= data_write(7 downto 0) & data_write(15 downto 8) & data_write(23 downto 16) & data_write(31 downto 24);
	periph_wr <= '1' when data_we /= "0000" else '0';
	periph <= '1' when address(31 downto 28) = x"e" else '0';

	peripherals: entity work.peripherals
	port map(
		clk_i => clock_in,
		rst_i => reset,
		addr_i => address,
		data_i => data_write_periph,
		data_o => data_read_periph_s,
		sel_i => periph,
		wr_i => periph_wr,
		irq_o => periph_irq,
		gpioa_in => gpioa_in,
		gpioa_out => gpioa_out,
		gpioa_ddr => gpioa_ddr,
		gpiob_in => gpiob_in,
		gpiob_out => gpiob_out,
		gpiob_ddr => gpiob_ddr,
		sync_mac_i => sync_mac_out,
		sync_mac_o => sync_mac_in,
		sync_mac_en_o => sync_mac_en,
		sync_mac_rst_o => sync_mac_rst,
		async_mac_i => async_mac_out,
		async_mac_o => async_mac_in,
		async_mac_en_o => async_mac_en,
		async_mac_rst_o => async_mac_rst,
		de_pause_o => de_pause,
		de_config_o => de_config,
		de_cde_sel_o => de_cde_sel,
		de_mde_sel_l_o => de_mde_sel_l,
		de_mde_sel_b_o => de_mde_sel_b,
		de_cde_ctrl_o => de_cde_ctrl
	);

	sram_ctrl_core: entity work.spi_sram_ctrl
	port map(	clk_i => clock_in,
			rst_i => reset,
			addr_i => address(23 downto 0),
			data_i => data_write,
			data_o => data_read_spi,
			burst_i => burst,
			bmode_i => data_mode(2),
			hmode_i => data_mode(1),
			wr_i => wr,
			rd_i => rd,
			saddr_i => address(24),
			wren_i => we,
			data_ack_o => open,
			cpu_stall_o => stall_spi,
			spi_cs_n_o => spi_cs_n_s,
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

	spi_eeprom: entity work.M25LC256
	port map(	SI => spi_mosi,
			SO => spi_miso,
			SCK => spi_clk,
			CS_N => spi_cs2,
			WP_N => hold_n,
			HOLD_N => hold_n,
			RESET => reset
	);

	-- boot ROM
	boot0lb: entity work.boot_ram
	generic map (	memory_file => "boot.txt",
					data_width => 8,
					address_width => 12,
					bank => 0)
	port map(
		clk 	=> clock_in,
		addr 	=> address(11 downto 2),
		cs_n 	=> boot_enable_n,
		we_n	=> '1',
		data_i	=> (others => '0'),
		data_o	=> data_read_boot(7 downto 0)
	);

	boot0ub: entity work.boot_ram
	generic map (	memory_file => "boot.txt",
					data_width => 8,
					address_width => 12,
					bank => 1)
	port map(
		clk 	=> clock_in,
		addr 	=> address(11 downto 2),
		cs_n 	=> boot_enable_n,
		we_n	=> '1',
		data_i	=> (others => '0'),
		data_o	=> data_read_boot(15 downto 8)
	);

	boot1lb: entity work.boot_ram
	generic map (	memory_file => "boot.txt",
					data_width => 8,
					address_width => 12,
					bank => 2)
	port map(
		clk 	=> clock_in,
		addr 	=> address(11 downto 2),
		cs_n 	=> boot_enable_n,
		we_n	=> '1',
		data_i	=> (others => '0'),
		data_o	=> data_read_boot(23 downto 16)
	);

	boot1ub: entity work.boot_ram
	generic map (	memory_file => "boot.txt",
					data_width => 8,
					address_width => 12,
					bank => 3)
	port map(
		clk 	=> clock_in,
		addr 	=> address(11 downto 2),
		cs_n 	=> boot_enable_n,
		we_n	=> '1',
		data_i	=> (others => '0'),
		data_o	=> data_read_boot(31 downto 24)
	);

	-- RAM
	memory0lb: entity work.bram
	generic map (	memory_file => memory_file,
					data_width => 8,
					address_width => address_width,
					bank => 0)
	port map(
		clk 	=> clock_in,
		addr 	=> address(address_width -1 downto 2),
		cs_n 	=> ram_enable_n,
		we_n	=> data_w_n_ram(0),
		data_i	=> data_write(7 downto 0),
		data_o	=> data_read_ram(7 downto 0)
	);

	memory0ub: entity work.bram
	generic map (	memory_file => memory_file,
					data_width => 8,
					address_width => address_width,
					bank => 1)
	port map(
		clk 	=> clock_in,
		addr 	=> address(address_width -1 downto 2),
		cs_n 	=> ram_enable_n,
		we_n	=> data_w_n_ram(1),
		data_i	=> data_write(15 downto 8),
		data_o	=> data_read_ram(15 downto 8)
	);

	memory1lb: entity work.bram
	generic map (	memory_file => memory_file,
					data_width => 8,
					address_width => address_width,
					bank => 2)
	port map(
		clk 	=> clock_in,
		addr 	=> address(address_width -1 downto 2),
		cs_n 	=> ram_enable_n,
		we_n	=> data_w_n_ram(2),
		data_i	=> data_write(23 downto 16),
		data_o	=> data_read_ram(23 downto 16)
	);

	memory1ub: entity work.bram
	generic map (	memory_file => memory_file,
					data_width => 8,
					address_width => address_width,
					bank => 3)
	port map(
		clk 	=> clock_in,
		addr 	=> address(address_width -1 downto 2),
		cs_n 	=> ram_enable_n,
		we_n	=> data_w_n_ram(3),
		data_i	=> data_write(31 downto 24),
		data_o	=> data_read_ram(31 downto 24)
	);

	-- debug process
	debug:
	if uart_support = "no" generate
		process(clock_in, address)
			file store_file : text open write_mode is "debug.txt";
			variable hex_file_line : line;
			variable c : character;
			variable index : natural;
			variable line_length : natural := 0;
		begin
			if clock_in'event and clock_in = '1' then
				if address = x"f00000d0" and data = '0' then
					data <= '1';
					index := conv_integer(data_write(30 downto 24));
					if index /= 10 then
						c := character'val(index);
						write(hex_file_line, c);
						line_length := line_length + 1;
					end if;
					if index = 10 or line_length >= 72 then
						writeline(store_file, hex_file_line);
						line_length := 0;
					end if;
				else
					data <= '0';
				end if;
			end if;
		end process;
	end generate;

	process(clock_in, reset, address)
	begin
		if reset = '1' then
		elsif clock_in'event and clock_in = '0' then
			assert address /= x"e0000000" report "end of simulation" severity failure;
			assert (address < x"70000000") or (address >= x"e0000000") report "out of memory region" severity failure;
			assert address /= x"40000104" report "handling IRQ" severity warning;
		end if;
	end process;

end tb;

