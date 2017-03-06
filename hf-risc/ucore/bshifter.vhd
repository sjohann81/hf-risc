library ieee;
use ieee.std_logic_1164.all;
 
entity bshift is				
	port (	left:		in std_logic;				-- '1' for left, '0' for right
         	logical:	in std_logic;				-- '1' for logical, '0' for arithmetic
		shift:		in std_logic_vector(4 downto 0);	-- shift count
		input:		in std_logic_vector (31 downto 0);
		output:		out std_logic_vector (31 downto 0)
	);
end entity bshift;
 
architecture logic of bshift is
	signal shift1l, shift2l, shift4l, shift8l, shift16l : std_logic_vector(31 downto 0);
	signal shift1r, shift2r, shift4r, shift8r, shift16r : std_logic_vector(31 downto 0);
	signal fill : std_logic_vector(31 downto 16);
begin
	fill <= (others => input(31)) when logical = '0' else x"0000";

	shift1l  <= input(30 downto 0) & '0' when shift(0) = '1' else input;
	shift2l  <= shift1l(29 downto 0) & "00" when shift(1) = '1' else shift1l;
	shift4l  <= shift2l(27 downto 0) & x"0" when shift(2) = '1' else shift2l;
	shift8l  <= shift4l(23 downto 0) & x"00" when shift(3) = '1' else shift4l;
	shift16l <= shift8l(15 downto 0) & x"0000" when shift(4) = '1' else shift8l;
 
	shift1r  <= fill(31) & input(31 downto 1) when shift(0) = '1' else input;
	shift2r  <= fill(31 downto 30) & shift1r(31 downto 2) when shift(1) = '1' else shift1r;
	shift4r  <= fill(31 downto 28) & shift2r(31 downto 4) when shift(2) = '1' else shift2r;
	shift8r  <= fill(31 downto 24) & shift4r(31 downto 8)  when shift(3) = '1' else shift4r;
	shift16r <= fill(31 downto 16) & shift8r(31 downto 16) when shift(4) = '1' else shift8r;
 
	output <= shift16r when left = '0' else shift16l;
end;

