library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.STD_LOGIC_ARITH.ALL;
use IEEE.STD_LOGIC_UNSIGNED.ALL;
use ieee.std_logic_textio.all;
use std.textio.all;

entity bram is
	generic(memory_file : string := "code.txt";
		data_width: integer := 8;			-- data width (fixed)
		address_width: integer := 16;			-- address width
		bank: integer := 0);					-- memory bank (0,1,2,3)
	port(
	clk : in std_logic;					--clock
	addr : in std_logic_vector(address_width - 1 downto 2);		--address bus
	cs_n : in std_logic;					--chip select
	we_n : in std_logic;					--write enable
	data_i: in std_logic_vector(data_width - 1 downto 0);	--write data bus
	data_o: out std_logic_vector(data_width - 1 downto 0)	--read data bus
	);
end bram;

architecture memory of bram is

type ram is array(2 ** address_width -1 downto 0) of std_logic_vector(data_width - 1 downto 0);
signal ram1 : ram := (others => (others => '0'));

begin
	process(clk)
		variable data : std_logic_vector(data_width*4 -1 downto 0); 
		variable index : natural := 0;
		file load_file : text open read_mode is "code.txt";
		variable hex_file_line : line;
	begin
		--Load in the ram executable image
		if index = 0 then
			while not endfile(load_file) loop
				readline(load_file, hex_file_line);
				hread(hex_file_line, data);
				ram1(conv_integer(index)) <= data(((bank+1)*data_width)-1 downto bank*data_width);
				index := index + 1;
			end loop;
		end if;
		
		if (clk'event and clk = '1') then
 			if(cs_n = '0') then 
 				if(we_n = '0') then
 					ram1(conv_integer(addr(address_width -1 downto 2))) <= data_i;
				else
					data_o <= ram1(conv_integer(addr(address_width -1 downto 2)));
				end if;
			end if;
 		end if;	
		 
	end process;

end memory;

