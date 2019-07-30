-- file:          minimal_soc.vhd
-- description:   basic SoC with peripherals
-- date:          01/2019
-- author:        Sergio Johann Filho <sergio.filho@pucrs.br>
--
-- Very simple configuration for a minimal SoC. Only a single GPIO port
-- a counter and timer are included in this version.

library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_unsigned.all;
use ieee.std_logic_arith.all;

entity peripherals is
	port (
		clk_i: in std_logic;
		rst_i: in std_logic;
		addr_i: in std_logic_vector(31 downto 0);
		data_i: in std_logic_vector(31 downto 0);
		data_o: out std_logic_vector(31 downto 0);
		sel_i: in std_logic;
		wr_i: in std_logic;
		irq_o: out std_logic;
		gpioa_in: in std_logic_vector(7 downto 0);
		gpioa_out: out std_logic_vector(7 downto 0);
		gpioa_ddr: out std_logic_vector(7 downto 0)
	);
end peripherals;

architecture peripherals_arch of peripherals is
	signal segment: std_logic_vector(3 downto 0);
	signal class: std_logic_vector(3 downto 0);
	signal device: std_logic_vector(5 downto 0);
	signal funct: std_logic_vector(3 downto 0);

	signal paaltcfg0, s0cause, gpiocause, gpiocause_inv, gpiomask, timercause, timercause_inv, timermask: std_logic_vector(3 downto 0);
	signal paddr, paout, pain, pain_inv, pain_mask: std_logic_vector(7 downto 0);
	signal timer0: std_logic_vector(31 downto 0);
	signal timer1, timer1_ctc, timer1_ocr: std_logic_vector(15 downto 0);
	signal timer1_pre: std_logic_vector(2 downto 0);
	signal timer1_set: std_logic;
	signal int_gpio, int_timer: std_logic;
	signal int_gpioa, int_timer1_ocr, int_timer1_ctc, tmr1_pulse, tmr1_dly, tmr1_dly2: std_logic;
	signal paalt0: std_logic;

