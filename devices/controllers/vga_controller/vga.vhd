library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_arith.all;
use ieee.std_logic_unsigned.all;

entity vga_controller is
	port (
		-- board 100MHz clock source
		board_clk_i : in std_logic;
		-- bus interface signals
		clk_i : in std_logic;
		rst_i : in std_logic;
		addr_i : in std_logic_vector(14 downto 0);
		we_i : in std_logic;
		data_i : in std_logic_vector(7 downto 0);
		data_o : out std_logic_vector(7 downto 0);
		creset_o : out std_logic;
		-- VGA signals
		vsync : out std_logic;
		hsync : out std_logic;
		red : out std_logic_vector(3 downto 0);
		green : out std_logic_vector(3 downto 0);
		blue : out std_logic_vector(3 downto 0)
	);
end vga_controller;

architecture arch_vga_controller of vga_controller is 
	signal clk_div, wen : std_logic;
	signal vsync_enable, reset, reset_dly, hreset, vreset : std_logic;
	signal r, g, b, i : std_logic;
	signal addr : std_logic_vector(14 downto 0);
	signal data : std_logic_vector(7 downto 0);
	signal pline, pcol : std_logic_vector(9 downto 0);
	signal pixel_line, pixel_col : std_logic_vector(14 downto 0);
	signal pixel_data, pixel_data_s : std_logic_vector(3 downto 0);
	signal pixel_addr : std_logic_vector(14 downto 0);
	signal reset_r : std_logic;
begin

	freq_div : entity work.freq_div			-- 100 MHz to 25 MHz, divide by 4
	port map (
		clk_i => board_clk_i,
		rst_i => rst_i,
		clk_o => clk_div
	);
	
	hsync_gen : entity work.hsync_generator
	port map(
		clk_i => board_clk_i,
		rst_i => rst_i,
		enable_i => clk_div,
		hsync_o => hsync,
		venable_o => vsync_enable,
		reset_o => hreset,
		count_o => pcol
	);
	
	vsync_gen : entity work.vsync_generator
	port map (
		clk_i => board_clk_i,
		rst_i => rst_i,
		enable_i => vsync_enable,
		vsync_o => vsync,
		reset_o => vreset,
		count_o => pline
	);

	reset <= hreset or vreset;
	
	addr <= pixel_addr when reset = '0' else addr_i;
	wen <= '0' when we_i = '1' and reset = '1' else '1';
	data_o <= data;
	creset_o <= reset;

	-- 32kB VRAM for 300x218 pixels, 4bpp
	vram: entity work.vram
	generic map (	DATA_WIDTH => 8,
			ADDR_WIDTH => 15)
	port map (
		clk_i => clk_i,
		addr_i => addr,
		cs_n_i => '0',
		we_n_i => wen,
		data_i => data_i,
		data_o => data
	);
		
	process(clk_div, rst_i)
	begin
		if rst_i = '1' then
			reset_r <= '1';
		elsif rising_edge(clk_div) then
			reset_r <= reset;
		end if;
	end process;
	

	-- Since we are working with 25MHz pixel clock (instead of 25.175MHz)
	-- it is not possible to work with a perfect 640x480 or 320x240 resolution.
	-- Instead of 640 pixels for each line, we have effectively 632 pixels. As
	-- we are aiming at quarter resolution (320x240 instead of 640x480) to
	-- reduce BRAM usage (VRAM will be 32kB), resolution is 300x218 pixels
	-- at 4bpp (16 colors).
	process(clk_div, rst_i, hreset, vreset, pline)
	begin
		if rst_i = '1' then
			pixel_line <= (others => '0');
			pixel_col <= (others => '0');
			pixel_data <= (others => '0');
		elsif rising_edge(clk_div) then
			if hreset = '1' then
				pixel_col <= (others => '0');
			else
				pixel_col <= pixel_col + 1;
			end if;
			
			if vreset = '1' then
				pixel_line <= (others => '0');
			else
				if pixel_col = 599 and pline(0) = '1' then
					pixel_line <= pixel_line + 150;
				end if;
			end if;
			
			pixel_data <= pixel_data_s;
		end if;
	end process;

	pixel_addr <= pixel_line + ("00" & pixel_col(14 downto 2));
	pixel_data_s <= data(7 downto 4) when pixel_col(1 downto 0) > "01" else data(3 downto 0);

	i <= pixel_data(3) and not reset_r;
	r <= pixel_data(2) and not reset_r;
	g <= pixel_data(1) and not reset_r;
	b <= pixel_data(0) and not reset_r;

	red <= r & i & r & i;
	green <= g & i & g & i;
	blue <= b & i & b & i;

end arch_vga_controller;
