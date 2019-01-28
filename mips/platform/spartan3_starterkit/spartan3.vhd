library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_unsigned.all;

entity hfrisc_soc is
	generic(
		address_width: integer := 14;
		memory_file : string := "code.txt";
		uart_support : string := "yes"
	);
	port (	clk_in:		in std_logic;
		reset_in:	in std_logic;
		int_in:		in std_logic;
		uart_read:	in std_logic;
		uart_write:	out std_logic
	);
end hfrisc_soc;

architecture top_level of hfrisc_soc is
	signal clock, boot_enable, ram_enable_n, stall, stall_cpu, irq_cpu, irq_ack_cpu, data_access_cpu, ram_dly, rff1, reset: std_logic;
	signal address, data_read, data_write, data_read_boot, data_read_ram, irq_vector_cpu, address_cpu, data_in_cpu, data_out_cpu: std_logic_vector(31 downto 0);
	signal ext_irq: std_logic_vector(7 downto 0);
	signal data_we, data_w_n_ram, data_w_cpu: std_logic_vector(3 downto 0);
begin
	-- clock divider (25MHz clock from 50MHz main clock for Spartan3 Starter Kit)
	process (reset_in, clk_in, clock)
	begin
		if reset_in = '1' then
			clock <= '0';
		else
			if clk_in'event and clk_in='1' then
				clock <= not clock;
			end if;
		end if;
	end process;

	-- reset synchronizer
	process (clock, reset_in)
	begin
		if (reset_in = '1') then
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
			ext_irq <= x"00";
		elsif clock'event and clock = '1' then
			ram_dly <= not ram_enable_n;
			ext_irq <= "0000000" & int_in;
		end if;
	end process;

	stall <= '0';
	boot_enable <= '1' when address(31 downto 28) = "0000" else '0';
	ram_enable_n <= '0' when address(31 downto 28) = "0100" else '1';
	data_read <= data_read_boot when address(31 downto 28) = "0000" and ram_dly = '0' else data_read_ram;
	data_w_n_ram <= not data_we;

	-- HF-RISC core
	core: entity work.datapath
	port map(	clock => clock,
			reset => reset,
			stall => stall_cpu,
			irq_vector => irq_vector_cpu,
			irq => irq_cpu,
			irq_ack => irq_ack_cpu,
			address => address_cpu,
			data_in => data_in_cpu,
			data_out => data_out_cpu,
			data_w => data_w_cpu,
			data_access => data_access_cpu
	);

	-- peripherals / busmux logic
	peripherals_busmux: entity work.busmux
	generic map(
		uart_support => uart_support
	)
	port map(
		clock => clock,
		reset => reset,

		stall => stall,

		stall_cpu => stall_cpu,
		irq_vector_cpu => irq_vector_cpu,
		irq_cpu => irq_cpu,
		irq_ack_cpu => irq_ack_cpu,
		address_cpu => address_cpu,
		data_in_cpu => data_in_cpu,
		data_out_cpu => data_out_cpu,
		data_w_cpu => data_w_cpu,
		data_access_cpu => data_access_cpu,
		
		addr_mem => address,
		data_read_mem => data_read,
		data_write_mem => data_write,
		data_we_mem => data_we,
		extio_in => ext_irq,
		extio_out => open,
		uart_read => uart_read,
		uart_write => uart_write
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

