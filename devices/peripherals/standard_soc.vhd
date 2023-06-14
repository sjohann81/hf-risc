-- file:          standard_soc.vhd
-- description:   standard SoC with peripherals
-- date:          08/2019
-- author:        Sergio Johann Filho <sergio.filho@pucrs.br>
--
-- Standard SoC configuration template for prototyping. Dual GPIO ports,
-- a counter, two timers, dual UARTs and dual SPIs are included in this version.

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
		gpioa_in: in std_logic_vector(15 downto 0);
		gpioa_out: out std_logic_vector(15 downto 0);
		gpioa_ddr: out std_logic_vector(15 downto 0);
		gpiob_in: in std_logic_vector(15 downto 0);
		gpiob_out: out std_logic_vector(15 downto 0);
		gpiob_ddr: out std_logic_vector(15 downto 0)
	);
end peripherals;

architecture peripherals_arch of peripherals is
	signal segment: std_logic_vector(3 downto 0);
	signal class: std_logic_vector(3 downto 0);
	signal device: std_logic_vector(5 downto 0);
	signal funct: std_logic_vector(3 downto 0);

	signal s0cause: std_logic_vector(4 downto 0);
	signal gpiocause, gpiocause_inv, gpiomask: std_logic_vector(3 downto 0);
	signal timercause, timercause_inv, timermask: std_logic_vector(7 downto 0);
	signal paddr, paout, pain, pain_inv, pain_mask: std_logic_vector(15 downto 0);
	signal pbddr, pbout, pbin, pbin_inv, pbin_mask: std_logic_vector(15 downto 0);

	signal paaltcfg0: std_logic_vector(31 downto 0);
	signal paalt: std_logic_vector(15 downto 0);	
	signal int_gpio, int_timer, int_gpioa, int_gpiob: std_logic;
	signal int_timer1_ocr, int_timer1_ctc, tmr1_pulse, tmr1_dly, tmr1_dly2: std_logic;
	signal timer0: std_logic_vector(31 downto 0);
	signal timer1, timer1_ctc, timer1_ocr: std_logic_vector(15 downto 0);
	signal timer1_pre: std_logic_vector(2 downto 0);
	signal timer1_set: std_logic;
	signal int_timer2_ocr, int_timer2_ctc, tmr2_pulse, tmr2_dly, tmr2_dly2: std_logic;
	signal timer2, timer2_ctc, timer2_ocr: std_logic_vector(15 downto 0);
	signal timer2_pre: std_logic_vector(2 downto 0);
	signal timer2_set: std_logic;

	signal int_uart: std_logic;
	signal uartcause, uartcause_inv, uartmask: std_logic_vector(3 downto 0);
	signal uart0_tx, uart0_rx, uart0_enable_w, uart0_enable_r, uart0_write_busy, uart0_data_avail: std_logic;
	signal uart0_data_read, uart0_data_write: std_logic_vector(7 downto 0);
	signal uart0_divisor: std_logic_vector(15 downto 0);
	signal uart1_tx, uart1_rx, uart1_enable_w, uart1_enable_r, uart1_write_busy, uart1_data_avail: std_logic;
	signal uart1_data_read, uart1_data_write: std_logic_vector(7 downto 0);
	signal uart1_divisor: std_logic_vector(15 downto 0);

	signal int_spi: std_logic;
	signal spicause, spicause_inv, spimask: std_logic_vector(3 downto 0);
	signal spi0_data_write, spi0_data_read: std_logic_vector(7 downto 0);
	signal spi0_clk_div: std_logic_vector(8 downto 0);
	signal spi0_data_valid, spi0_data_xfer, spi0_ssn, spi0_clk_i, spi0_clk_o, spi0_do, spi0_di: std_logic;
	signal spi1_data_write, spi1_data_read: std_logic_vector(7 downto 0);
	signal spi1_clk_div: std_logic_vector(8 downto 0);
	signal spi1_data_valid, spi1_data_xfer, spi1_ssn, spi1_clk_i, spi1_clk_o, spi1_do, spi1_di: std_logic;

