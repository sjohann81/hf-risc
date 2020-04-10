library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_unsigned.all;
use ieee.std_logic_arith.all;

entity mul is
	generic (
		mul_width: integer := 32
	);
	port (	clock: in std_logic;
		reset: in std_logic;
		trg: in std_logic;
		sig0: in std_logic;
		sig1: in std_logic;
		i0: in std_logic_vector(mul_width-1 downto 0);
		i1: in std_logic_vector(mul_width-1 downto 0);
		o0: out std_logic_vector(mul_width-1 downto 0);
		o1: out std_logic_vector(mul_width-1 downto 0);
		rdy: out std_logic
	);
end mul;

architecture mul_arch of mul is
	type states is (start, multiply, done);
	signal current_state, next_state: states;
	signal reg: std_logic_vector(mul_width*2 downto 0);
	signal i0_reg: std_logic_vector(mul_width-1 downto 0);
	signal a, add_sub: std_logic_vector(mul_width downto 0);
	signal sign, rdy_sig: std_logic;
	signal counter: std_logic_vector(4 downto 0);
begin
	process(clock, reset)
	begin
		if reset = '1' then
			rdy_sig <= '1';
			reg <= (others => '0');
		elsif clock'event and clock = '1' then
			case current_state is
				when start =>
					i0_reg <= i0;
					reg(mul_width*2 downto mul_width+1) <= (others => '0');
--					if (sig1 = '1') then
--						reg(mul_width downto 0) <= i1(mul_width-1) & i1;
--					else
						reg(mul_width downto 0) <= '0' & i1;
--					end if;
					counter <= (others => '0');
				when multiply =>
					rdy_sig <= '0';
					if (reg(0) = '1') then
						reg <= ((sig0 and add_sub(mul_width)) & add_sub & reg(mul_width-1 downto 1));
						counter <= counter + 1;
					elsif (reg(3 downto 0) = "0000" and counter < mul_width-4) then
						reg <= (reg(mul_width*2) & reg(mul_width*2) & reg(mul_width*2) & reg(mul_width*2) & reg((mul_width*2) downto 4));
						counter <= counter + 4;
					else
						reg <= (reg(mul_width*2) & reg((mul_width*2) downto 1));
						counter <= counter + 1;
					end if;
				when done =>
					rdy_sig <= '1';
				when others => null;
			end case;
		end if;
	end process;

	process(clock, reset)
	begin
		if reset = '1' then
			current_state <= start;
		elsif clock'event and clock = '1' then
			current_state <= next_state;
		end if;
	end process;

	process(current_state, counter, trg)
	begin
		case current_state is
			when start =>
				if trg = '1' then
					next_state <= multiply;
				else
					next_state <= start;
				end if;
			when multiply =>
				if counter = mul_width-1 then
					next_state <= done;
				else
					next_state <= multiply;
				end if;
			when done =>
				next_state <= start;
		end case;
	end process;

	sign <= i0_reg(mul_width-1) when sig0 = '1' else '0';
	a <= sign & i0_reg;

	add_sub <= reg(mul_width*2 downto mul_width) - a when (counter = mul_width-1) and sig0 = '1' else
			reg(mul_width*2 downto mul_width) + a;

	o1 <= reg((mul_width*2)-1 downto mul_width);
--	o1 <= not reg((mul_width*2)-1 downto mul_width) + 1 when sig1 = '1' and i0_reg(mul_width-1) = '1' else reg((mul_width*2)-1 downto mul_width);
	o0 <= reg(mul_width-1 downto 0);

	rdy <= rdy_sig;

end mul_arch;
