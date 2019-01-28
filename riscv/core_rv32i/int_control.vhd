library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_unsigned.all;
use ieee.std_logic_arith.all;

entity interrupt_controller is
	port (	clock:		in std_logic;
		reset:		in std_logic;

		stall:		in std_logic;

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
		extio_out:	out std_logic_vector(7 downto 0)
	);
end interrupt_controller;

architecture arch_interrupt_controller of interrupt_controller is
	signal irq_cause, irq_mask_reg, irq_status_reg, extio_out_reg: std_logic_vector(7 downto 0);
	signal periph_data, irq_vector_reg, irq_epc_reg: std_logic_vector(31 downto 0);
	signal interrupt, irq: std_logic;

	type pulse_state_type is (irq_idle, irq_int, irq_req, irq_ackn, irq_done);
	signal pulse_state: pulse_state_type;
	signal pulse_next_state: pulse_state_type;

	signal periph_access, periph_access_dly, periph_access_we: std_logic;
	signal data_we_mem_s: std_logic_vector(3 downto 0);

begin
	-- address decoder, read from peripheral registers
	process(clock, reset, periph_access, address_cpu, irq_vector_reg, irq_cause, irq_mask_reg, irq_status_reg, irq_epc_reg, data_read_mem, extio_in, extio_out_reg)
	begin
		if reset = '1' then
			periph_data <= (others => '0');
		elsif clock'event and clock = '1' then
			if periph_access = '1' then
				case address_cpu(7 downto 4) is
					when "0000" =>		-- IRQ_VECTOR		(RW)
						periph_data <= irq_vector_reg;
					when "0001" =>		-- IRQ_CAUSE		(RO)
						periph_data <= irq_cause(7 downto 0) & x"000000";
					when "0010" =>		-- IRQ_MASK		(RW)
						periph_data <= irq_mask_reg(7 downto 0) & x"000000";
					when "0011" =>		-- IRQ_STATUS		(RW)
						periph_data <= irq_status_reg & x"000000";
					when "0100" =>		-- IRQ_EPC		(RO)
						periph_data <= irq_epc_reg(7 downto 0) & irq_epc_reg(15 downto 8) & irq_epc_reg(23 downto 16) & irq_epc_reg(31 downto 24);
					when "1000" =>		-- EXTIO_IN		(RO)
						periph_data <= extio_in & x"000000";
					when "1001" =>		-- EXTIO_OUT		(RW)
						periph_data <= extio_out_reg & x"000000";
					when others =>
						periph_data <= data_read_mem;
				end case;
			end if;
		end if;
	end process;

	data_in_cpu <= data_read_mem when periph_access_dly = '0' else periph_data;

	-- peripheral register logic, write to peripheral registers
	process(clock, reset, address_cpu, data_out_cpu, periph_access, periph_access_we, irq_ack_cpu)
	begin
		if reset = '1' then
			irq_vector_reg <= (others => '0');
			irq_mask_reg <= (others => '0');
			irq_status_reg <= (others => '0');
			extio_out_reg <= (others => '0');
		elsif clock'event and clock = '1' then
			if periph_access = '1' and periph_access_we = '1' then
				case address_cpu(7 downto 4) is
					when "0000" =>	-- IRQ_VECTOR
						irq_vector_reg <= data_out_cpu(7 downto 0) & data_out_cpu(15 downto 8) & data_out_cpu(23 downto 16) & data_out_cpu(31 downto 24);
					when "0010" =>	-- IRQ_MASK
						irq_mask_reg <= data_out_cpu(31 downto 24);
					when "0011" =>	-- IRQ_STATUS
						irq_status_reg <= data_out_cpu(31 downto 24);
					when "1001" =>	-- EXTIO_OUT
						extio_out_reg <= data_out_cpu(31 downto 24);
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

	-- memory address / write enable muxes
	addr_mem <= address_cpu;
	data_write_mem <= data_out_cpu;
	data_we_mem_s <= data_w_cpu when data_access_cpu = '1' and periph_access = '0' else "0000";
	data_we_mem <= data_we_mem_s;

	-- interrupts masking
	interrupt <= '0' when (irq_cause and irq_mask_reg) = x"0000" else '1';
	irq_cause <= extio_in;

	irq_cpu <= irq;
	irq_vector_cpu <= irq_vector_reg;
	extio_out <= extio_out_reg;

end arch_interrupt_controller;

