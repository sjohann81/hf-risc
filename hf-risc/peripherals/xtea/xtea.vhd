--XTEA encryption algorithm v0.3
--Sergio Johann Filho, 2016
--
--based on reference code (below) released into the public domain by David Wheeler and Roger Needham
--the code takes 64 bits of data in v[0] and v[1] and 128 bits of key in key[0] - key[3]
--recommended number of rounds is 32 (2 Feistel-network rounds are performed on each iteration).
--
--
--void encipher(uint32_t num_rounds, uint32_t v[2], uint32_t const key[4]){
--	uint32_t i;
--	uint32_t v0 = v[0], v1 = v[1], sum = 0, delta = 0x9E3779B9;
--
--	for (i = 0; i < num_rounds; i++){
--		v0 += (((v1 << 4) ^ (v1 >> 5)) + v1) ^ (sum + key[sum & 3]);
--		sum += delta;
--		v1 += (((v0 << 4) ^ (v0 >> 5)) + v0) ^ (sum + key[(sum>>11) & 3]);
--	}
--	v[0] = v0; v[1] = v1;
--}
--
--void decipher(uint32_t num_rounds, uint32_t v[2], uint32_t const key[4]){
--	uint32_t i;
--	uint32_t v0 = v[0], v1 = v[1], delta = 0x9E3779B9, sum = delta * num_rounds;
--
--	for (i = 0; i < num_rounds; i++){
--		v1 -= (((v0 << 4) ^ (v0 >> 5)) + v0) ^ (sum + key[(sum>>11) & 3]);
--		sum -= delta;
--		v0 -= (((v1 << 4) ^ (v1 >> 5)) + v1) ^ (sum + key[sum & 3]);
--	}
--	v[0] = v0; v[1] = v1;
--}

library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_unsigned.all;
use ieee.std_logic_arith.all;

entity xtea is
	generic (
		ROUNDS: integer := 32
	);
	port (	clock: in std_logic;
		reset: in std_logic;
		start: in std_logic;
		encrypt: in std_logic;
		key: in std_logic_vector(127 downto 0);
		input: in std_logic_vector(63 downto 0);
		output: out std_logic_vector(63 downto 0);
		ready: out std_logic
	);
end xtea;

architecture xtea_arch of xtea is
	type states is (idle, enc_step1, enc_step2, enc_step3, dec_step1, dec_step2, dec_step3, key_sel1, key_sel2, done);
	signal state: states;
	signal v0, v1, sum, delta, key_sel: std_logic_vector(31 downto 0);
	signal counter: std_logic_vector(7 downto 0);
begin

	delta <= x"9e3779b9";

	process(clock, reset)
	begin
		if reset = '1' then
			v0 <= (others => '0');
			v1 <= (others => '0');
			sum <= (others => '0');
			key_sel <= (others => '0');
			counter <= (others => '0');
			output <= (others => '0');
			ready <= '0';
		elsif clock'event and clock = '1' then
			case state is
				when idle =>
					ready <= '0';
					counter <= (others => '0');
				when key_sel1 =>
					case sum(1 downto 0) is
						when "00" =>
							key_sel <= key(127 downto 96);
						when "01" =>
							key_sel <= key(95 downto 64);
						when "10" =>
							key_sel <= key(63 downto 32);
						when "11" =>
							key_sel <= key(31 downto 0);
						when others => null;
					end case;
				when key_sel2 =>
					case sum(12 downto 11) is
						when "00" =>
							key_sel <= key(127 downto 96);
						when "01" =>
							key_sel <= key(95 downto 64);
						when "10" =>
							key_sel <= key(63 downto 32);
						when "11" =>
							key_sel <= key(31 downto 0);
						when others => null;
					end case;
				when enc_step1 =>
					v1 <= input(31 downto 0);
					v0 <= input(63 downto 32);
					sum <= (others => '0');
				when enc_step2 =>
					v0 <= v0 + ((((v1(27 downto 0) & "0000") xor ("00000" & v1(31 downto 5))) + v1) xor (sum + key_sel));
					sum <= sum + delta;
				when enc_step3 =>
					v1 <= v1 + ((((v0(27 downto 0) & "0000") xor ("00000" & v0(31 downto 5))) + v0) xor (sum + key_sel));
					counter <= counter + 1;
				when dec_step1 =>
					v1 <= input(31 downto 0);
					v0 <= input(63 downto 32);
					sum <= x"c6ef3720";
				when dec_step2 =>
					v1 <= v1 - ((((v0(27 downto 0) & "0000") xor ("00000" & v0(31 downto 5))) + v0) xor (sum + key_sel));
					sum <= sum - delta;
				when dec_step3 =>
					v0 <= v0 - ((((v1(27 downto 0) & "0000") xor ("00000" & v1(31 downto 5))) + v1) xor (sum + key_sel));
					counter <= counter + 1;
				when done =>
					output(63 downto 32) <= v0;
					output(31 downto 0) <= v1;
					ready <= '1';
				when others => null;
			end case;
		end if;
	end process;

	process(clock, reset, state, counter, start, encrypt)
	begin
		if reset = '1' then
			state <= idle;
		elsif clock'event and clock = '1' then
			case state is
				when idle =>
					if (start = '1') then
						if (encrypt = '1') then
							state <= enc_step1;
						else
							state <= dec_step1;
						end if;
					else
						state <= idle;
					end if;
				when key_sel1 => 
					if (encrypt = '1') then
						state <= enc_step2;
					else
						state <= dec_step3;
					end if;
				when key_sel2 =>
					if (encrypt = '1') then
						state <= enc_step3;
					else
						state <= dec_step2;
					end if;
				when enc_step1 => state <= key_sel1;
				when enc_step2 => state <= key_sel2;
				when enc_step3 =>
					if (counter < ROUNDS-1) then
						state <= key_sel1;
					else
						state <= done;
					end if;
				when dec_step1 => state <= key_sel2;
				when dec_step2 => state <= key_sel1;
				when dec_step3 =>
					if (counter < ROUNDS-1) then
						state <= key_sel2;
					else
						state <= done;
					end if;
				when done =>
					if (start = '1') then
						state <= done;
					else
						state <= idle;
					end if;
				when others => null;
			end case;
		end if;
	end process;
	
end xtea_arch;


