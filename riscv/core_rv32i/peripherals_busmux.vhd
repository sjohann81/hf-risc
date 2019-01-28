-- HF-RISCV v1.5
-- Sergio Johann Filho, 2015 - 2016
--
-- *This is a quick and dirty organization of a 3-stage pipelined RISC-V microprocessor. All registers / memory
--  accesses are synchronized to the rising edge of clock. The same processor could be designed with only 2
--  pipeline stages, but this would require memories to be either asynchronous (as presented on comp arq text
--  books), double clocked or operating on the opposite edge. Pipeline stages are:
--
--  FETCH: instruction memory is accessed (address is PC), data becomes available in one cycle. PC is updated.
--  DECODE: an instruction is fed into the decoding / control logic and values are registered for the next
--  stage. pipeline stalls, as well as bubble insertion is performed in this stage.
--  EXECUTE: the register file is accessed and the ALU calculates the result. data access is performed (loads
--  and stores) or simply the result (or pc) is written to the register file (normal operations). branch target
--  and outcome are calculated.
--
-- *This design is a compromise between performance, area and complexity.
-- *Only the RV32I base instruction set is implemented. FENCE and SYSTEM instructions are missing. SYSTEM
--  instructions always trap the processor and can be handled in software.
-- *Memory is accessed in little endian mode.
-- *No co-processor is implemented and all peripherals are memory mapped.
-- *Loads and stores take 3 cycles. This version is organized as a Von Neumann machine, so there is only one
--  memory interface that is shared betweeen code and data accesses.
-- *Branches have a 1 cycle delay (not taken) or 3 cycle dalay (taken), including two branch delay slots.
--  This is a side effect of the pipeline refill and memory access policy. All other instructions are single
--  cycle. No branch predictor is implemented (default branch target is 'not taken').
-- *Interrupts are handled using VECTOR, CAUSE, MASK, STATUS and EPC registers. The VECTOR register is used to hold
--  the address of the default (non-vectored) interrupt handler. The CAUSE register is read only and peripheral
--  interrupt lines are connected to this register. The MASK register is read/write and holds the interrupt mask
--  for the CAUSE register. The interrupt STATUS register is automatically cleared on interrupts, and is set by
--  software when returning from interrupts - this works as a global interrupt enable/disable flag. This register is
--  read and write capable, so it can also be cleared by software. Setting this register just before returning
--  from interrupts (enable is delayed in a few cycles) re-enables interrupts. The EPC register holds the program
--  counter when the processor is interrupted (we should re-execute the last instruction (EPC-4), as it was not
--  commited yet). EPC is a read only register, and is used to return from an interrupt using simple LW / JR
--  instructions. As an interrupt is accepted, the processor jumps to VECTOR address where the first level of irq
--  handling is done. A second level handler (in C) implements the interrupt priority mechanism and calls the
--  appropriate ISR for each interrupt.
-- *Built in peripherals: running counter (32 bit), two counter comparators (32 and 24 bit), I/O ports and UART. the
--  UART baud rate is defined in a 16 bit divisor register. Two counter bits (bits 18 and 16 and their complements) are
--  tied to interrupt lines, so are the two counter comparators and the UART.
--
-- Memory map:
--
-- ROM / Flash				0x00000000 - 0x1fffffff (512MB)
-- Reserved				0x20000000 - 0x3fffffff (512MB)
-- SRAM					0x40000000 - 0x5fffffff (512MB)
-- External RAM	/ device		0x60000000 - 0x9fffffff (1GB)
-- External RAM / device		0xa0000000 - 0xdfffffff (1GB)		(uncached)
-- Reserved				0xe0000000 - 0xe0ffffff (16MB)
-- Peripheral (SoC segment 0)		0xe1000000 - 0xe1ffffff (16MB)		(uncached)
-- Peripheral (SoC segment 1)		0xe2000000 - 0xe3ffffff (32MB)		(uncached)
-- Peripheral (SoC segment 2)		0xe4000000 - 0xe7ffffff (64MB)		(uncached)
-- Peripheral (SoC segment 3)		0xe8000000 - 0xefffffff (128MB)		(uncached)
-- Peripheral (core)			0xf0000000 - 0xf0ffffff (16MB)		(uncached)
-- Reserved				0xf1000000 - 0xffffffff (240MB)
--
--   IRQ_VECTOR			0xf0000000
--   IRQ_CAUSE			0xf0000010
--   IRQ_MASK			0xf0000020
--   IRQ_STATUS			0xf0000030
--   IRQ_EPC			0xf0000040
--   COUNTER			0xf0000050
--   COMPARE			0xf0000060
--   COMPARE2			0xf0000070
--   EXTIO_IN			0xf0000080
--   EXTIO_OUT			0xf0000090
--   DEBUG			0xf00000d0
--   UART_WRITE / UART_READ	0xf00000e0
--   UART_DIVISOR		0xf00000f0
--
-- Interrupt masks:
--
-- IRQ_COUNTER			0x00000001		(bit 18 of the counter is set)
-- IRQ_COUNTER_NOT		0x00000002		(bit 18 of the counter is clear)
-- IRQ_COUNTER2			0x00000004		(bit 16 of the counter is set)
-- IRQ_COUNTER2_NOT		0x00000008		(bit 16 of the counter is clear)
-- IRQ_COMPARE			0x00000010		(counter is equal to compare, clears irq when updated)
-- IRQ_COMPARE2			0x00000020		(counter bits 23 to 0 are equal to compare2, clears irq when updated)
-- IRQ_UART_READ_AVAILABLE	0x00000040		(there is data available for reading on the UART)
-- IRQ_UART_WRITE_AVAILABLE	0x00000080		(UART is not busy)
-- EXT_IRQ0			0x00010000		(external interrupts on extio_in, high level triggered)
-- EXT_IRQ1			0x00020000
-- EXT_IRQ2			0x00040000
-- EXT_IRQ3			0x00080000
-- EXT_IRQ4			0x00100000
-- EXT_IRQ5			0x00200000
-- EXT_IRQ6			0x00400000
-- EXT_IRQ7			0x00800000
-- EXT_IRQ0_NOT			0x01000000		(external interrupts on extio_in, low level triggered)
-- EXT_IRQ1_NOT			0x02000000
-- EXT_IRQ2_NOT			0x04000000
-- EXT_IRQ3_NOT			0x08000000
-- EXT_IRQ4_NOT			0x10000000
-- EXT_IRQ5_NOT			0x20000000
-- EXT_IRQ6_NOT			0x40000000
-- EXT_IRQ7_NOT			0x80000000

