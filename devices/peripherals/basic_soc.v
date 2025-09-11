// file:          basic_soc.vhd
// description:   basic SoC with peripherals
// date:          08/2025
// author:        Sergio Johann Filho <sergio.filho@pucrs.br>
//
// Basic SoC configuration template for prototyping. Dual GPIO ports,
// a counter, a timer (two PWM channels) and a UART are included in this
// version.


module peripherals
(
	input wire clk_i,
	input wire rst_i,
	input wire [31:0] addr_i,
	input wire [31:0] data_i,
	output reg [31:0] data_o,
	input wire sel_i,
	input wire wr_i,
	output wire irq_o,
	input wire [15:0] gpioa_in,
	output wire [15:0] gpioa_out,
	output wire [15:0] gpioa_ddr,
	input wire [15:0] gpiob_in,
	output wire [15:0] gpiob_out,
	output wire [15:0] gpiob_ddr
);

	wire [3:0] segment;
	wire [3:0] clas;
	wire [5:0] device;
	wire [3:0] funct;
	wire [3:0] s0cause; wire [3:0] gpiocause; reg [3:0] gpiocause_inv; reg [3:0] gpiomask;
	wire [4:0] timercause; reg [4:0] timercause_inv; reg [4:0] timermask;
	reg [15:0] paddr; reg [15:0] paout; wire [15:0] pain; reg [15:0] pain_inv; reg [15:0] pain_mask;
	reg [15:0] pbddr; reg [15:0] pbout; wire [15:0] pbin; reg [15:0] pbin_inv; reg [15:0] pbin_mask;
	reg [23:0] paaltcfg0;
	wire paalt0; wire paalt1; wire paalt2; wire paalt8; wire paalt9; wire paalt10;
	wire int_gpio; wire int_timer;
	wire int_gpioa; wire int_gpiob; wire int_timer1_ocr; reg int_timer1_ctc; wire val_timer1_ocr1;
	wire tmr1_pulse; reg tmr1_dly; reg tmr1_dly2;
	reg [31:0] timer0;
	reg [15:0] timer1; reg [15:0] timer1_ctc; reg [15:0] timer1_ocr; reg [15:0] timer1_ocr1;
	reg [2:0] timer1_pre;
	wire int_uart; wire uart0_tx; wire uart0_rx; reg uart0_enable_w; reg uart0_enable_r; wire uart0_write_busy; wire uart0_data_avail;
	wire [3:0] uartcause; reg [3:0] uartcause_inv; reg [3:0] uartmask;
	wire [7:0] uart0_data_read; reg [7:0] uart0_data_write;
	reg [15:0] uart0_divisor;

	assign segment = addr_i[27:24];
	assign clas = addr_i[19:16];
	assign device = addr_i[15:10];
	assign funct = addr_i[7:4];
	assign irq_o = s0cause != 4'b0000 ? 1'b1 : 1'b0;
	assign s0cause = {int_uart,int_timer,int_gpio,1'b0};
	assign int_gpio = ((gpiocause ^ gpiocause_inv) & gpiomask) != 4'b0000 ? 1'b1 : 1'b0;
	assign gpiocause = {2'b00,int_gpiob,int_gpioa};
	assign int_gpioa = ((pain ^ pain_inv) & pain_mask) != 4'b0000 ? 1'b1 : 1'b0;
	assign int_gpiob = ((pbin ^ pbin_inv) & pbin_mask) != 4'b0000 ? 1'b1 : 1'b0;
	assign int_timer = ((timercause ^ timercause_inv) & timermask) != 4'b0000 ? 1'b1 : 1'b0;
	assign timercause = {timer1[15],int_timer1_ocr,int_timer1_ctc,timer0[18],timer0[16]};
	assign pain = gpioa_in[15:0];
	assign gpioa_out = {paout[15:11],paalt10,paalt9,paalt8,paout[7:3],paalt2,paalt1,paalt0};
	assign gpioa_ddr = paddr;
	assign pbin = gpiob_in[15:0];
	assign gpiob_out = pbout;
	assign gpiob_ddr = pbddr;
	assign int_uart = ((uartcause ^ uartcause_inv) & uartmask) != 4'b0000 ? 1'b1 : 1'b0;
	assign uartcause = {2'b00,uart0_write_busy,uart0_data_avail};
	assign paalt0 = paaltcfg0[1:0] == 2'b01 ? int_timer1_ocr : paout[0];
	assign paalt1 = paaltcfg0[3:2] == 2'b01 ? val_timer1_ocr1 : paout[1];
	assign paalt2 = paaltcfg0[5:4] == 2'b01 ? uart0_tx : paout[2];
	assign paalt8 = paaltcfg0[17:16] == 2'b01 ? int_timer1_ocr : paout[8];
	assign paalt9 = paaltcfg0[19:18] == 2'b01 ? val_timer1_ocr1 : paout[9];
	assign paalt10 = paaltcfg0[21:20] == 2'b01 ? uart0_tx : paout[10];
	assign uart0_rx = paaltcfg0[7:6] == 2'b01 ? gpioa_in[3] : paaltcfg0[23:22] == 2'b01 ? gpioa_in[11] : 1'b1;

	// address decoder, read from peripheral registers
	always @(posedge clk_i, posedge rst_i) begin
		if (rst_i == 1'b1) begin
			data_o <= {32{1'b0}};
			uart0_enable_r <= 1'b0;
		end else begin
			if (sel_i == 1'b1) begin
				case(segment)
				4'b0001 : begin
					case(clas)
					4'b0000 : begin
					// Segment 0
						case(device)
						6'b000001 : begin
							// S0CAUSE		(RO)
							data_o <= {28'h0000000,s0cause};
						end
						6'b010000 : begin
							// PAALTCFG0		(RW)
							data_o <= {8'h00,paaltcfg0};
						end
						default : begin
							data_o <= {32{1'b0}};
						end
						endcase
					end
					4'b0001 : begin
						// GPIO
						case(device)
						6'b000001 : begin
							// GPIOCAUSE		(RO)
							data_o <= {28'h0000000,gpiocause};
						end
						6'b000010 : begin
							// GPIOCAUSE_INV	(RW)
							data_o <= {28'h0000000,gpiocause_inv};
						end
						6'b000011 : begin
							// GPIOMASK		(RW)
							data_o <= {28'h0000000,gpiomask};
						end
						6'b010000 : begin
							// PORTA
							case(funct)
							4'b0000 : begin
								// PADDR		(RW)
								data_o <= {16'h0000,paddr};
							end
							4'b0001 : begin
								// PAOUT		(RW)
								data_o <= {16'h0000,paout};
							end
							4'b0010 : begin
								// PAIN			(RO)
								data_o <= {16'h0000,pain};
							end
							4'b0011 : begin
								// PAIN_INV		(RW)
								data_o <= {16'h0000,pain_inv};
							end
							4'b0100 : begin
								// PAIN_MASK		(RW)
								data_o <= {16'h0000,pain_mask};
							end
							default : begin
								data_o <= {32{1'b0}};
							end
							endcase
						end
						6'b010001 : begin
							// PORTB
							case(funct)
							4'b0000 : begin
								// PBDDR		(RW)
								data_o <= {16'h0000,pbddr};
							end
							4'b0001 : begin
								// PBOUT		(RW)
								data_o <= {16'h0000,pbout};
							end
							4'b0010 : begin
								// PBIN			(RO)
								data_o <= {16'h0000,pbin};
							end
							4'b0011 : begin
								// PBIN_INV		(RW)
								data_o <= {16'h0000,pbin_inv};
							end
							4'b0100 : begin
								// PBIN_MASK		(RW)
								data_o <= {16'h0000,pbin_mask};
							end
							default : begin
								data_o <= {32{1'b0}};
							end
							endcase
						end
						default : begin
							data_o <= {32{1'b0}};
						end
						endcase
					end
					4'b0010 : begin
						// timers
						case(device)
						6'b000001 : begin
							// TIMERCAUSE		(RO)
							data_o <= {24'h000000,3'b000,timercause};
						end
						6'b000010 : begin
							// TIMERCAUSE_INV	(RW)
							data_o <= {24'h000000,3'b000,timercause_inv};
						end
						6'b000011 : begin
							// TIMERMASK		(RW)
							data_o <= {24'h000000,3'b000,timermask};
						end
						6'b010000 : begin
							// TIMER0		(RO)
							data_o <= timer0;
						end
						6'b010001 : begin
							// TIMER1
							case(funct)
							4'b0000 : begin
								// TIMER1		(RW)
								data_o <= {16'h0000,timer1};
							end
							4'b0001 : begin
								// TIMER1_PRE		(RW)
								data_o <= {28'h0000000,1'b0,timer1_pre};
							end
							4'b0010 : begin
								// TIMER1_CTC		(RW)
								data_o <= {16'h0000,timer1_ctc};	
							end
							4'b0011 : begin
								// TIMER1_OCR ch0	(RW)
								data_o <= {16'h0000,timer1_ocr};
							end
							4'b0100 : begin
								// TIMER1_OCR ch1	(RW)
								data_o <= {16'h0000,timer1_ocr1};
							end
							default : begin
								data_o <= {32{1'b0}};
							end
							endcase
						end
						default : begin
							data_o <= {32{1'b0}};
						end
						endcase
					end
					4'b0011 : begin
						// UARTs
						case(device)
						6'b000001 : begin
							// TIMERCAUSE		(RO)
							data_o <= {28'h0000000,uartcause};
						end
						6'b000010 : begin
							// UARTCAUSE_INV	(RW)
							data_o <= {28'h0000000,uartcause_inv};
						end
						6'b000011 : begin
							// UARTMASK		(RW)
							data_o <= {28'h0000000,uartmask};
						end
						6'b010000 : begin
							// UART0
							case(funct)
							4'b0000 : begin
								// UART0		(RW)
								data_o <= {24'h000000,uart0_data_read};
								uart0_enable_r <= 1'b1;
							end
							4'b0001 : begin
								// UART0DIV		(RW)
								data_o <= {16'h0000,uart0_divisor};
							end
							default : begin
							end
							endcase
						end
						default : begin
						end
						endcase
					end
					default : begin
						data_o <= {32{1'b0}};
					end
					endcase
				end
				default : begin
					data_o <= {32{1'b0}};
				end
				endcase
			end
			else begin
				uart0_enable_r <= 1'b0;
			end
		end
	end

	// peripheral register logic, write to peripheral registers
	always @(posedge clk_i, posedge rst_i) begin
		if (rst_i == 1'b1) begin
			paaltcfg0 <= {24{1'b0}};
			gpiocause_inv <= {4{1'b0}};
			gpiomask <= {4{1'b0}};
			paout <= {16{1'b0}};
			pain_inv <= {16{1'b0}};
			pain_mask <= {16{1'b0}};
			paddr <= {16{1'b0}};
			pbout <= {16{1'b0}};
			pbin_inv <= {16{1'b0}};
			pbin_mask <= {16{1'b0}};
			pbddr <= {16{1'b0}};
			timercause_inv <= {5{1'b0}};
			timermask <= {5{1'b0}};
			timer0 <= {32{1'b0}};
			timer1 <= {16{1'b0}};
			timer1_pre <= {3{1'b0}};
			timer1_ctc <= {16{1'b1}};
			timer1_ocr <= {16{1'b0}};
			timer1_ocr1 <= {16{1'b0}};
			int_timer1_ctc <= 1'b0;
			uartcause_inv <= {4{1'b0}};
			uartmask <= {4{1'b0}};
			uart0_enable_w <= 1'b0;
			uart0_data_write <= {8{1'b0}};
			uart0_divisor <= {16{1'b0}};
		end else begin
			if (sel_i == 1'b1 && wr_i == 1'b1) begin
				case(segment)
				4'b0001 : begin
					case(clas)
					4'b0000 : begin
						// Segment 0
						case(device)
						6'b010000 : begin
							// PAALTCFG0		(RW)
							paaltcfg0 <= data_i[23:0];
						end
						default : begin
						end
						endcase
					end
					4'b0001 : begin
						// GPIO
						case(device)
						6'b000010 : begin
							// GPIOCAUSE_INV	(RW)
							gpiocause_inv <= data_i[3:0];
						end
						6'b000011 : begin
							// GPIOMASK		(RW)
							gpiomask <= data_i[3:0];
						end
						6'b010000 : begin
							// PORTA
							case(funct)
							4'b0000 : begin
								// PADDR		(RW)
								paddr <= data_i[15:0];
							end
							4'b0001 : begin
								// PAOUT		(RW)
								paout <= data_i[15:0];
							end
							4'b0011 : begin
								// PAIN_INV		(RW)
								pain_inv <= data_i[15:0];
							end
							4'b0100 : begin
								// PAIN_MASK		(RW)
								pain_mask <= data_i[15:0];
							end
							default : begin
							end
							endcase
						end
						6'b010001 : begin
							// PORTB
							case(funct)
							4'b0000 : begin
								// PBDDR		(RW)
								pbddr <= data_i[15:0];
							end
							4'b0001 : begin
								// PBOUT		(RW)
								pbout <= data_i[15:0];
							end
							4'b0011 : begin
								// PBIN_INV		(RW)
								pbin_inv <= data_i[15:0];
							end
							4'b0100 : begin
								// PBIN_MASK		(RW)
								pbin_mask <= data_i[15:0];
							end
							default : begin
							end
							endcase
						end
						default : begin
						end
						endcase
					end
					4'b0010 : begin
						// timers
						case(device)
						6'b000010 : begin
							// TIMERCAUSE_INV	(RW)
							timercause_inv <= data_i[4:0];
						end
						6'b000011 : begin
							// TIMERMASK		(RW)
							timermask <= data_i[4:0];
						end
						6'b010001 : begin
							// TIMER1
							case(funct)
							4'b0000 : begin
								// TIMER1		(RW)
								timer1 <= data_i[15:0];
							end
							4'b0001 : begin
								// TIMER1_PRE		(RW)
								timer1_pre <= data_i[2:0];
							end
							4'b0010 : begin
								// TIMER1_CTC		(RW)
								timer1_ctc <= data_i[15:0];
							end
							4'b0011 : begin
								// TIMER1_OCR ch0	(RW)
								timer1_ocr <= data_i[15:0];
							end
							4'b0100 : begin
								// TIMER1_OCR ch1	(RW)
								timer1_ocr1 <= data_i[15:0];
							end
							default : begin
							end
							endcase
						end
						default : begin
						end
						endcase
					end
					4'b0011 : begin
						// UARTs
						case(device)
						6'b000010 : begin
							// UARTCAUSE_INV	(RW)
							uartcause_inv <= data_i[3:0];
						end
						6'b000011 : begin
							// UARTMASK		(RW)
							uartmask <= data_i[3:0];
						end
						6'b010000 : begin
							// UART0
							case(funct)
							4'b0000 : begin
								// UART0		(RW)
								uart0_data_write <= data_i[7:0];
								uart0_enable_w <= 1'b1;
							end
							4'b0001 : begin
								// UART0DIV		(RW)
								uart0_divisor <= data_i[15:0];
							end
							default : begin
							end
							endcase
						end
						default : begin
						end
						endcase
					end
					default : begin
					end
					endcase
				end
				default : begin
				end
				endcase
			end
			else begin
				uart0_enable_w <= 1'b0;
			end
			
			timer0 <= timer0 + 1;
			
			if (tmr1_pulse == 1'b1) begin
				if ((timer1 != timer1_ctc)) begin
					timer1 <= timer1 + 1;
				end
				else begin
					int_timer1_ctc <=  ~int_timer1_ctc;
					timer1 <= {16{1'b0}};
				end
			end
		end
	end

	always @(posedge clk_i, posedge rst_i) begin
		// TIMER1 prescaler
		if (rst_i == 1'b1) begin
			tmr1_dly <= 1'b0;
			tmr1_dly2 <= 1'b0;
		end else begin
			case(timer1_pre)
				3'b001 : begin
				tmr1_dly <= timer0[2];
				// /4
			end
			3'b010 : begin
				tmr1_dly <= timer0[4];
				// /16
			end
			3'b011 : begin
				tmr1_dly <= timer0[6];
				// /64
			end
			3'b100 : begin
				tmr1_dly <= timer0[8];
				// /256
			end
			3'b101 : begin
				tmr1_dly <= timer0[10];
				// /1024
			end
			3'b110 : begin
				tmr1_dly <= timer0[12];
				// /4096
			end
			3'b111 : begin
				tmr1_dly <= timer0[14];
				// /16384
			end
			default : begin
				tmr1_dly <= timer0[0];
				// /1
			end
			endcase
		tmr1_dly2 <= tmr1_dly;
		end
	end

	assign tmr1_pulse = tmr1_dly != tmr1_dly2 ? 1'b1 : 1'b0;
	assign int_timer1_ocr = timer1 < timer1_ocr ? 1'b1 : 1'b0;
	assign val_timer1_ocr1 = timer1 < timer1_ocr1 ? 1'b1 : 1'b0;
  
	uart uart0
	(
		.clk(clk_i),
		.reset(rst_i),
		.divisor(uart0_divisor[11:0]),
		.enable_read(uart0_enable_r),
		.enable_write(uart0_enable_w),
		.data_in(uart0_data_write),
		.data_out(uart0_data_read),
		.uart_read(uart0_rx),
		.uart_write(uart0_tx),
		.busy_write(uart0_write_busy),
		.data_avail(uart0_data_avail)
	);

endmodule
