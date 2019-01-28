library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_textio.all;
use ieee.std_logic_unsigned.all;
use std.textio.all;

entity uart is
	port(	clk		: in std_logic;
		reset		: in std_logic;
		divisor		: in std_logic_vector(11 downto 0);
		enable_read	: in std_logic;
		enable_write	: in std_logic;
		data_in		: in std_logic_vector(7 downto 0);
		data_out	: out std_logic_vector(7 downto 0);
		uart_read	: in std_logic;
		uart_write	: out std_logic;
		busy_write	: out std_logic;
		data_avail	: out std_logic
	);
end;

architecture logic of uart is
	signal delay_write_reg : std_logic_vector(11 downto 0);
	signal bits_write_reg  : std_logic_vector(3 downto 0);
	signal data_write_reg  : std_logic_vector(8 downto 0);
	signal delay_read_reg  : std_logic_vector(11 downto 0);
	signal bits_read_reg   : std_logic_vector(3 downto 0);
	signal data_read_reg   : std_logic_vector(7 downto 0);
	signal data_save_reg   : std_logic_vector(8 downto 0);
	signal busy_write_sig  : std_logic;
	signal read_value_reg  : std_logic_vector(7 downto 0);
	signal uart_read2      : std_logic;

begin

	uart_proc:
	process(clk, reset, enable_read, enable_write, data_in, data_write_reg, bits_write_reg,
		delay_write_reg, data_read_reg, bits_read_reg, delay_read_reg, data_save_reg,
		read_value_reg, uart_read2, busy_write_sig, uart_read)
	begin
		uart_read2 <= read_value_reg(read_value_reg'length - 1);

		if reset = '1' then
			data_write_reg  <= x"00" & '1';
			bits_write_reg  <= "0000";
			delay_write_reg <= (others => '0');
			read_value_reg  <= (others => '1');
			data_read_reg   <= (others => '0');
			bits_read_reg   <= "0000";
			delay_read_reg  <= (others => '0');
			data_save_reg   <= (others => '0');
		elsif clk'event and clk = '1' then
			-- UART write
			if bits_write_reg = "0000" then							--nothing left to write?
				if enable_write = '1' then
					delay_write_reg <= (others => '0');				--delay before next bit
					bits_write_reg <= "1010";					--number of bits to write
					data_write_reg <= data_in & '0';				--remember data & start bit
				end if;
			else
				if delay_write_reg /= divisor then
					delay_write_reg <= delay_write_reg + 1;				--delay before next bit
				else
					delay_write_reg <= (others => '0');				--reset delay
					bits_write_reg <= bits_write_reg - 1;				--bits left to write
					data_write_reg <= '1' & data_write_reg(8 downto 1);
				end if;
			end if;

			-- average uart_read signal
			if uart_read = '1' then
				if read_value_reg /= x"ff" then
					read_value_reg <= read_value_reg + 1;
				end if;
			else
				if read_value_reg /= x"00" then
					read_value_reg <= read_value_reg - 1;
				end if;
			end if;

			-- UART read
			if delay_read_reg = x"0000" then						--done delay for read?
				if bits_read_reg = "0000" then						--nothing left to read?
					if uart_read2 = '0' then					--wait for start bit
						delay_read_reg <= '0' & divisor(11 downto 1);		--half period
						bits_read_reg <= "1001";				--bits left to read
					end if;
				else
					delay_read_reg <= divisor;					--initialize delay
					bits_read_reg <= bits_read_reg - 1;				--bits left to read
					data_read_reg <= uart_read2 & data_read_reg(7 downto 1);
				end if;
			else
				delay_read_reg <= delay_read_reg - 1;					--delay
			end if;

			if bits_read_reg = "0000" and delay_read_reg = divisor then
				data_save_reg <= '1' & data_read_reg;
			elsif enable_read = '1' then
				data_save_reg(8) <= '0';						--data_available
			end if;
		end if;

		if bits_write_reg /= "0000" then
			busy_write_sig <= '1';
		else
			busy_write_sig <= '0';
		end if;

		uart_write <= data_write_reg(0);
		busy_write <= busy_write_sig;
		data_avail <= data_save_reg(8);
		data_out <= data_save_reg(7 downto 0);
	end process;
end;