begin
	segment <= addr_i(27 downto 24);
	class <= addr_i(19 downto 16);
	device <= addr_i(15 downto 10);
	funct <= addr_i(7 downto 4);

	irq_o <= '1' when s0cause /= "00000" else '0';
	s0cause <= int_spi & int_uart & int_timer & int_gpio & '0';

	int_gpio <= '1' when ((gpiocause xor gpiocause_inv) and gpiomask) /= "0000" else '0';
	gpiocause <= "00" & int_gpiob & int_gpioa;
	int_gpioa <= '1' when ((pain xor pain_inv) and pain_mask) /= "0000" else '0';
	int_gpiob <= '1' when ((pbin xor pbin_inv) and pbin_mask) /= "0000" else '0';

	pain <= gpioa_in(15 downto 0);
	gpioa_out <= paalt;
	gpioa_ddr <= paddr;

	pbin <= gpiob_in(15 downto 0);
	gpiob_out <= pbout;
	gpiob_ddr <= pbddr;

	int_timer <= '1' when ((timercause xor timercause_inv) and timermask) /= x"00" else '0';
	timercause <= timer2(15) & int_timer2_ocr & int_timer2_ctc & timer1(15) & int_timer1_ocr & int_timer1_ctc & timer0(18) & timer0(16);

	int_uart <= '1' when ((uartcause xor uartcause_inv) and uartmask) /= "0000" else '0';
	uartcause <= uart1_write_busy & uart1_data_avail & uart0_write_busy & uart0_data_avail;
	
	int_spi <= '1' when ((spicause xor spicause_inv) and spimask) /= "0000" else '0';
	spicause <= "00" & spi1_data_valid & spi0_data_valid;
	
	-- PORT A alternate config MUXes for outputs
	paalt(0) <= int_timer1_ctc when paaltcfg0(1 downto 0) = "01" else int_timer1_ocr when paaltcfg0(1 downto 0) = "10" else paout(0);
	paalt(1) <= int_timer2_ctc when paaltcfg0(1 downto 0) = "01" else int_timer2_ocr when paaltcfg0(1 downto 0) = "10" else spi1_clk_o when paaltcfg0(3 downto 2) = "11" else paout(1);
	paalt(2) <= uart0_tx when paaltcfg0(5 downto 4) = "01" else spi1_do when paaltcfg0(5 downto 4) = "11" else paout(2);
	paalt(3) <= spi1_do when paaltcfg0(7 downto 6) = "11" else paout(3);
	paalt(4) <= uart1_tx when paaltcfg0(9 downto 8) = "01" else paout(4);
	paalt(5) <= spi0_clk_o when paaltcfg0(11 downto 10) =  "10" else paout(5);
	paalt(6) <= spi0_do when paaltcfg0(13 downto 12) = "10" else paout(6);
	paalt(7) <= spi0_do when paaltcfg0(15 downto 14) = "10" else paout(7);
	
	paalt(8) <= int_timer1_ctc when paaltcfg0(17 downto 16) = "01" else int_timer1_ocr when paaltcfg0(17 downto 16) = "10" else paout(8);
	paalt(9) <= int_timer2_ctc when paaltcfg0(19 downto 18) = "01" else int_timer2_ocr when paaltcfg0(19 downto 18) = "10" else spi1_clk_o when paaltcfg0(19 downto 18) = "11" else paout(9);
	paalt(10) <= uart0_tx when paaltcfg0(21 downto 20) = "01" else spi1_do when paaltcfg0(21 downto 20) = "11" else paout(10);
	paalt(11) <= spi1_do when paaltcfg0(23 downto 22) = "11" else paout(11);
	paalt(12) <= uart1_tx when paaltcfg0(25 downto 24) = "01" else paout(12);
	paalt(13) <= spi0_clk_o when paaltcfg0(27 downto 26) = "10" else paout(13);
	paalt(14) <= spi0_do when paaltcfg0(29 downto 28) = "10" else paout(14);
	paalt(15) <= spi0_do when paaltcfg0(31 downto 30) = "10" else paout(15);
	
	-- PORT A alternate config MUXes for inputs
	uart0_rx <= pain(3) when paaltcfg0(7 downto 6) = "01" else pain(11) when paaltcfg0(23 downto 22) = "01" else '1';
	uart1_rx <= pain(5) when paaltcfg0(11 downto 10) = "01" else pain(13) when paaltcfg0(27 downto 26) = "01" else '1';
	
	spi0_ssn <= pain(4) when paaltcfg0(9 downto 8) = "10" else pain(12) when paaltcfg0(25 downto 24) = "10" else '1';
	spi0_clk_i <= pain(5) when paaltcfg0(11 downto 10) = "10" else pain(13) when paaltcfg0(27 downto 26) = "10" else '0';
	spi0_di <= pain(6) when paaltcfg0(13 downto 12) = "10" else pain(14) when paaltcfg0(29 downto 28) = "10" else
		pain(7) when paaltcfg0(15 downto 14) = "10" else pain(15) when paaltcfg0(31 downto 30) = "10" else '0';
	
	spi1_ssn <= pain(0) when paaltcfg0(1 downto 0) = "11" else pain(8) when paaltcfg0(17 downto 16) = "11" else '1';
	spi1_clk_i <= pain(1) when paaltcfg0(3 downto 2) = "11" else pain(9) when paaltcfg0(19 downto 18) = "11" else '0';
	spi1_di <= pain(2) when paaltcfg0(5 downto 4) = "11" else pain(10) when paaltcfg0(21 downto 20) = "11" else
		pain(3) when paaltcfg0(7 downto 6) = "11" else pain(11) when paaltcfg0(23 downto 22) = "11" else '0';
	
	-- address decoder, read from peripheral registers
	process(clk_i, rst_i, segment, class, device, funct)
	begin
		if rst_i = '1' then
			data_o <= (others => '0');
			uart0_enable_r <= '0';
			uart1_enable_r <= '0';
		elsif clk_i'event and clk_i = '1' then
			if sel_i = '1' then
				case segment is
				when "0001" =>
					case class is
					when "0000" =>							-- Segment 0
						case device is
						when "000001" =>					-- S0CAUSE		(RO)
							data_o <= x"000000" & "000" & s0cause;
						when "010000" =>					-- PAALTCFG0		(RW)
							data_o <= paaltcfg0;
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
								data_o <= x"0000" & paddr;
							when "0001" =>					-- PAOUT		(RW)
								data_o <= x"0000" & paout;
							when "0010" =>					-- PAIN			(RO)
								data_o <= x"0000" & pain;
							when "0011" =>					-- PAIN_INV		(RW)
								data_o <= x"0000" & pain_inv;
							when "0100" =>					-- PAIN_MASK		(RW)
								data_o <= x"0000" & pain_mask;
							when others =>
								data_o <= (others => '0');
							end case;
						when "010001" =>					-- PORTB
							case funct is
							when "0000" =>					-- PBDDR		(RW)
								data_o <= x"0000" & pbddr;
							when "0001" =>					-- PBOUT		(RW)
								data_o <= x"0000" & pbout;
							when "0010" =>					-- PBIN			(RO)
								data_o <= x"0000" & pbin;
							when "0011" =>					-- PBIN_INV		(RW)
								data_o <= x"0000" & pbin_inv;
							when "0100" =>					-- PBIN_MASK		(RW)
								data_o <= x"0000" & pbin_mask;
							when others =>
								data_o <= (others => '0');
							end case;
						when others =>
							data_o <= (others => '0');
						end case;
					when "0010" =>							-- timers
						case device is
						when "000001" =>					-- TIMERCAUSE		(RO)
							data_o <= x"000000" & timercause;
						when "000010" =>					-- TIMERCAUSE_INV	(RW)
							data_o <= x"000000" & timercause_inv;
						when "000011" =>					-- TIMERMASK		(RW)
							data_o <= x"000000" & timermask;
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
						when "010010" =>					-- TIMER2
							case funct is
							when "0000" =>					-- TIMER2		(RW)
								data_o <= x"0000" & timer2;
							when "0001" =>					-- TIMER2_PRE		(RW)
								data_o <= x"0000000" & '0' & timer2_pre;
							when "0010" =>					-- TIMER2_CTC		(RW)
								data_o <= x"0000" & timer2_ctc;
							when "0011" =>					-- TIMER2_OCR		(RW)
								data_o <= x"0000" & timer2_ocr;
							when others =>
								data_o <= (others => '0');
							end case;
						when others =>
							data_o <= (others => '0');
						end case;
					when "0011" =>							-- UARTs
						case device is
						when "000001" =>					-- TIMERCAUSE		(RO)
							data_o <= x"0000000" & uartcause;
						when "000010" =>					-- UARTCAUSE_INV	(RW)
							data_o <= x"0000000" & uartcause_inv;
						when "000011" =>					-- UARTMASK		(RW)
							data_o <= x"0000000" & uartmask;
						when "010000" =>					-- UART0
							case funct is
							when "0000" =>					-- UART0		(RW)
								data_o <= x"000000" & uart0_data_read;
								uart0_enable_r <= '1';
							when "0001" =>					-- UART0DIV		(RW)
								data_o <= x"0000" & uart0_divisor;
							when others =>
							end case;
						when "010001" =>					-- UART1
							case funct is
							when "0000" =>					-- UART1		(RW)
								data_o <= x"000000" & uart1_data_read;
								uart1_enable_r <= '1';
							when "0001" =>					-- UART1DIV		(RW)
								data_o <= x"0000" & uart1_divisor;
							when others =>
							end case;
						when others =>
						end case;
					when "0100" =>							-- SPIs
						case device is
						when "000001" =>					-- SPICAUSE		(RO)
							data_o <= x"0000000" & spicause;
						when "000010" =>					-- SPICAUSE_INV		(RW)
							data_o <= x"0000000" & spicause_inv;
						when "000011" =>					-- SPIMASK		(RW)
							data_o <= x"0000000" & spimask;
						when "010000" =>					-- SPI0
							case funct is
							when "0000" => 					-- SPI0			(RW)
								data_o <= x"000000" & spi0_data_read;
							when "0001" =>					-- SPI0CTRL		(RW)
								data_o <= x"0000" & "0000" & spi0_clk_div & "0" & spi0_data_valid & spi0_data_xfer;
							when others =>
							end case;
						when "010001" =>					-- SPI1
							case funct is
							when "0000" => 					-- SPI1			(RW)
								data_o <= x"000000" & spi1_data_read;
							when "0001" =>					-- SPI1CTRL		(RW)
								data_o <= x"0000" & "0000" & spi1_clk_div & "0" & spi1_data_valid & spi1_data_xfer;
							when others =>
							end case;
						when others =>
						end case;
					when others =>
						data_o <= (others => '0');
					end case;
				when others =>
					data_o <= (others => '0');
				end case;
			else
				uart0_enable_r <= '0';
				uart1_enable_r <= '0';
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
			pbout <= (others => '0');
			pbin_inv <= (others => '0');
			pbin_mask <= (others => '0');
			pbddr <= (others => '0');
			timercause_inv <= (others => '0');
			timermask <= (others => '0');
			timer0 <= (others => '0');
			timer1 <= (others => '0');
			timer1_set <= '0';
			timer1_pre <= (others => '0');
			timer1_ctc <= (others => '1');
			timer1_ocr <= (others => '0');
			timer2 <= (others => '0');
			timer2_set <= '0';
			timer2_pre <= (others => '0');
			timer2_ctc <= (others => '1');
			timer2_ocr <= (others => '0');
			int_timer1_ctc <= '0';
			int_timer2_ctc <= '0';
			uartcause_inv <= (others => '0');
			uartmask <= (others => '0');
			uart0_enable_w <= '0';
			uart0_data_write <= (others => '0');
			uart0_divisor <= (others => '0');
			uart1_enable_w <= '0';
			uart1_data_write <= (others => '0');
			uart1_divisor <= (others => '0');
			spicause_inv <= (others => '0');
			spimask <= (others => '0');
			spi0_data_write <= (others => '0');
			spi0_data_xfer <= '0';
			spi0_clk_div <= (others => '0');
			spi1_data_write <= (others => '0');
			spi1_data_xfer <= '0';
			spi1_clk_div <= (others => '0');
		elsif clk_i'event and clk_i = '1' then
			if sel_i = '1' and wr_i = '1' then
				case segment is
				when "0001" =>
					case class is
					when "0000" =>							-- Segment 0
						case device is
						when "010000" =>					-- PAALTCFG0		(RW)
							paaltcfg0 <= data_i(31 downto 0);
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
								paddr <= data_i(15 downto 0);
							when "0001" =>					-- PAOUT		(RW)
								paout <= data_i(15 downto 0);
							when "0011" =>					-- PAIN_INV		(RW)
								pain_inv <= data_i(15 downto 0);
							when "0100" =>					-- PAIN_MASK		(RW)
								pain_mask <= data_i(15 downto 0);
							when others =>
							end case;
						when "010001" =>					-- PORTB
							case funct is
							when "0000" =>					-- PBDDR		(RW)
								pbddr <= data_i(15 downto 0);
							when "0001" =>					-- PBOUT		(RW)
								pbout <= data_i(15 downto 0);
							when "0011" =>					-- PBIN_INV		(RW)
								pbin_inv <= data_i(15 downto 0);
							when "0100" =>					-- PBIN_MASK		(RW)
								pbin_mask <= data_i(15 downto 0);
							when others =>
							end case;
						when others =>
						end case;
					when "0010" =>							-- timers
						case device is
						when "000010" =>					-- TIMERCAUSE_INV	(RW)
							timercause_inv <= data_i(7 downto 0);
						when "000011" =>					-- TIMERMASK		(RW)
							timermask <= data_i(7 downto 0);
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
						when "010010" =>					-- TIMER2
							case funct is
							when "0000" =>					-- TIMER2		(RW)
								if data_i(31) = '1' then
									timer2_set <= '1';
								end if;
								if timer2_set = '1' then
									timer2 <= data_i(15 downto 0);
									timer2_set <= '0';
								end if;
							when "0001" =>					-- TIMER2_PRE		(RW)
								timer2_pre <= data_i(2 downto 0);
							when "0010" =>					-- TIMER2_CTC		(RW)
								timer2_ctc <= data_i(15 downto 0);
							when "0011" =>					-- TIMER2_OCR		(RW)
								timer2_ocr <= data_i(15 downto 0);
							when others =>
							end case;
						when others =>
						end case;
					when "0011" =>							-- UARTs
						case device is
						when "000010" =>					-- UARTCAUSE_INV	(RW)
							uartcause_inv <= data_i(3 downto 0);
						when "000011" =>					-- UARTMASK		(RW)
							uartmask <= data_i(3 downto 0);
						when "010000" =>					-- UART0
							case funct is
							when "0000" =>					-- UART0		(RW)
								uart0_data_write <= data_i(7 downto 0);
								uart0_enable_w <= '1';
							when "0001" =>					-- UART0DIV		(RW)
								uart0_divisor <= data_i(15 downto 0);
							when others =>
							end case;
						when "010001" =>					-- UART1
							case funct is
							when "0000" =>					-- UART1		(RW)
								uart1_data_write <= data_i(7 downto 0);
								uart1_enable_w <= '1';
							when "0001" =>					-- UART1DIV		(RW)
								uart1_divisor <= data_i(15 downto 0);
							when others =>
							end case;
						when others =>
						end case;
					when "0100" =>							-- SPIs
						case device is
						when "000010" =>					-- SPICAUSE_INV		(RW)
							spicause_inv <= data_i(3 downto 0);
						when "000011" =>					-- SPIMASK		(RW)
							spimask <= data_i(3 downto 0);
						when "010000" =>					-- SPI0
							case funct is
							when "0000" =>					-- SPI0			(RW)
								spi0_data_write <= data_i(7 downto 0);
								spi0_data_xfer <= '1';
							when "0001" =>					-- SPI0CTRL		(RW)
								spi0_data_xfer <= data_i(0);
								spi0_clk_div <= data_i(11 downto 3);
							when others =>
							end case;
						when "010001" =>					-- SPI1
							case funct is
							when "0000" =>					-- SPI1			(RW)
								spi1_data_write <= data_i(7 downto 0);
								spi1_data_xfer <= '1';
							when "0001" =>					-- SPI1CTRL		(RW)
								spi1_data_xfer <= data_i(0);
								spi1_clk_div <= data_i(11 downto 3);
							when others =>
							end case;
						when others =>
						end case;
					when others =>
					end case;
				when others =>
				end case;
			else
				uart0_enable_w <= '0';
				uart1_enable_w <= '0';
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
			
			if tmr2_pulse = '1' then
				if (timer2 /= timer2_ctc) then
					if timer2_set = '0' then
						timer2 <= timer2 + 1;
					end if;
				else
					int_timer2_ctc <= not int_timer2_ctc;
					timer2 <= (others => '0');
				end if;
			end if;
		end if;
	end process;

	process(clk_i, rst_i)				-- TIMER1 and TIMER2 prescalers
	begin
		if rst_i = '1' then
			tmr1_dly <= '0';
			tmr1_dly2 <= '0';
			tmr2_dly <= '0';
			tmr2_dly2 <= '0';
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

			case timer2_pre is
			when "001" =>
				tmr2_dly <= timer0(2);	-- /4
			when "010" =>
				tmr2_dly <= timer0(4);	-- /16
			when "011" =>
				tmr2_dly <= timer0(6);	-- /64
			when "100" =>
				tmr2_dly <= timer0(8);	-- /256
			when "101" =>
				tmr2_dly <= timer0(10);	-- /1024
			when "110" =>
				tmr2_dly <= timer0(12);	-- /4096
			when "111" =>
				tmr2_dly <= timer0(14);	-- /16384
			when others =>
				tmr2_dly <= timer0(0);	-- /1
			end case;
			
			tmr1_dly2 <= tmr1_dly;
			tmr2_dly2 <= tmr2_dly;
		end if;
	end process;

	tmr1_pulse <= '1' when tmr1_dly /= tmr1_dly2 else '0';
	int_timer1_ocr <= '1' when timer1 < timer1_ocr else '0';
	tmr2_pulse <= '1' when tmr2_dly /= tmr2_dly2 else '0';
	int_timer2_ocr <= '1' when timer2 < timer2_ocr else '0';

	uart0: entity work.uart
	port map(
		clk		=> clk_i,
		reset		=> rst_i,
		divisor		=> uart0_divisor(11 downto 0),
		enable_read	=> uart0_enable_r,
		enable_write	=> uart0_enable_w,
		data_in		=> uart0_data_write,
		data_out	=> uart0_data_read,
		uart_read	=> uart0_rx,
		uart_write	=> uart0_tx,
		busy_write	=> uart0_write_busy,
		data_avail	=> uart0_data_avail
	);
	
	uart1: entity work.uart
	port map(
		clk		=> clk_i,
		reset		=> rst_i,
		divisor		=> uart1_divisor(11 downto 0),
		enable_read	=> uart1_enable_r,
		enable_write	=> uart1_enable_w,
		data_in		=> uart1_data_write,
		data_out	=> uart1_data_read,
		uart_read	=> uart1_rx,
		uart_write	=> uart1_tx,
		busy_write	=> uart1_write_busy,
		data_avail	=> uart1_data_avail
	);

	spi0: entity work.spi_master_slave
	generic map(
		BYTE_SIZE => 8
	)
	port map(	clk_i => clk_i,
			rst_i => rst_i,
			data_i => spi0_data_write,
			data_o => spi0_data_read,
			data_valid_o => spi0_data_valid,
			wren_i => spi0_data_xfer,
			clk_div_i => spi0_clk_div,
			spi_ssn_i => spi0_ssn,
			spi_clk_i => spi0_clk_i,
			spi_clk_o => spi0_clk_o,
			spi_do_o => spi0_do,
			spi_di_i => spi0_di
	);

	spi1: entity work.spi_master_slave
	generic map(
		BYTE_SIZE => 8
	)
	port map(	clk_i => clk_i,
			rst_i => rst_i,
			data_i => spi1_data_write,
			data_o => spi1_data_read,
			data_valid_o => spi1_data_valid,
			wren_i => spi1_data_xfer,
			clk_div_i => spi1_clk_div,
			spi_ssn_i => spi1_ssn,
			spi_clk_i => spi1_clk_i,
			spi_clk_o => spi1_clk_o,
			spi_do_o => spi1_do,
			spi_di_i => spi1_di
	);

end peripherals_arch;
