library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_unsigned.all;

entity reg_bank is
	port (	clock:		in std_logic;
		read_reg1:	in std_logic_vector(3 downto 0);
		read_reg2:	in std_logic_vector(3 downto 0);
		write_reg:	in std_logic_vector(3 downto 0);
		wreg:		in std_logic;
		write_data:	in std_logic_vector(31 downto 0);
		read_data1:	out std_logic_vector(31 downto 0);
		read_data2:	out std_logic_vector(31 downto 0)
	);
end reg_bank;

architecture arch_reg_bank of reg_bank is
	type bank is array(0 to 15) of std_logic_vector(31 downto 0);
	signal registers: bank := (others => (others => '0'));
begin
	process(clock, write_reg, wreg, write_data, read_reg1, read_reg2, registers)
	begin
		if clock'event and clock = '1' then
			if write_reg /= "00000" and wreg = '1' then
				registers(conv_integer(write_reg)) <= write_data;
			end if;
		end if;
	end process;

	read_data1 <= registers(conv_integer(read_reg1)) when read_reg1 /= "00000" else (others => '0');
	read_data2 <= registers(conv_integer(read_reg2)) when read_reg2 /= "00000" else (others => '0');
end arch_reg_bank;

