library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

entity uart_axis is
	generic (
		DATA_WIDTH : integer := 8;
		STOP_WIDTH : integer := 1
	);
	port (
		-- external interface signals
		divisor_i : in std_logic_vector(11 downto 0);
		rxd_i : in  std_logic;
		txd_o : out std_logic;
		-- AXI stream interface signals
		axis_aclk_i : in std_logic;
		axis_aresetn_i : in std_logic;
		-- master axi stream interface
		m_axis_tready_i : in  std_logic;
		m_axis_tvalid_o : out std_logic;
		m_axis_tdata_o : out std_logic_vector(DATA_WIDTH-1 downto 0);
		-- slave axi stream interface
		s_axis_tready_o : out std_logic;
		s_axis_tvalid_i : in  std_logic;
		s_axis_tdata_i : in  std_logic_vector(DATA_WIDTH-1 downto 0)
	);
end uart_axis;

architecture uart_arch of uart_axis is
begin

	urx : entity work.uart_rx_master(uart_rx_arch)
		generic map (DATA_WIDTH=>DATA_WIDTH, STOP_WIDTH=>STOP_WIDTH)
		port map (divisor_i => divisor_i, rxd_i=>rxd_i, axis_aclk_i=>axis_aclk_i, axis_aresetn_i=>axis_aresetn_i, 
			m_axis_tready_i=>m_axis_tready_i, m_axis_tvalid_o=>m_axis_tvalid_o, m_axis_tdata_o=>m_axis_tdata_o);

	utx : entity work.uart_tx_slave(uart_tx_arch)
		generic map (DATA_WIDTH=>DATA_WIDTH, STOP_WIDTH=>STOP_WIDTH)
		port map (divisor_i => divisor_i, txd_o=>txd_o, axis_aclk_i=>axis_aclk_i, axis_aresetn_i=>axis_aresetn_i, 
			s_axis_tready_o=>s_axis_tready_o, s_axis_tvalid_i=>s_axis_tvalid_i, s_axis_tdata_i=>s_axis_tdata_i);

end uart_arch;
