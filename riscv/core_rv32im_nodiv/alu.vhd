library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_unsigned.all;

entity alu is
	port (	clock:		in std_logic;
		reset:		in std_logic;
		op1:		in std_logic_vector(31 downto 0);
		op2:		in std_logic_vector(31 downto 0);
		alu_op:		in std_logic_vector(3 downto 0);
		result:		out std_logic_vector(31 downto 0);
		zero:		out std_logic;
		less_than:	out std_logic;
		alu_wait:	out std_logic
	);
end alu;

architecture arch_alu of alu is
	signal r, shift, mul_lo, mul_hi: std_logic_vector(31 downto 0);
	signal shift_op2: std_logic_vector(4 downto 0);
	signal addsub: std_logic_vector(32 downto 0);
	signal less, left, logical, mul_trg, mul_sig0, mul_sig1, mul_rdy, mul_rdy2, mul_hold: std_logic;
begin
	process(op1, op2, alu_op, addsub, less, shift_op2, shift, mul_lo, mul_hi)
	begin
		case alu_op is
			when "0000" => r <= op1 and op2;
			when "0001" => r <= op1 or op2;
			when "0010" => r <= op1 xor op2;
			when "0100" | "0101" => r <= addsub(31 downto 0);
			when "0110" => r <= op2;
			when "0111" | "1000" => r <= x"0000000" & "000" & less;
			when "1001" | "1010" | "1011" => r <= shift;
			when "1100" => r <= mul_lo;
			when others => r <= mul_hi;
		end case;
	end process;

	result <= r;

	addsub <= ('0' & op1) - ('0' & op2) when alu_op > "0100" else ('0' & op1) + ('0' & op2);
	less <= addsub(32) when op1(31) = op2(31) or alu_op = "1000" else op1(31);
	less_than <= less;
	zero <= not (r(31) or r(30) or r(29) or r(28) or r(27) or r(26) or r(25) or r(24) or
			r(23) or r(22) or r(21) or r(20) or r(19) or r(18) or r(17) or r(16) or
			r(15) or r(14) or r(13) or r(12) or r(11) or r(10) or r(9) or r(8) or
			r(7) or r(6) or r(5) or r(4) or r(3) or r(2) or r(1) or r(0));

	shift_op2 <= op2(4 downto 0);

	left <= '1' when alu_op(1) = '0' else '0';
	logical <= '1' when alu_op(1 downto 0) /= "11" else '0';

	barrel_shifter: entity work.bshift
	port map(	left => left,
			logical => logical,
			shift => shift_op2,
			input => op1,
			output => shift
	);

	mul_trg <= '1' when alu_op >= "1100" and mul_rdy = '1' and mul_rdy2 = '1' else '0';
	mul_sig0 <= '1' when alu_op(1) = '0' else '0';
	mul_sig1 <= '1' when alu_op(1 downto 0) = "10" else '0';
	alu_wait <= mul_trg or mul_hold or not mul_rdy;

	process(clock, reset)
	begin
		if reset = '1' then
			mul_hold <= '0';
			mul_rdy2 <= '0';
		elsif clock'event and clock = '1' then
			mul_hold <= mul_trg;
			mul_rdy2 <= mul_rdy;
		end if;
	end process;

	multiplier: entity work.mul
	port map(	clock => clock,
			reset => reset,
			trg => mul_trg,
			sig0 => mul_sig0,
			sig1 => mul_sig1,
			i0 => op1,
			i1 => op2,
			o0 => mul_lo,
			o1 => mul_hi,
			rdy => mul_rdy
	);

end arch_alu;

