library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

entity if_vga is
	generic (
		SOC_SEGMENT : integer := 	16#e2#;		-- SoC segment address is 0xe2
		SOC_CLASS : integer := 		16#80#;		-- Class address is 0x80
		AXIS_DATA_WIDTH : integer := 8
	);
	port (
		-- CPU interface signals
		clk_i : in std_logic;
		rst_i : in std_logic;
		addr_i : in std_logic_vector(31 downto 0);
		data_i : in std_logic_vector(31 downto 0);
		data_o : out std_logic_vector(31 downto 0);
		data_access_o : out std_logic;
		data_w_i : in std_logic;
		-- board 100MHz clock source
		board_clk_i : in std_logic;
		-- VGA interface signals
		vsync : out std_logic;
		hsync : out std_logic;
		red : out std_logic_vector(3 downto 0);
		green : out std_logic_vector(3 downto 0);
		blue : out std_logic_vector(3 downto 0)
	);
end if_vga;

architecture if_arch of if_vga is
	signal device : std_logic_vector(15 downto 0);
	signal data_access, data_access_dly, we, creset : std_logic;
	signal data_read : std_logic_vector(31 downto 0);
	signal data_in, data_out : std_logic_vector(7 downto 0);
	signal vga_addr : std_logic_vector(14 downto 0);
	signal vga_write : std_logic;
begin
	device <= addr_i(15 downto 0);
	data_access <= '1' when addr_i(31 downto 24) = std_logic_vector(to_unsigned(SOC_SEGMENT, 8)) and
				addr_i(23 downto 16) = std_logic_vector(to_unsigned(SOC_CLASS, 8))
				else '0';
	data_access_o <= data_access;
	data_o <= data_read;
	
	vga: entity work.vga_controller
	port map(
		board_clk_i => board_clk_i,
		clk_i => clk_i,
		rst_i => rst_i,
		
		addr_i => vga_addr,
		we_i => we,
		data_i => data_in,
		data_o => data_out,
		creset_o => creset,
		
		vsync => vsync,
		hsync => hsync,
		red => red,
		green => green,
		blue => blue
	);

	process(clk_i, rst_i, device)
	begin
		if rst_i = '1' then
			data_read <= (others => '0');
		elsif rising_edge(clk_i) then
			if (data_access = '1') then
				case device(6 downto 4) is
					when "001" =>
						data_read <= x"0000000" & "00" & vga_write & creset;
					when "010" =>
						data_read <= x"0000" & '0' & vga_addr;
					when "011" =>
						data_read <= x"000000" & data_out;
					when others =>
						data_read <= (others => '0');
				end case;
			end if;
		end if;
	end process;

	process(clk_i, rst_i, device)
	begin
		if rst_i = '1' then
			vga_addr <= (others => '0');
			vga_write <= '0';
			data_in <= (others => '0');
		elsif rising_edge(clk_i) then
			if (data_access = '1' and data_w_i = '1') then
				case device(6 downto 4) is
					when "001" =>
						vga_write <= data_i(1);
					when "010" =>
						vga_addr <= data_i(14 downto 0);
					when "011" =>
						data_in <= data_i(7 downto 0);
					when others =>
				end case;
			end if;
		end if;
	end process;

	we <= vga_write;
		
end if_arch;