begin
	segment <= addr_i(27 downto 24);
	class <= addr_i(19 downto 16);
	device <= addr_i(15 downto 10);
	funct <= addr_i(7 downto 4);

	irq_o <= '1' when s0cause /= "0000" else '0';
	s0cause <= '0' & int_timer & int_gpio & '0';

	int_gpio <= '1' when ((gpiocause xor gpiocause_inv) and gpiomask) /= "0000" else '0';
	gpiocause <= "000" & int_gpioa;
	int_gpioa <= '1' when ((pain xor pain_inv) and pain_mask) /= "0000" else '0';

	int_timer <= '1' when ((timercause xor timercause_inv) and timermask) /= "0000" else '0';
	timercause <= int_timer1_ocr & int_timer1_ctc & timer0(18) & timer0(16);

	pain <= gpioa_in;
	gpioa_out <= paout(7 downto 1) & paalt0;
	gpioa_ddr <= paddr;

	paalt0 <= int_timer1_ctc when paaltcfg0(1 downto 0) = "01" else int_timer1_ocr when paaltcfg0(1 downto 0) = "10" else paout(0);



	-- address decoder, read from peripheral registers
	process(clk_i, rst_i, segment, class, device, funct)
	begin
		if rst_i = '1' then
			data_o <= (others => '0');
		elsif clk_i'event and clk_i = '1' then
			if sel_i = '1' then
				case segment is
				when "0001" =>
					case class is
					when "0000" =>							-- Segment 0
						case device is
						when "000001" =>					-- S0CAUSE		(RO)
							data_o <= x"0000000" & s0cause;
						when "010000" =>					-- PAALTCFG0		(RW)
							data_o <= x"0000000" & paaltcfg0;
						when others =>
							data_o <= (others => '0');
						end case;
					when "0001" =>							-- GPIO
						case device is
						when "000001" =>					-- GPIOCAUSE		(RO)
							data_o <= x"0000000" & gpiocause;
						when "000010" =>					-- GPIOCAUSE_INV	(RW)
							data_o <= x"0000000" & gpiocause_inv;
						when "000011" =>					-- GPIOMASK		(RW)
							data_o <= x"0000000" & gpiomask;
						when "010000" =>					-- PORTA
							case funct is
							when "0000" =>					-- PADDR		(RW)
								data_o <= x"000000" & paddr;
							when "0001" =>					-- PAOUT		(RW)
								data_o <= x"000000" & paout;
							when "0010" =>					-- PAIN			(RO)
								data_o <= x"000000" & pain;
							when "0011" =>					-- PAIN_INV		(RW)
								data_o <= x"000000" & pain_inv;
							when "0100" =>					-- PAIN_MASK		(RW)
								data_o <= x"000000" & pain_mask;
							when others =>
								data_o <= (others => '0');
							end case;
						when others =>
							data_o <= (others => '0');
						end case;
					when "0010" =>							-- timers
						case device is
						when "000001" =>					-- TIMERCAUSE		(RO)
							data_o <= x"0000000" & timercause;
						when "000010" =>					-- TIMERCAUSE_INV	(RW)
							data_o <= x"0000000" & timercause_inv;
						when "000011" =>					-- TIMERMASK		(RW)
							data_o <= x"0000000" & timermask;
						when "010000" =>					-- TIMER0		(RO)
							data_o <= timer0;
						when "010001" =>					-- TIMER1
							case funct is
							when "0000" =>					-- TIMER1		(RW)
								data_o <= x"0000" & timer1;
							when "0001" =>					-- TIMER1_PRE		(RW)
								data_o <= x"0000000" & '0' & timer1_pre;
							when "0010" =>					-- TIMER1_CTC		(RW)
								data_o <= x"0000" & timer1_ctc;
							when "0011" =>					-- TIMER1_OCR		(RW)
								data_o <= x"0000" & timer1_ocr;
							when others =>
								data_o <= (others => '0');
							end case;
						when others =>
							data_o <= (others => '0');
						end case;
					when others =>
						data_o <= (others => '0');
					end case;
				when others =>
					data_o <= (others => '0');
				end case;
			end if;
		end if;
	end process;

	-- peripheral register logic, write to peripheral registers
	process(clk_i, rst_i, segment, class, device, funct, tmr1_pulse)
	begin
		if rst_i = '1' then
			paaltcfg0 <= (others => '0');
			gpiocause_inv <= (others => '0');
			gpiomask <= (others => '0');
			paout <= (others => '0');
			pain_inv <= (others => '0');
			pain_mask <= (others => '0');
			paddr <= (others => '0');
			timercause_inv <= (others => '0');
			timermask <= (others => '0');
			timer0 <= (others => '0');
			timer1 <= (others => '0');
			timer1_set <= '0';
			timer1_pre <= (others => '0');
			timer1_ctc <= (others => '1');
			timer1_ocr <= (others => '0');
			int_timer1_ctc <= '0';
		elsif clk_i'event and clk_i = '1' then
			if sel_i = '1' and wr_i = '1' then
				case segment is
				when "0001" =>
					case class is
					when "0000" =>							-- Segment 0
						case device is
						when "010000" =>					-- PAALTCFG0		(RW)
							paaltcfg0 <= data_i(3 downto 0);
						when others =>
						end case;
					when "0001" =>							-- GPIO
						case device is
						when "000010" =>					-- GPIOCAUSE_INV	(RW)
							gpiocause_inv <= data_i(3 downto 0);
						when "000011" =>					-- GPIOMASK		(RW)
							gpiomask <= data_i(3 downto 0);
						when "010000" =>					-- PORTA
							case funct is
							when "0000" =>					-- PADDR		(RW)
								paddr <= data_i(7 downto 0);
							when "0001" =>					-- PAOUT		(RW)
								paout <= data_i(7 downto 0);
							when "0011" =>					-- PAIN_INV		(RW)
								pain_inv <= data_i(7 downto 0);
							when "0100" =>					-- PAIN_MASK		(RW)
								pain_mask <= data_i(7 downto 0);
							when others =>
							end case;
						when others =>
						end case;
					when "0010" =>							-- timers
						case device is
						when "000010" =>					-- TIMERCAUSE_INV	(RW)
							timercause_inv <= data_i(3 downto 0);
						when "000011" =>					-- TIMERMASK		(RW)
							timermask <= data_i(3 downto 0);
						when "010001" =>					-- TIMER1
							case funct is
							when "0000" =>					-- TIMER1		(RW)
								if data_i(31) = '1' then
									timer1_set <= '1';
								end if;
								if timer1_set = '1' then
									timer1 <= data_i(15 downto 0);
									timer1_set <= '0';
								end if;
							when "0001" =>					-- TIMER1_PRE		(RW)
								timer1_pre <= data_i(2 downto 0);
							when "0010" =>					-- TIMER1_CTC		(RW)
								timer1_ctc <= data_i(15 downto 0);
							when "0011" =>					-- TIMER1_OCR		(RW)
								timer1_ocr <= data_i(15 downto 0);
							when others =>
							end case;
						when others =>
						end case;
					when others =>
					end case;
				when others =>
				end case;
			end if;

			timer0 <= timer0 + 1;

			if tmr1_pulse = '1' then
				if (timer1 /= timer1_ctc) then
					if timer1_set = '0' then
						timer1 <= timer1 + 1;
					end if;
				else
					int_timer1_ctc <= not int_timer1_ctc;
					timer1 <= (others => '0');
				end if;
			end if;
		end if;
	end process;

	process(clk_i, rst_i)				-- TIMER1 prescaler
	begin
		if rst_i = '1' then
			tmr1_dly <= '0';
			tmr1_dly2 <= '0';
		elsif clk_i'event and clk_i = '1' then
			case timer1_pre is
			when "001" =>
				tmr1_dly <= timer0(2);	-- /4
			when "010" =>
				tmr1_dly <= timer0(4);	-- /16
			when "011" =>
				tmr1_dly <= timer0(6);	-- /64
			when "100" =>
				tmr1_dly <= timer0(8);	-- /256
			when "101" =>
				tmr1_dly <= timer0(10);	-- /1024
			when "110" =>
				tmr1_dly <= timer0(12);	-- /4096
			when "111" =>
				tmr1_dly <= timer0(14);	-- /16384
			when others =>
				tmr1_dly <= timer0(0);	-- /1
			end case;

			tmr1_dly2 <= tmr1_dly;
		end if;
	end process;

	tmr1_pulse <= '1' when tmr1_dly /= tmr1_dly2 else '0';
	int_timer1_ocr <= '1' when timer1 < timer1_ocr else '0';

end peripherals_arch;
