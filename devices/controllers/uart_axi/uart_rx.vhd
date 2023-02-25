library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_unsigned.all;
use ieee.std_logic_arith.all;

entity uart_rx_master is
	generic (
		DATA_WIDTH : integer := 8;
		STOP_WIDTH : integer := 1
	);
	port (
		-- external interface signals
		divisor_i : in std_logic_vector(11 downto 0);
		rxd_i : in  std_logic;
		-- axi stream master interface        
		axis_aclk_i : in  std_logic;
		axis_aresetn_i : in  std_logic;
		m_axis_tready_i : in  std_logic;
		m_axis_tvalid_o : out std_logic;
		m_axis_tdata_o  : out std_logic_vector(DATA_WIDTH-1 downto 0)
	);
end uart_rx_master;

architecture uart_rx_arch of uart_rx_master is
	signal tvalid : std_logic;
	signal tdata  : std_logic_vector(DATA_WIDTH-1 downto 0) := (others => '0');

	type state_type is (st_idle, st_data, st_stop);
	signal state : state_type;
	signal rxbuf : std_logic_vector(DATA_WIDTH-1 downto 0);
	signal bitcount : std_logic_vector(3 downto 0);
	signal clkcount : std_logic_vector(11 downto 0);
begin
	m_axis_tvalid_o <= tvalid;
	m_axis_tdata_o <= tdata;

	rx_process: process(axis_aclk_i) is
	begin
		if axis_aresetn_i = '0' then
			tvalid <= '0';
			tdata <= (others => '0');
			rxbuf <= (others => '0');
			bitcount <= (others => '0');
			clkcount <= (others => '0');
		elsif axis_aclk_i'event and axis_aclk_i = '1' then
			if m_axis_tready_i = '1' then
				tvalid <= '0';
			end if;

			case state is
				when st_idle =>
					if rxd_i = '0' then
						if clkcount = ('0' & divisor_i(11 downto 1)) - 1 then
							clkcount <= (others => '0');
							state <= st_data;
						else
							clkcount <= clkcount + 1;
						end if;
					else
						clkcount <= (others => '0');
					end if;
				when st_data =>
					if clkcount = divisor_i - 1 then
						clkcount <= (others => '0');
						rxbuf <= rxd_i & rxbuf(DATA_WIDTH-1 downto 1);
						if bitcount = DATA_WIDTH-1 then
							bitcount <= (others => '0');
							state <= st_stop;
						else
							bitcount <= bitcount + 1;
						end if;
					else
						clkcount <= clkcount + 1;
					end if;
				when st_stop =>
					if clkcount = divisor_i - 1 then
						clkcount <= (others => '0');
						state <= st_idle;
						tvalid <= '1';
						tdata <= rxbuf;
					else
						clkcount <= clkcount + 1;
					end if;
			end case;
		end if;
	end process;

end uart_rx_arch;
