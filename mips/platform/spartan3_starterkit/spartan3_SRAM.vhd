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
		uart_write:	out std_logic;

		extio_in:	in std_logic_vector(7 downto 0);
		extio_out:	out std_logic_vector(7 downto 0);

		ram_address:	out std_logic_vector(31 downto 2);
		ram_data:	inout std_logic_vector(31 downto 0);
		ram_ce1_n:	out std_logic;
		ram_ub1_n:	out std_logic;
		ram_lb1_n:	out std_logic;
		ram_ce2_n:	out std_logic;
		ram_ub2_n:	out std_logic;
		ram_lb2_n:	out std_logic;
		ram_we_n:	out std_logic;
		ram_oe_n:	out std_logic
	);
end hfrisc_soc;

architecture top_level of hfrisc_soc is
	signal clock, boot_enable, ram_enable_n, stall, stall_cpu, irq_cpu, irq_ack_cpu, data_access_cpu, ram_dly, rff1, reset: std_logic;
	signal address, data_read, data_write, data_read_boot, data_read_ram, irq_vector_cpu, address_cpu, data_in_cpu, data_out_cpu: std_logic_vector(31 downto 0);
	signal data_we, data_w_cpu: std_logic_vector(3 downto 0);

	signal we_n_next    : std_logic;
	signal we_n_reg     : std_logic;
	signal data_reg     : std_logic_vector(31 downto 0);
begin
	-- clock divider (25MHz clock from 50MHz main clock for Spartan3 Starter Kit)
	process (reset_in, clk_in, clock, we_n_next)
	begin
		if reset_in = '1' then
			clock <= '0';
		else
			if clk_in'event and clk_in='1' then
				clock <= not clock;
			end if;
		end if;

		if reset_in = '1' then
			we_n_reg <= '1';
		elsif rising_edge(clk_in) then
			we_n_reg <= we_n_next or not clock;
		end if;

		if reset_in = '1' then
			data_read_ram <= (others => '0');
		elsif rising_edge(clock) then
			data_read_ram <= ram_data;
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


	process (reset, clock, ram_enable_n)
	begin
		if reset = '1' then
			ram_dly <= '0';
		elsif clock'event and clock = '1' then
			ram_dly <= not ram_enable_n;
		end if;
	end process;

	stall <= '0';
	boot_enable <= '1' when address(31 downto 28) = "0000" else '0';
	data_read <= data_read_boot when address(31 downto 28) = "0000" and ram_dly = '0' else data_read_ram;

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
		extio_in => extio_in,
		extio_out => extio_out,
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

	-- instruction and data memory (external SRAM)
	-- very simple SRAM memory controller using both IS61LV25616AL chips.
	-- these SRAMs have 16-bit words, so we use both chips and access each using low and
	-- high banks. using this arrangement, we have byte addressable 32-bit words.
	-- the address bus is controlled directly by the CPU.
	ram_enable_n <= '0' when address(31 downto 28) = "0100" else '1';
	ram_address <= address(31 downto 2);
	ram_we_n <= we_n_reg;

	ram_control:   
	process(clock, ram_enable_n, data_we, data_write)
	begin
		if ram_enable_n = '0' then			--SRAM
			ram_ce1_n <= '0';
			ram_ce2_n <= '0';
			if data_we = "0000" then		-- read
				ram_data  <= (others => 'Z');
				ram_ub1_n <= '0';
				ram_lb1_n <= '0';
				ram_ub2_n <= '0';
				ram_lb2_n <= '0';
				we_n_next <= '1';
				ram_oe_n  <= '0';
			else					-- write
				if clock = '1' then
					ram_data <= (others => 'Z');
				else
					ram_data <= data_write;
				end if;
				ram_ub1_n <= not data_we(3);
				ram_lb1_n <= not data_we(2);
				ram_ub2_n <= not data_we(1);
				ram_lb2_n <= not data_we(0);
				we_n_next <= '0';
				ram_oe_n  <= '1';
			end if;
		else
			ram_data <= (others => 'Z');
			ram_ce1_n <= '1';
			ram_ub1_n <= '1';
			ram_lb1_n <= '1';
			ram_ce2_n <= '1';
			ram_ub2_n <= '1';
			ram_lb2_n <= '1';
			we_n_next <= '1';
			ram_oe_n  <= '1';
		end if;
	end process;

end top_level;

