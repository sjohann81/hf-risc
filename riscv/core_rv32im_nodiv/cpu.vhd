library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_unsigned.all;
use ieee.std_logic_arith.all;

entity processor is
	port (	clk_i:		in std_logic;
		rst_i:		in std_logic;

		stall_i:	in std_logic;

		addr_o:		out std_logic_vector(31 downto 0);
		data_i:		in std_logic_vector(31 downto 0);
		data_o:		out std_logic_vector(31 downto 0);
		data_w_o:	out std_logic_vector(3 downto 0);
		
		data_mode_o:	out std_logic_vector(2 downto 0);

		extio_in:	in std_logic_vector(7 downto 0);
		extio_out:	out std_logic_vector(7 downto 0)
	);
end processor;

architecture arch_processor of processor is
	signal irq_cpu, irq_ack_cpu, exception_cpu, data_b_cpu, data_h_cpu, data_access_cpu: std_logic;
	signal irq_vector_cpu, address_cpu, data_in_cpu, data_out_cpu: std_logic_vector(31 downto 0);
	signal data_w_cpu: std_logic_vector(3 downto 0);
begin
	data_mode_o <= data_b_cpu & data_h_cpu & data_access_cpu;

	-- HF-RISC core
	core: entity work.datapath
	port map(	clock => clk_i,
			reset => rst_i,
			stall => stall_i,
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

	-- interrupt controller
	int_control: entity work.interrupt_controller
	port map(
		clock => clk_i,
		reset => rst_i,

		stall => stall_i,

		irq_vector_cpu => irq_vector_cpu,
		irq_cpu => irq_cpu,
		irq_ack_cpu => irq_ack_cpu,
		exception_cpu => exception_cpu,
		address_cpu => address_cpu,
		data_in_cpu => data_in_cpu,
		data_out_cpu => data_out_cpu,
		data_w_cpu => data_w_cpu,
		data_access_cpu => data_access_cpu,

		addr_mem => addr_o,
		data_read_mem => data_i,
		data_write_mem => data_o,
		data_we_mem => data_w_o,
		extio_in => extio_in,
		extio_out => extio_out
	);
end arch_processor;
