library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_unsigned.all;
use ieee.std_logic_arith.all;

entity uart_tx_slave is
	generic (
		DATA_WIDTH : integer := 8;
		STOP_WIDTH : integer := 1
	);
	port (
		-- external interface signals
		divisor_i : in std_logic_vector(11 downto 0);
		txd_o : out std_logic;
		-- axi stream slave interface
		axis_aclk_i : in  std_logic;
		axis_aresetn_i : in  std_logic;
		s_axis_tready_o : out std_logic;
		s_axis_tvalid_i : in  std_logic;
		s_axis_tdata_i  : in  std_logic_vector(DATA_WIDTH-1 downto 0)
	);
end uart_tx_slave;

architecture uart_tx_arch of uart_tx_slave is
	signal tready : std_logic;
		type state_type is (st_idle, st_start, st_data, st_stop);
	signal state : state_type;
	signal txbuf : std_logic_vector(DATA_WIDTH-1 downto 0);
	signal bitcount : std_logic_vector(3 downto 0);
	signal clkcount : std_logic_vector(11 downto 0);
begin
	s_axis_tready_o <= tready;

	tx_process: process(axis_aclk_i) is
	begin
		if axis_aresetn_i = '0' then
			tready <= '0';
			txbuf <= (others => '0');
			bitcount <= (others => '0');
			clkcount <= (others => '0');
		elsif axis_aclk_i'event and axis_aclk_i = '1' then
			txd_o <= '1';
			tready <= '0';

			case state is
				when st_idle =>
					tready <= '1';
					if s_axis_tvalid_i = '1' then
						txbuf <= s_axis_tdata_i;
						state <= st_start;
					end if;
				when st_start =>
					txd_o <= '0';
					if clkcount = divisor_i - 1 then
						clkcount <= (others => '0');
						state <= st_data;
					else
						clkcount <= clkcount + 1;
					end if;
				when st_data =>
					txd_o <= txbuf(conv_integer(bitcount));
					if clkcount = divisor_i - 1 then
						clkcount <= (others => '0');

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
					txd_o <= '1';
					if clkcount = divisor_i - 1 then
						clkcount <= (others => '0');
						if bitcount = STOP_WIDTH-1 then
							bitcount <= (others => '0');
							state <= st_idle;
						else
							bitcount <= bitcount + 1;
						end if;
					else
						clkcount <= clkcount + 1;
					end if;
			end case;
		end if;
	end process;

end uart_tx_arch;
