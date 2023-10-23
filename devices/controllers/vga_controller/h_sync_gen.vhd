library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_arith.all;
use ieee.std_logic_unsigned.all;

entity hsync_generator is
	port (
		clk_i : in std_logic;
		rst_i : in std_logic;
		enable_i : in std_logic;
		hsync_o : out std_logic;
		venable_o : out std_logic;
		reset_o : out std_logic;
		count_o	: out std_logic_vector(9 downto 0)
	);
end hsync_generator;

architecture hsync_generator_arch of hsync_generator is
	constant hsync_pulse_time : integer := 794;	-- 800
	constant hpulse_width : integer := 96;
	constant hfront_porch : integer := 34;		--16;
	constant hback_porch : integer := 64;		--48;
	signal count : std_logic_vector(9 downto 0);
	signal reset_s : std_logic;
begin
	process(clk_i, rst_i, enable_i)
	begin
		if rst_i = '1' then
			count <= (others => '0');
		elsif rising_edge(clk_i) then
			if reset_s = '1' then
				count <= (others => '0');
			elsif enable_i = '1' then 
				count <= count + 1;
			end if;
		end if;
	end process;
	
	process(count)
	begin
		if count = hsync_pulse_time then
			venable_o <= '1';
			reset_s <= '1';
		else
			venable_o <= '0';
			reset_s <= '0';
		end if;
		
		if hpulse_width < count then
			hsync_o <= '1';
		else
			hsync_o <= '0';
		end if;

		if (hpulse_width + hback_porch) < count and count < (hsync_pulse_time - hfront_porch) then
			reset_o <= '0';
		else
			reset_o <= '1';
		end if;
	end process;
	
	count_o <= count;

end hsync_generator_arch;