library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_unsigned.all;
use ieee.std_logic_arith.all;

entity busmux is
	generic(
		log_file: string := "UNUSED";			-- options are "out.txt" and "UNUSED"
		uart_support: string := "no"			-- options are "yes" and "no".
	);
	port (	clock:		in std_logic;
		reset:		in std_logic;

		stall:		in std_logic;

		stall_cpu:	out std_logic;
		irq_vector_cpu:	out std_logic_vector(31 downto 0);
		irq_cpu:	out std_logic;
		irq_ack_cpu:	in std_logic;
		exception_cpu:	in std_logic;
		address_cpu: 	in std_logic_vector(31 downto 0);
		data_in_cpu:	out std_logic_vector(31 downto 0);
		data_out_cpu:	in std_logic_vector(31 downto 0);
		data_w_cpu:	in std_logic_vector(3 downto 0);
		data_access_cpu:	in std_logic;

		addr_mem:	out std_logic_vector(31 downto 0);
		data_read_mem:	in std_logic_vector(31 downto 0);
		data_write_mem:	out std_logic_vector(31 downto 0);
		data_we_mem:	out std_logic_vector(3 downto 0);

		extio_in:	in std_logic_vector(7 downto 0);
		extio_out:	out std_logic_vector(7 downto 0);
		uart_read:	in std_logic;
		uart_write:	out std_logic
	);
end busmux;

architecture arch of busmux is
	signal write_enable: std_logic;
	signal uart_divisor: std_logic_vector(15 downto 0);
	signal irq_status_reg, extio_out_reg: std_logic_vector(7 downto 0);
	signal periph_data, irq_vector_reg, irq_cause, irq_mask_reg, irq_epc_reg, compare_reg, counter_reg: std_logic_vector(31 downto 0);
	signal compare2_reg: std_logic_vector(23 downto 0);
	signal interrupt, irq, irq_counter, irq_counter_not, irq_counter2, irq_counter2_not, irq_compare, irq_compare2, compare_trig, compare2_trig: std_logic;
	signal data_read_uart, data_write_uart: std_logic_vector(7 downto 0);
	signal enable_uart, enable_uart_read, enable_uart_write, uart_write_busy, uart_data_avail: std_logic;

	type pulse_state_type is (irq_idle, irq_int, irq_req, irq_ackn, irq_done);
	signal pulse_state: pulse_state_type;
	signal pulse_next_state: pulse_state_type;

	signal periph_access, periph_access_dly, periph_access_we: std_logic;
	signal data_we_mem_s: std_logic_vector(3 downto 0);

