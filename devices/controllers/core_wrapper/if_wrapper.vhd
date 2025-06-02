library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

entity if_wrapper is
	generic (
		SOC_SEGMENT : integer := 	16#e2#;		-- SoC segment address is 0xe2
		SOC_CLASS : integer := 		16#00#;		-- Class address is 0x00
		AXIS_DATA_WIDTH : integer := 8
	);
	port (
		-- CPU bus signals
		clk_i  : in std_logic;
		rst_i  : in std_logic;
		addr_i : in std_logic_vector(31 downto 0);
		data_i : in std_logic_vector(31 downto 0);
		data_o : out std_logic_vector(31 downto 0);
		data_access_o : out std_logic;
		data_w_i: in std_logic
	);
end if_wrapper;

architecture if_arch of if_wrapper is
	-- wrapper internal signals
	signal data_access : std_logic;
	signal data_read_wrapper : std_logic_vector(31 downto 0);
	signal device : std_logic_vector(15 downto 0);
	
	-- registers
	signal control: std_logic_vector(11 downto 0);
	signal data: std_logic_vector(127 downto 0);
begin
	device <= addr_i(15 downto 0);
	data_access <= '1' when addr_i(31 downto 24) = std_logic_vector(to_unsigned(SOC_SEGMENT, 8)) and
				addr_i(23 downto 16) = std_logic_vector(to_unsigned(SOC_CLASS, 8))
				else '0';
	data_access_o <= data_access;
	data_o <= data_read_wrapper;
	
	-- MMIO register read operations
	process (clk_i, rst_i, device)
	begin
		if rst_i = '1' then
			data_read_wrapper <= (others => '0');
		elsif clk_i'event and clk_i = '1' then
			if (data_access = '1') then
				case device(7 downto 4) is
					when "0000" =>		-- control	0xe2000000
						data_read_wrapper <= x"0000" & "0000" & control;
					when "0001" =>		-- data[0]	0xe2000010
						data_read_wrapper <= data(127 downto 96);
					when "0010" =>		-- data[1]	0xe2000020
						data_read_wrapper <= data(95 downto 64);
					when "0011" =>		-- data[2]	0xe2000030
						data_read_wrapper <= data(63 downto 32);
					when "0100" =>		-- data[3]	0xe2000040
						data_read_wrapper <= data(31 downto 0);
					when others =>
						data_read_wrapper <= (others => '0');
				end case;
			end if;
		end if;
	end process;

	-- MMIO register write operations
	process (clk_i, rst_i, device)
	begin
		if rst_i = '1' then
			control <= (others => '0');
			data <= (others => '0');
		elsif clk_i'event and clk_i = '1' then
			if (data_access = '1' and data_w_i = '1') then
				case device(7 downto 4) is
					when "0000" =>		-- control	0xe2000000
						control <= data_i(11 downto 0);
					when "0001" =>		-- data[0]	0xe2000010
						data(127 downto 96) <= data_i;
					when "0010" =>		-- data[1]	0xe2000020
						data(95 downto 64) <= data_i;
					when "0011" =>		-- data[2]	0xe2000030
						data(63 downto 32) <= data_i;
					when "0100" =>		-- data[3]	0xe2000040
						data(31 downto 0) <= data_i;
					when others =>
				end case;
			end if;
		end if;
	end process;
	
	-- user logic ...

end if_arch;
