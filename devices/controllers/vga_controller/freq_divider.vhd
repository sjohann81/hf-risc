library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_arith.all;
use ieee.std_logic_unsigned.all;

entity freq_div is
	port (
		clk_i : in std_logic;
		rst_i : in std_logic;
		clk_o : out std_logic
	);
end freq_div;

architecture freq_div_arch of freq_div is
	signal count : std_logic_vector(1 downto 0);
begin
	process(clk_i, rst_i)
	begin
		if rst_i = '1' then
			count <= (others => '0');
		elsif rising_edge(clk_i) then
			count <= count + 1;
			
			if count = "11" then
				clk_o <= '1';
			else
				clk_o <= '0';
			end if;
		end if;
	end process;
	
end freq_div_arch;
