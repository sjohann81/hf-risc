library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_unsigned.all;

entity hfrisc_soc is
	generic(
		address_width: integer := 15;
		memory_file : string := "code.txt"
	);
	port (	clk_i:		in std_logic;
		rst_i:		in std_logic;

		gpioa:		inout std_logic_vector(15 downto 0);
		gpiob:		inout std_logic_vector(15 downto 0);
		
		uart_tx:	out std_logic;
		uart_rx:	in std_logic
	);
end hfrisc_soc;

architecture top_level of hfrisc_soc is
	signal clock, boot_enable, ram_enable_n, stall, ram_dly, rff1, reset: std_logic;
	signal address, data_read, data_write, data_read_boot, data_read_ram: std_logic_vector(31 downto 0);
	signal ext_irq: std_logic_vector(7 downto 0);
	signal data_we, data_w_n_ram: std_logic_vector(3 downto 0);

	signal periph, periph_dly, periph_wr, periph_irq: std_logic;
	signal data_read_periph, data_read_periph_s, data_write_periph: std_logic_vector(31 downto 0);
	
	signal gpioa_in, gpioa_out, gpioa_ddr: std_logic_vector(15 downto 0);
	signal gpiob_in, gpiob_out, gpiob_ddr: std_logic_vector(15 downto 0);
