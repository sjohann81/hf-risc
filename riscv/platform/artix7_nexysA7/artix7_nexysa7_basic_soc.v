module hfrisc_soc
(
	input wire clk_i,
	input wire rst_i,
	inout wire [15:0] gpioa,
	inout wire [15:0] gpiob,
	output wire uart_tx,
	input wire uart_rx
);

	parameter [31:0] address_width=15;

	reg clock; wire boot_enable; wire [3:0] ram_enable_n; wire stall; reg ram_dly; reg rff1; reg reset;
	wire [31:0] address; wire [31:0] data_read; wire [31:0] data_write; wire [31:0] data_read_boot; wire [31:0] data_read_ram;
	wire [7:0] ext_irq;
	wire [3:0] data_we; wire [3:0] data_w_n_ram;
	wire periph; reg periph_dly; wire periph_wr; wire periph_irq;
	wire [31:0] data_read_periph; wire [31:0] data_read_periph_s; wire [31:0] data_write_periph;
	wire [15:0] gpioa_in; wire [15:0] gpioa_out; wire [15:0] gpioa_ddr;
	wire [15:0] gpiob_in; wire [15:0] gpiob_out; wire [15:0] gpiob_ddr;

	// clock divider (50MHz clock from 100MHz main clock for Nexys A7 kit)
	always @(posedge clk_i, posedge rst_i) begin
		if (rst_i == 1'b1) begin
			clock <= 1'b0;
		end else begin
			clock <=  ~clock;
		end
	end

	// reset synchronizer
	always @(posedge clock, posedge rst_i) begin
		if ((rst_i == 1'b1)) begin
			rff1 <= 1'b1;
			reset <= 1'b1;
		end else begin
			rff1 <= 1'b0;
			reset <= rff1;
		end
	end

	always @(posedge clock, posedge reset) begin
		if (reset == 1'b1) begin
			ram_dly <= 1'b0;
			periph_dly <= 1'b0;
		end else begin
			ram_dly <=  ~ram_enable_n;
			periph_dly <= periph;
		end
	end

	// GPIO A
	//
	// JA (inputs)
	assign gpioa[0] = gpioa_ddr[0] == 1'b1 ? gpioa_out[0] : 1'bZ;
	assign gpioa[1] = gpioa_ddr[1] == 1'b1 ? gpioa_out[1] : 1'bZ;
	assign gpioa[2] = gpioa_ddr[2] == 1'b1 ? gpioa_out[2] : 1'bZ;
	assign gpioa[3] = gpioa_ddr[3] == 1'b1 ? gpioa_out[3] : 1'bZ;
	assign gpioa[4] = gpioa_ddr[4] == 1'b1 ? gpioa_out[4] : 1'bZ;
	assign gpioa[5] = gpioa_ddr[5] == 1'b1 ? gpioa_out[5] : 1'bZ;
	assign gpioa[6] = gpioa_ddr[6] == 1'b1 ? gpioa_out[6] : 1'bZ;
	assign gpioa[7] = gpioa_ddr[7] == 1'b1 ? gpioa_out[7] : 1'bZ;
	// JB (inputs)
	assign gpioa[8] = gpioa_ddr[8] == 1'b1 ? gpioa_out[8] : 1'bZ;
	assign gpioa[9] = gpioa_ddr[9] == 1'b1 ? gpioa_out[9] : 1'bZ;
	assign gpioa[10] = gpioa_ddr[10] == 1'b1 ? gpioa_out[10] : 1'bZ;
	assign gpioa[11] = gpioa_ddr[11] == 1'b1 ? gpioa_out[11] : 1'bZ;
	assign gpioa[12] = gpioa_ddr[12] == 1'b1 ? gpioa_out[12] : 1'bZ;
	assign gpioa[13] = gpioa_ddr[13] == 1'b1 ? gpioa_out[13] : 1'bZ;
	assign gpioa[14] = gpioa_ddr[14] == 1'b1 ? gpioa_out[14] : 1'bZ;
	assign gpioa[15] = gpioa_ddr[15] == 1'b1 ? gpioa_out[15] : 1'bZ;
  
	// JA (outputs)
	assign gpioa_in[0] = gpioa_ddr[0] == 1'b1 ? 1'b0 : gpioa[0];
	assign gpioa_in[1] = gpioa_ddr[1] == 1'b1 ? 1'b0 : gpioa[1];
	assign gpioa_in[2] = gpioa_ddr[2] == 1'b1 ? 1'b0 : gpioa[2];
	//	gpioa_in(3) <= '0' when gpioa_ddr(3) = '1' else gpioa(3);
	assign gpioa_in[4] = gpioa_ddr[4] == 1'b1 ? 1'b0 : gpioa[4];
	assign gpioa_in[5] = gpioa_ddr[5] == 1'b1 ? 1'b0 : gpioa[5];
	assign gpioa_in[6] = gpioa_ddr[6] == 1'b1 ? 1'b0 : gpioa[6];
	assign gpioa_in[7] = gpioa_ddr[7] == 1'b1 ? 1'b0 : gpioa[7];
	// JB (outputs)
	assign gpioa_in[8] = gpioa_ddr[8] == 1'b1 ? 1'b0 : gpioa[8];
	assign gpioa_in[9] = gpioa_ddr[9] == 1'b1 ? 1'b0 : gpioa[9];
	assign gpioa_in[10] = gpioa_ddr[10] == 1'b1 ? 1'b0 : gpioa[10];
	assign gpioa_in[11] = gpioa_ddr[11] == 1'b1 ? 1'b0 : gpioa[11];
	assign gpioa_in[12] = gpioa_ddr[12] == 1'b1 ? 1'b0 : gpioa[12];
	assign gpioa_in[13] = gpioa_ddr[13] == 1'b1 ? 1'b0 : gpioa[13];
	assign gpioa_in[14] = gpioa_ddr[14] == 1'b1 ? 1'b0 : gpioa[14];
	assign gpioa_in[15] = gpioa_ddr[15] == 1'b1 ? 1'b0 : gpioa[15];

	// UART
	assign uart_tx = gpioa_out[2];
	assign gpioa_in[3] = uart_rx;
	// GPIO B
	//
	// JC (inputs)
	assign gpiob[0] = gpiob_ddr[0] == 1'b1 ? gpiob_out[0] : 1'bZ;
	assign gpiob[1] = gpiob_ddr[1] == 1'b1 ? gpiob_out[1] : 1'bZ;
	assign gpiob[2] = gpiob_ddr[2] == 1'b1 ? gpiob_out[2] : 1'bZ;
	assign gpiob[3] = gpiob_ddr[3] == 1'b1 ? gpiob_out[3] : 1'bZ;
	assign gpiob[4] = gpiob_ddr[4] == 1'b1 ? gpiob_out[4] : 1'bZ;
	assign gpiob[5] = gpiob_ddr[5] == 1'b1 ? gpiob_out[5] : 1'bZ;
	assign gpiob[6] = gpiob_ddr[6] == 1'b1 ? gpiob_out[6] : 1'bZ;
	assign gpiob[7] = gpiob_ddr[7] == 1'b1 ? gpiob_out[7] : 1'bZ;
	// JD (inputs)
	assign gpiob[8] = gpiob_ddr[8] == 1'b1 ? gpiob_out[8] : 1'bZ;
	assign gpiob[9] = gpiob_ddr[9] == 1'b1 ? gpiob_out[9] : 1'bZ;
	assign gpiob[10] = gpiob_ddr[10] == 1'b1 ? gpiob_out[10] : 1'bZ;
	assign gpiob[11] = gpiob_ddr[11] == 1'b1 ? gpiob_out[11] : 1'bZ;
	assign gpiob[12] = gpiob_ddr[12] == 1'b1 ? gpiob_out[12] : 1'bZ;
	assign gpiob[13] = gpiob_ddr[13] == 1'b1 ? gpiob_out[13] : 1'bZ;
	assign gpiob[14] = gpiob_ddr[14] == 1'b1 ? gpiob_out[14] : 1'bZ;
	assign gpiob[15] = gpiob_ddr[15] == 1'b1 ? gpiob_out[15] : 1'bZ;

	// JC (outputs)
	assign gpiob_in[0] = gpiob_ddr[0] == 1'b1 ? 1'b0 : gpiob[0];
	assign gpiob_in[1] = gpiob_ddr[1] == 1'b1 ? 1'b0 : gpiob[1];
	assign gpiob_in[2] = gpiob_ddr[2] == 1'b1 ? 1'b0 : gpiob[2];
	assign gpiob_in[3] = gpiob_ddr[3] == 1'b1 ? 1'b0 : gpiob[3];
	assign gpiob_in[4] = gpiob_ddr[4] == 1'b1 ? 1'b0 : gpiob[4];
	assign gpiob_in[5] = gpiob_ddr[5] == 1'b1 ? 1'b0 : gpiob[5];
	assign gpiob_in[6] = gpiob_ddr[6] == 1'b1 ? 1'b0 : gpiob[6];
	assign gpiob_in[7] = gpiob_ddr[7] == 1'b1 ? 1'b0 : gpiob[7];
	// JD (outputs)
	assign gpiob_in[8] = gpiob_ddr[8] == 1'b1 ? 1'b0 : gpiob[8];
	assign gpiob_in[9] = gpiob_ddr[9] == 1'b1 ? 1'b0 : gpiob[9];
	assign gpiob_in[10] = gpiob_ddr[10] == 1'b1 ? 1'b0 : gpiob[10];
	assign gpiob_in[11] = gpiob_ddr[11] == 1'b1 ? 1'b0 : gpiob[11];
	assign gpiob_in[12] = gpiob_ddr[12] == 1'b1 ? 1'b0 : gpiob[12];
	assign gpiob_in[13] = gpiob_ddr[13] == 1'b1 ? 1'b0 : gpiob[13];
	assign gpiob_in[14] = gpiob_ddr[14] == 1'b1 ? 1'b0 : gpiob[14];
	assign gpiob_in[15] = gpiob_ddr[15] == 1'b1 ? 1'b0 : gpiob[15];

	assign stall = 1'b0;
	assign boot_enable = address[31:28] == 4'b0000 ? 1'b1 : 1'b0;
	assign ram_enable_n = address[31:28] == 4'b0100 ? 4'b0000 : 1'b1111;
	assign data_read = periph == 1'b1 || periph_dly == 1'b1 ? data_read_periph : address[31:28] == 4'b0000 && ram_dly == 1'b0 ? data_read_boot : data_read_ram;
	assign data_w_n_ram =  ~data_we;
	assign ext_irq = {7'b0000000,periph_irq};

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
	ram #(
	)
	boot_ram
	(
		.clk(clock),
		.enable(boot_enable),
		.write_byte_enable(4'b0000),
		.address(address[11:2]),
		.data_write(),
		.data_read(data_read_boot)
	);
	
	// instruction and data memory
	bram #(
	)
	bram
	(
		.clk(clock),
		.addr(address[address_width - 1:2]),
		.cs_n(ram_enable_n),
		.we_n(data_w_n_ram),
		.data_i(data_write),
		.data_o(data_read_ram)
	);


endmodule
