library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_arith.all;
use ieee.std_logic_unsigned.all;

entity vsync_generator is
	port (
		clk_i : in std_logic;
		rst_i : in std_logic;
		enable_i : in std_logic;
		vsync_o : out std_logic;
		reset_o : out std_logic;
		count_o : out std_logic_vector(9 downto 0)
	);
end vsync_generator;

architecture vsync_generator_arch of vsync_generator is
	constant vsync_pulse_time : integer := 521;	-- 525
	constant vpulse_width : integer := 2;
	constant vfront_porch : integer := 25;		--10;
	constant vback_porch : integer := 58;		--33;
	signal count : std_logic_vector(9 downto 0);
begin
	process(clk_i, rst_i, enable_i)
	begin
		if rst_i = '1' then
			count <= (others => '0');
		elsif rising_edge(clk_i) then
			if count = vsync_pulse_time then
				count <= (others => '0');
			end if;

			if enable_i = '1' then 
				count <= count + 1;
			end if;
		end if;
	end process;
	
	process(count)
	begin
		if vpulse_width < count then
			vsync_o <= '1';
		else
			vsync_o <= '0';
		end if;

		if (vpulse_width + vback_porch) < count and count < (vsync_pulse_time - vfront_porch) then
			reset_o <= '0';
		else
			reset_o <= '1';
		end if;
	end process;
	
	count_o <= count;

end vsync_generator_arch;
