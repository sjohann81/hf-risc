module top_tb
(
	input wire clk_i
);

	parameter [31:0] address_width=15;

	reg clock = 1'b0; reg reset = 1'b0;
	wire stall = 1'b0;
	wire uart_read; wire uart_write;
	wire [3:0] boot_enable_n; wire [3:0] ram_enable_n; reg ram_dly;
	wire [31:0] address; wire [31:0] data_read; wire [31:0] data_write; wire [31:0] data_read_boot; wire [31:0] data_read_ram;
	wire [7:0] ext_irq;
	wire [3:0] data_we; wire [3:0] data_w_n_ram; wire [3:0] no_we;
	wire periph; reg periph_dly; wire periph_wr; wire periph_irq;
	wire [31:0] data_read_periph; wire [31:0] data_read_periph_s; wire [31:0] data_write_periph;
	wire [15:0] gpioa_in; wire [15:0] gpioa_out; wire [15:0] gpioa_ddr;
	wire [15:0] gpiob_in; wire [15:0] gpiob_out; wire [15:0] gpiob_ddr;

	// reset generation
	initial begin
		reset = 1'b1;
		#500
		reset = 1'b0;
	end

	// clock generation
	initial begin
		clock = 1'b0;
		forever #20 clock = ~clock;
	end

	assign no_we = {4'b1111};
	assign gpioa_in = {16'h0000};
	assign gpiob_in = {16'h0000};
	assign stall = 1'b0;
	assign ext_irq = {7'b0000000,periph_irq};
	assign boot_enable_n = (address[31:28] == 4'b0000) || reset == 1'b1 ? 4'b0000 : 4'b1111;
	assign ram_enable_n = (address[31:28] == 4'b0100) || reset == 1'b1 ? 4'b0000 : 4'b1111;
	assign data_read = periph == 1'b1 || periph_dly == 1'b1 ? data_read_periph : address[31:28] == 4'b0000 && ram_dly == 1'b0 ? data_read_boot : data_read_ram;
	assign data_w_n_ram = ~data_we;

	always @(posedge clock, posedge reset) begin
		if (reset == 1'b1) begin
			ram_dly <= 1'b0;
			periph_dly <= 1'b0;
		end else begin
			ram_dly <=  ~ram_enable_n;
			periph_dly <= periph;
		end
	end

	// HF-RISCV core
	processor processor
	(
		.clk_i(clock),
		.rst_i(reset),
		.stall_i(stall),
		.addr_o(address),
		.data_i(data_read),
		.data_o(data_write),
		.data_w_o(data_we),
		.data_mode_o(/* open */),
		.extio_in(ext_irq),
		.extio_out(/* open */)
	);

	assign data_read_periph = {data_read_periph_s[7:0],data_read_periph_s[15:8],data_read_periph_s[23:16],data_read_periph_s[31:24]};
	assign data_write_periph = {data_write[7:0],data_write[15:8],data_write[23:16],data_write[31:24]};
	assign periph_wr = data_we != 4'b0000 ? 1'b1 : 1'b0;
	assign periph = address[31:28] == 4'he ? 1'b1 : 1'b0;

	peripherals peripherals
	(
		.clk_i(clock),
		.rst_i(reset),
		.addr_i(address),
		.data_i(data_write_periph),
		.data_o(data_read_periph_s),
		.sel_i(periph),
		.wr_i(periph_wr),
		.irq_o(periph_irq),
		.gpioa_in(gpioa_in),
		.gpioa_out(gpioa_out),
		.gpioa_ddr(gpioa_ddr),
		.gpiob_in(gpiob_in),
		.gpiob_out(gpiob_out),
		.gpiob_ddr(gpiob_ddr)
	);

	// instruction and data memory (boot RAM)
	bram
	#() rom (
		.clk(clock),
		.addr(address[11:2]),
		.cs_n(boot_enable_n),
		.we_n(no_we),
		.data_i(),
		.data_o(data_read_boot)
	);
	
	// instruction and data memory
	ram
	#() ram (
		.clk(clock),
		.addr(address[14:2]),
		.cs_n(ram_enable_n),
		.we_n(data_w_n_ram),
		.data_i(data_write),
		.data_o(data_read_ram)
	);
	
	initial begin
		$dumpfile("top_tb.vcd");
		$dumpvars(0, top_tb);
		
		//$monitor("time=%1d, loop=%1d", $time, i);
		#510
		
		while (address != 32'he0000000) begin
			@(posedge clock);
			//$strobe("---> x=%h", address);
		end
		
		//#100000
		//$strobe("---> x=%h", address);
		$display("end of simulation");
		$finish(0);
	end
endmodule : top_tb
