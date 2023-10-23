library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_arith.all;
use ieee.std_logic_unsigned.all;

entity vram is
	generic(DATA_WIDTH: integer := 8;
		ADDR_WIDTH: integer := 12);
	port(
		clk_i : in std_logic;					--clock
		addr_i : in std_logic_vector(ADDR_WIDTH-1 downto 0);	--address bus
		cs_n_i : in std_logic;					--chip select
		we_n_i : in std_logic;					--write enable
		data_i : in std_logic_vector(DATA_WIDTH-1 downto 0);	--write data bus
		data_o : out std_logic_vector(DATA_WIDTH-1 downto 0)	--read data bus
	);
end vram;

architecture vram_arch of vram is
	type ram is array(2 ** ADDR_WIDTH - 1 downto 0) of std_logic_vector(DATA_WIDTH-1 downto 0);
	signal ram1 : ram := (others => (others => '0'));
begin
	process(clk_i)
	begin
		if (clk_i'event and clk_i = '1') then
 			if (cs_n_i = '0') then 
 				if (we_n_i = '0') then
 					ram1(conv_integer(addr_i(ADDR_WIDTH-1 downto 0))) <= data_i;
				else
					data_o <= ram1(conv_integer(addr_i(ADDR_WIDTH-1 downto 0)));
				end if;
			end if;
 		end if;	
	end process;

end vram_arch;