begin
	-- clock divider (50MHz clock from 100MHz main clock for Nexys A7 kit)
	process (rst_i, clk_i, clock)
	begin
		if rst_i = '1' then
			clock <= '0';
		else
			if clk_i'event and clk_i = '1' then
				clock <= not clock;
			end if;
		end if;
	end process;

	-- reset synchronizer
	process (clock, rst_i)
	begin
		if (rst_i = '1') then
			rff1 <= '1';
			reset <= '1';
		elsif (clock'event and clock = '1') then
			rff1 <= '0';
			reset <= rff1;
		end if;
	end process;


	process (reset, clock, ext_irq, ram_enable_n)
	begin
		if reset = '1' then
			ram_dly <= '0';
			periph_dly <= '0';
		elsif clock'event and clock = '1' then
			ram_dly <= not ram_enable_n;
			periph_dly <= periph;
		end if;
	end process;
	
	-- GPIO A
	--
	-- JA (inputs)
	gpioa(0) <= gpioa_out(0) when gpioa_ddr(0) = '1' else 'Z';
	gpioa(1) <= gpioa_out(1) when gpioa_ddr(1) = '1' else 'Z';
	gpioa(2) <= gpioa_out(2) when gpioa_ddr(2) = '1' else 'Z';
	gpioa(3) <= gpioa_out(3) when gpioa_ddr(3) = '1' else 'Z';
	gpioa(4) <= gpioa_out(4) when gpioa_ddr(4) = '1' else 'Z';
	gpioa(5) <= gpioa_out(5) when gpioa_ddr(5) = '1' else 'Z';
	gpioa(6) <= gpioa_out(6) when gpioa_ddr(6) = '1' else 'Z';
	gpioa(7) <= gpioa_out(7) when gpioa_ddr(7) = '1' else 'Z';
	
	-- JB (inputs)
	gpioa(8) <= gpioa_out(8) when gpioa_ddr(8) = '1' else 'Z';
	gpioa(9) <= gpioa_out(9) when gpioa_ddr(9) = '1' else 'Z';
	gpioa(10) <= gpioa_out(10) when gpioa_ddr(10) = '1' else 'Z';
	gpioa(11) <= gpioa_out(11) when gpioa_ddr(11) = '1' else 'Z';
	gpioa(12) <= gpioa_out(12) when gpioa_ddr(12) = '1' else 'Z';
	gpioa(13) <= gpioa_out(13) when gpioa_ddr(13) = '1' else 'Z';
	gpioa(14) <= gpioa_out(14) when gpioa_ddr(14) = '1' else 'Z';
	gpioa(15) <= gpioa_out(15) when gpioa_ddr(15) = '1' else 'Z';
	
	-- JA (outputs)
	gpioa_in(0) <= '0' when gpioa_ddr(0) = '1' else gpioa(0);
	gpioa_in(1) <= '0' when gpioa_ddr(1) = '1' else gpioa(1);
	gpioa_in(2) <= '0' when gpioa_ddr(2) = '1' else gpioa(2);
--	gpioa_in(3) <= '0' when gpioa_ddr(3) = '1' else gpioa(3);
	gpioa_in(4) <= '0' when gpioa_ddr(4) = '1' else gpioa(4);
	gpioa_in(5) <= '0' when gpioa_ddr(5) = '1' else gpioa(5);
	gpioa_in(6) <= '0' when gpioa_ddr(6) = '1' else gpioa(6);
	gpioa_in(7) <= '0' when gpioa_ddr(7) = '1' else gpioa(7);
	
	-- JB (outputs)
	gpioa_in(8) <= '0' when gpioa_ddr(8) = '1' else gpioa(8);
	gpioa_in(9) <= '0' when gpioa_ddr(9) = '1' else gpioa(9);
	gpioa_in(10) <= '0' when gpioa_ddr(10) = '1' else gpioa(10);
	gpioa_in(11) <= '0' when gpioa_ddr(11) = '1' else gpioa(11);
	gpioa_in(12) <= '0' when gpioa_ddr(12) = '1' else gpioa(12);
	gpioa_in(13) <= '0' when gpioa_ddr(13) = '1' else gpioa(13);
	gpioa_in(14) <= '0' when gpioa_ddr(14) = '1' else gpioa(14);
	gpioa_in(15) <= '0' when gpioa_ddr(15) = '1' else gpioa(15);
	
	-- UART
	uart_tx <= gpioa_out(2);
	gpioa_in(3) <= uart_rx;
	
	
	-- GPIO B
	--
	-- JC (inputs)
	gpiob(0) <= gpiob_out(0) when gpiob_ddr(0) = '1' else 'Z';
	gpiob(1) <= gpiob_out(1) when gpiob_ddr(1) = '1' else 'Z';
	gpiob(2) <= gpiob_out(2) when gpiob_ddr(2) = '1' else 'Z';
	gpiob(3) <= gpiob_out(3) when gpiob_ddr(3) = '1' else 'Z';
	gpiob(4) <= gpiob_out(4) when gpiob_ddr(4) = '1' else 'Z';
	gpiob(5) <= gpiob_out(5) when gpiob_ddr(5) = '1' else 'Z';
	gpiob(6) <= gpiob_out(6) when gpiob_ddr(6) = '1' else 'Z';
	gpiob(7) <= gpiob_out(7) when gpiob_ddr(7) = '1' else 'Z';
	
	-- JD (inputs)
	gpiob(8) <= gpiob_out(8) when gpiob_ddr(8) = '1' else 'Z';
	gpiob(9) <= gpiob_out(9) when gpiob_ddr(9) = '1' else 'Z';
	gpiob(10) <= gpiob_out(10) when gpiob_ddr(10) = '1' else 'Z';
	gpiob(11) <= gpiob_out(11) when gpiob_ddr(11) = '1' else 'Z';
	gpiob(12) <= gpiob_out(12) when gpiob_ddr(12) = '1' else 'Z';
	gpiob(13) <= gpiob_out(13) when gpiob_ddr(13) = '1' else 'Z';
	gpiob(14) <= gpiob_out(14) when gpiob_ddr(14) = '1' else 'Z';
	gpiob(15) <= gpiob_out(15) when gpiob_ddr(15) = '1' else 'Z';
	
	-- JC (outputs)
	gpiob_in(0) <= '0' when gpiob_ddr(0) = '1' else gpiob(0);
	gpiob_in(1) <= '0' when gpiob_ddr(1) = '1' else gpiob(1);
	gpiob_in(2) <= '0' when gpiob_ddr(2) = '1' else gpiob(2);
	gpiob_in(3) <= '0' when gpiob_ddr(3) = '1' else gpiob(3);
	gpiob_in(4) <= '0' when gpiob_ddr(4) = '1' else gpiob(4);
	gpiob_in(5) <= '0' when gpiob_ddr(5) = '1' else gpiob(5);
	gpiob_in(6) <= '0' when gpiob_ddr(6) = '1' else gpiob(6);
	gpiob_in(7) <= '0' when gpiob_ddr(7) = '1' else gpiob(7);
	
	-- JD (outputs)
	gpiob_in(8) <= '0' when gpiob_ddr(8) = '1' else gpiob(8);
	gpiob_in(9) <= '0' when gpiob_ddr(9) = '1' else gpiob(9);
	gpiob_in(10) <= '0' when gpiob_ddr(10) = '1' else gpiob(10);
	gpiob_in(11) <= '0' when gpiob_ddr(11) = '1' else gpiob(11);
	gpiob_in(12) <= '0' when gpiob_ddr(12) = '1' else gpiob(12);
	gpiob_in(13) <= '0' when gpiob_ddr(13) = '1' else gpiob(13);
	gpiob_in(14) <= '0' when gpiob_ddr(14) = '1' else gpiob(14);
	gpiob_in(15) <= '0' when gpiob_ddr(15) = '1' else gpiob(15);
	

	stall <= '0';
	boot_enable <= '1' when address(31 downto 28) = "0000" else '0';
	ram_enable_n <= '0' when address(31 downto 28) = "0100" else '1';
	data_read <= data_read_periph when periph = '1' or periph_dly = '1' else data_read_boot when address(31 downto 28) = "0000" and ram_dly = '0' else data_read_ram;
	data_w_n_ram <= not data_we;
	ext_irq <= "0000000" & periph_irq;

	-- HF-RISCV core
	processor: entity work.processor
	port map(	clk_i => clock,
			rst_i => reset,
			stall_i => stall,
			addr_o => address,
			data_i => data_read,
			data_o => data_write,
			data_w_o => data_we,
			data_mode_o => open,
			extio_in => ext_irq,
			extio_out => open
	);

	data_read_periph <= data_read_periph_s(7 downto 0) & data_read_periph_s(15 downto 8) & data_read_periph_s(23 downto 16) & data_read_periph_s(31 downto 24);
	data_write_periph <= data_write(7 downto 0) & data_write(15 downto 8) & data_write(23 downto 16) & data_write(31 downto 24);
	periph_wr <= '1' when data_we /= "0000" else '0';
	periph <= '1' when address(31 downto 28) = x"e" else '0';

	peripherals: entity work.peripherals
	port map(
		clk_i => clock,
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
		gpiob_ddr => gpiob_ddr
	);

	-- instruction and data memory (boot RAM)
	boot_ram: entity work.ram
	generic map (memory_type => "DEFAULT")
	port map (
		clk			=> clock,
		enable			=> boot_enable,
		write_byte_enable	=> "0000",
		address			=> address(31 downto 2),
		data_write		=> (others => '0'),
		data_read		=> data_read_boot
	);

	-- instruction and data memory (external RAM)
	memory0lb: entity work.bram
	generic map (	memory_file => memory_file,
					data_width => 8,
					address_width => address_width,
					bank => 0)
	port map(
		clk 	=> clock,
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
		clk 	=> clock,
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
		clk 	=> clock,
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
		clk 	=> clock,
		addr 	=> address(address_width -1 downto 2),
		cs_n 	=> ram_enable_n,
		we_n	=> data_w_n_ram(3),
		data_i	=> data_write(31 downto 24),
		data_o	=> data_read_ram(31 downto 24)
	);

end top_level;

