library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_textio.all;
use ieee.std_logic_unsigned.all;
use std.textio.all;
use ieee.numeric_std.all;

entity tb is
	generic(
		address_width: integer := 16;
		memory_file : string := "code.txt";
		log_file: string := "out.txt";
		uart_support : string := "no"
	);
end tb;

architecture tb of tb is
	signal clock_in, reset, stall_cpu, data, stall, stall_sig: std_logic := '0';
	signal uart_read, uart_write: std_logic;
	signal boot_enable_n, ram_enable_n, irq_cpu, irq_ack_cpu, exception_cpu, data_b_cpu, data_h_cpu, data_access_cpu, ram_dly: std_logic;
	signal address, data_read, data_write, data_read_boot, data_read_ram, irq_vector_cpu, address_cpu, data_in_cpu, data_out_cpu: std_logic_vector(31 downto 0);
	signal ext_irq: std_logic_vector(7 downto 0);
	signal data_we, data_w_n_ram, data_w_cpu: std_logic_vector(3 downto 0);

	signal ext_periph, ext_periph_dly, ready: std_logic;
	signal key: std_logic_vector(127 downto 0);
	signal input, output: std_logic_vector(63 downto 0);
	signal data_read_xtea, data_read_xtea_s, data_write_s: std_logic_vector(31 downto 0);
	signal control: std_logic_vector(1 downto 0);
begin

	process						--25Mhz system clock
	begin
		clock_in <= not clock_in;
		wait for 20 ns;
		clock_in <= not clock_in;
		wait for 20 ns;
	end process;

	process(clock_in, reset)
	begin
		if reset = '1' then
			ram_dly <= '0';
			ext_periph_dly <= '0';
			ext_irq <= x"00";
		elsif clock_in'event and clock_in = '1' then
			ram_dly <= not ram_enable_n;
			ext_periph_dly <= ext_periph;
			ext_irq <= x"00";
		end if;
	end process;

	process (clock_in, reset, address_cpu, key, input, output)
	begin
		if reset = '1' then
			data_read_xtea_s <= (others => '0');
		elsif clock_in'event and clock_in = '1' then
			if (ext_periph = '1') then	-- XTEA is at 0xfa000000
				case address_cpu(7 downto 4) is
					when "0000" =>		-- control	0xfa000000	(bit2 - ready (R), bit1 - encrypt (RW), bit0 - start (RW)
						data_read_xtea_s <= x"000000" & "00000" & ready & control;
					when "0001" =>		-- key[0]	0xfa000010
						data_read_xtea_s <= key(127 downto 96);
					when "0010" =>		-- key[1]	0xfa000020
						data_read_xtea_s <= key(95 downto 64);
					when "0011" =>		-- key[2]	0xfa000030
						data_read_xtea_s <= key(63 downto 32);
					when "0100" =>		-- key[3]	0xfa000040
						data_read_xtea_s <= key(31 downto 0);
					when "0101" =>		-- input[0]	0xfa000050
						data_read_xtea_s <= input(63 downto 32);
					when "0110" =>		-- input[1]	0xfa000060
						data_read_xtea_s <= input(31 downto 0);
					when "0111" =>		-- output[0]	0xfa000070
						data_read_xtea_s <= output(63 downto 32);
					when "1000" =>		-- output[1]	0xfa000080
						data_read_xtea_s <= output(31 downto 0);
					when others =>
						data_read_xtea_s <= (others => '0');
				end case;
			end if;
		end if;
	end process;

	process (clock_in, reset, address_cpu, control, key, input, output)
	begin
		if reset = '1' then
			key <= (others => '0');
			input <= (others => '0');
			control <= "00";
		elsif clock_in'event and clock_in = '1' then
			if (ext_periph = '1' and data_we /= "0000") then	-- XTEA is at 0xfa000000
				case address_cpu(7 downto 4) is
					when "0000" =>		-- control	0xfa000000	(bit2 - ready (R), bit1 - encrypt (RW), bit0 - start (RW)
						control <= data_write_s(1 downto 0);
					when "0001" =>		-- key[0]	0xfa000010
						key(127 downto 96) <= data_write_s;
					when "0010" =>		-- key[1]	0xfa000020
						key(95 downto 64) <= data_write_s;
					when "0011" =>		-- key[2]	0xfa000030
						key(63 downto 32) <= data_write_s;
					when "0100" =>		-- key[3]	0xfa000040
						key(31 downto 0) <= data_write_s;
					when "0101" =>		-- input[0]	0xfa000050
						input(63 downto 32) <= data_write_s;
					when "0110" =>		-- input[1]	0xfa000060
						input(31 downto 0) <= data_write_s;
					when others =>
				end case;
			end if;
		end if;
	end process;

	data_read_xtea <= data_read_xtea_s(7 downto 0) & data_read_xtea_s(15 downto 8) & data_read_xtea_s(23 downto 16) & data_read_xtea_s(31 downto 24);
	data_write_s <= data_write(7 downto 0) & data_write(15 downto 8) & data_write(23 downto 16) & data_write(31 downto 24);

	reset <= '0', '1' after 5 ns, '0' after 500 ns;
	stall_sig <= '0';
	uart_read <= '1';
	boot_enable_n <= '0' when (address(31 downto 28) = "0000" and stall_cpu = '0') or reset = '1' else '1';
	ram_enable_n <= '0' when (address(31 downto 28) = "0100" and stall_cpu = '0') or reset = '1' else '1';
	ext_periph <= '1' when address(31 downto 24) = x"fa" else '0';
	data_read <= data_read_xtea when ext_periph = '1' or ext_periph_dly = '1' else data_read_boot when address(31 downto 28) = "0000" and ram_dly = '0' else data_read_ram;
	data_w_n_ram <= not data_we;

	-- HF-RISCV core
	core: entity work.datapath
	port map(	clock => clock_in,
			reset => reset,
			stall => stall_cpu,
			irq_vector => irq_vector_cpu,
			irq => irq_cpu,
			irq_ack => irq_ack_cpu,
			exception => exception_cpu,
			address => address_cpu,
			data_in => data_in_cpu,
			data_out => data_out_cpu,
			data_w => data_w_cpu,
			data_b => data_b_cpu,
			data_h => data_h_cpu,
			data_access => data_access_cpu
	);

	-- peripherals / busmux logic
	peripherals_busmux: entity work.busmux
	generic map(
		log_file => log_file,
		uart_support => uart_support
	)
	port map(
		clock => clock_in,
		reset => reset,

		stall => stall_sig,

		stall_cpu => stall_cpu,
		irq_vector_cpu => irq_vector_cpu,
		irq_cpu => irq_cpu,
		irq_ack_cpu => irq_ack_cpu,
		exception_cpu => exception_cpu,
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

	-- XTEA core
	crypto_core: entity work.xtea
	port map(	clock => clock_in,
			reset => reset,
			start => control(0),
			encrypt => control(1),
			key => key,
			input => input,
			output => output,
			ready => ready
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
		process(clock_in, address_cpu)
			file store_file : text open write_mode is "debug.txt";
			variable hex_file_line : line;
			variable c : character;
			variable index : natural;
			variable line_length : natural := 0;
		begin
			if clock_in'event and clock_in = '1' then
				if address_cpu = x"f00000d0" and data = '0' then
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
			assert (address < x"50000000") or (address >= x"f0000000") report "out of memory region" severity failure;
			assert address /= x"40000100" report "handling IRQ" severity warning;
		end if;
	end process;

end tb;