begin
	-- address decoder, read from peripheral registers
	process(clock, reset, periph_access, address_cpu, irq_vector_reg, irq_cause, irq_mask_reg, irq_status_reg, irq_epc_reg, compare_reg, compare2_reg, counter_reg, data_read_uart, uart_divisor, data_read_mem, extio_in, extio_out_reg)
	begin
		if reset = '1' then
			periph_data <= (others => '0');
		elsif clock'event and clock = '1' then
			if periph_access = '1' then
				case address_cpu(7 downto 4) is
					when "0000" =>		-- IRQ_VECTOR		(RW)
						periph_data <= irq_vector_reg;
					when "0001" =>		-- IRQ_CAUSE		(RO)
						periph_data <= irq_cause(7 downto 0) & irq_cause(15 downto 8) & irq_cause(23 downto 16) & irq_cause(31 downto 24);
					when "0010" =>		-- IRQ_MASK		(RW)
						periph_data <= irq_mask_reg(7 downto 0) & irq_mask_reg(15 downto 8) & irq_mask_reg(23 downto 16) & irq_mask_reg(31 downto 24);
					when "0011" =>		-- IRQ_STATUS		(RW)
						periph_data <= irq_status_reg & x"000000";
					when "0100" =>		-- IRQ_EPC		(RO)
						periph_data <= irq_epc_reg(7 downto 0) & irq_epc_reg(15 downto 8) & irq_epc_reg(23 downto 16) & irq_epc_reg(31 downto 24);
					when "0101" =>		-- COUNTER		(RO)
						periph_data <= counter_reg(7 downto 0) & counter_reg(15 downto 8) & counter_reg(23 downto 16) & counter_reg(31 downto 24);
					when "0110" =>		-- IRQ_COMPARE		(RW)
						periph_data <= compare_reg(7 downto 0) & compare_reg(15 downto 8) & compare_reg(23 downto 16) & compare_reg(31 downto 24);
					when "0111" =>		-- IRQ_COMPARE2		(RW)
						periph_data <= compare_reg(15 downto 8) & compare_reg(23 downto 16) & compare_reg(31 downto 24) & x"00";
					when "1000" =>		-- EXTIO_IN		(RO)
						periph_data <= extio_in & x"000000";
					when "1001" =>		-- EXTIO_OUT		(RW)
						periph_data <= extio_out_reg & x"000000";
					when "1110" =>		-- UART			(RW)
						periph_data <= data_read_uart & x"000000";
					when "1111" =>		-- UART_DIVISOR		(RW)
						periph_data <= uart_divisor(7 downto 0) & uart_divisor(15 downto 8) & x"0000";
					when others =>
						periph_data <= data_read_mem;
				end case;
			end if;
		end if;
	end process;

	data_in_cpu <= data_read_mem when periph_access_dly = '0' else periph_data;

	-- peripheral register logic, write to peripheral registers
	process(clock, reset, counter_reg, address_cpu, data_out_cpu, periph_access, periph_access_we, irq_ack_cpu)
	begin
		if reset = '1' then
			irq_vector_reg <= (others => '0');
			irq_mask_reg <= (others => '0');
			irq_status_reg <= (others => '0');
			counter_reg <= (others => '0');
			compare_reg <= (others => '0');
			compare_trig <= '0';
			compare2_reg <= (others => '0');
			compare2_trig <= '0';
			extio_out_reg <= (others => '0');
			uart_divisor <= (others => '0');
		elsif clock'event and clock = '1' then
			counter_reg <= counter_reg + 1;
			if compare_reg = counter_reg then
				compare_trig <= '1';
			end if;
			if compare2_reg = counter_reg(23 downto 0) then
				compare2_trig <= '1';
			end if;
			if periph_access = '1' and periph_access_we = '1' then
				case address_cpu(7 downto 4) is
					when "0000" =>	-- IRQ_VECTOR
						irq_vector_reg <= data_out_cpu(7 downto 0) & data_out_cpu(15 downto 8) & data_out_cpu(23 downto 16) & data_out_cpu(31 downto 24);
					when "0010" =>	-- IRQ_MASK
						irq_mask_reg <= data_out_cpu(7 downto 0) & data_out_cpu(15 downto 8) & data_out_cpu(23 downto 16) & data_out_cpu(31 downto 24);
					when "0011" =>	-- IRQ_STATUS
						irq_status_reg <= data_out_cpu(31 downto 24);
					when "0110" =>	-- IRQ_COMPARE
						compare_reg <= data_out_cpu(7 downto 0) & data_out_cpu(15 downto 8) & data_out_cpu(23 downto 16) & data_out_cpu(31 downto 24);
						compare_trig <= '0';
					when "0111" =>	-- IRQ_COMPARE2
						compare2_reg <= data_out_cpu(15 downto 8) & data_out_cpu(23 downto 16) & data_out_cpu(31 downto 24);
						compare2_trig <= '0';
					when "1001" =>	-- EXTIO_OUT
						extio_out_reg <= data_out_cpu(31 downto 24);
					when "1111" =>	-- UART_DIVISOR
						uart_divisor <= data_out_cpu(23 downto 16) & data_out_cpu(31 downto 24);
					when others =>
				end case;
			end if;
			if irq_ack_cpu = '1' or exception_cpu = '1' then
				irq_status_reg(0) <= '0';		-- IRQ_STATUS (clear master int bit on interrupt)
			end if;
		end if;
	end process;

	-- EPC register register load on interrupts
	process(clock, reset, address_cpu, irq, irq_ack_cpu)
	begin
		if reset = '1' then
			irq_epc_reg <= x"00000000";
		elsif clock'event and clock = '1' then
			if ((irq = '1' and irq_ack_cpu = '0') or exception_cpu = '1') then
				irq_epc_reg <= address_cpu;
			end if;
		end if;
	end process;

	-- interrupt state machine
	process(clock, reset, pulse_state, interrupt, irq_status_reg, stall)
	begin
		if reset = '1' then
			pulse_state <= irq_idle;
			pulse_next_state <= irq_idle;
			irq <= '0';
		elsif clock'event and clock = '1' then
			if stall = '0' then
				pulse_state <= pulse_next_state;
				case pulse_state is
					when irq_idle =>
						if interrupt = '1' and irq_status_reg(0) = '1' then
							pulse_next_state <= irq_int;
						end if;
					when irq_int =>
						irq <= '1';
						pulse_next_state <= irq_req;
					when irq_req =>
						if irq_ack_cpu = '1' then
							irq <= '0';
							pulse_next_state <= irq_ackn;
						end if;
					when irq_ackn =>
						pulse_next_state <= irq_done;
					when irq_done =>
						if irq_status_reg(0) = '1' then
							pulse_next_state <= irq_idle;
						end if;
					when others =>
						pulse_next_state <= irq_idle;
				end case;
			end if;
		end if;
	end process;

	-- data / peripheral access delay
	process(clock, reset, irq_ack_cpu, periph_access, stall)
	begin
		if reset = '1' then
			periph_access_dly <= '0';
		elsif clock'event and clock = '1' then
			if stall = '0' then
				periph_access_dly <= periph_access;
			end if;
		end if;
	end process;

	periph_access <= '1' when address_cpu(31 downto 27) = "11110" and data_access_cpu = '1' else '0';
	periph_access_we <= '1' when periph_access <= '1' and data_w_cpu /= "0000" else '0';

	-- memory address / write enable muxes and cpu stall logic
	addr_mem <= address_cpu;
	data_write_mem <= data_out_cpu;
	data_we_mem_s <= data_w_cpu when data_access_cpu = '1' and periph_access = '0' else "0000";
	data_we_mem <= data_we_mem_s;

	stall_cpu <= stall;

	-- interrupts and peripherals
	interrupt <= '0' when (irq_cause and irq_mask_reg) = x"0000" else '1';
	irq_cause <= not extio_in & extio_in & x"00" & not uart_write_busy & uart_data_avail & irq_compare2 & irq_compare & irq_counter2_not & irq_counter2 & irq_counter_not & irq_counter;

	irq_cpu <= irq;
	irq_vector_cpu <= irq_vector_reg;
	irq_counter <= counter_reg(18);
	irq_counter_not <= not counter_reg(18);
	irq_counter2 <= counter_reg(16);
	irq_counter2_not <= not counter_reg(16);
	irq_compare <= '1' when compare_trig = '1' else '0';
	irq_compare2 <= '1' when compare2_trig = '1' else '0';
	extio_out <= extio_out_reg;

	write_enable <= '1' when data_we_mem_s /= "0000" else '0';
	data_write_uart <= data_out_cpu(31 downto 24);

	uart:
	if uart_support = "yes" generate
		enable_uart <= '1' when periph_access = '1' and address_cpu(7 downto 4) = "1110" else '0';
		enable_uart_write <= enable_uart and periph_access_we;
		enable_uart_read <= enable_uart and not periph_access_we;

		-- a simple UART
		serial: entity work.uart
		generic map (log_file => log_file)
		port map(
			clk		=> clock,
			reset		=> reset,
			divisor		=> uart_divisor(11 downto 0),
			enable_read	=> enable_uart_read,
			enable_write	=> enable_uart_write,
			data_in		=> data_write_uart,
			data_out	=> data_read_uart,
			uart_read	=> uart_read,
			uart_write	=> uart_write,
			busy_write	=> uart_write_busy,
			data_avail	=> uart_data_avail
		);
	end generate;

	no_uart:
	if uart_support = "no" generate
		enable_uart <= '0';
		data_read_uart <= (others => '0');
		uart_write_busy <= '0';
		uart_data_avail <= '0';
	end generate;

end arch;

