module processor
(
	input wire clk_i,
	input wire rst_i,
	input wire stall_i,
	output wire [31:0] addr_o,
	input wire [31:0] data_i,
	output wire [31:0] data_o,
	output wire [3:0] data_w_o,
	output wire [2:0] data_mode_o,
	input wire [7:0] extio_in,
	output wire [7:0] extio_out
);

	wire stall_cpu; wire mwait_cpu; wire irq_cpu; wire irq_ack_cpu; wire exception_cpu; wire data_b_cpu; wire data_h_cpu; wire data_access_cpu;
	wire [31:0] irq_vector_cpu; wire [31:0] inst_addr_cpu; wire [31:0] inst_in_cpu; wire [31:0] data_addr_cpu; wire [31:0] data_in_cpu; wire [31:0] data_out_cpu;
	wire [3:0] data_w_cpu;

	assign data_mode_o = {data_b_cpu,data_h_cpu,data_access_cpu};

	// HF-RISC core
	datapath core
	(
		.clock(clk_i),
		.reset(rst_i),
		.stall(stall_cpu),
		.mwait(mwait_cpu),
		.irq_vector(irq_vector_cpu),
		.irq(irq_cpu),
		.irq_ack(irq_ack_cpu),
		.exception(exception_cpu),
		.inst_addr(inst_addr_cpu),
		.inst_in(inst_in_cpu),
		.data_addr(data_addr_cpu),
		.data_in(data_in_cpu),
		.data_out(data_out_cpu),
		.data_w(data_w_cpu),
		.data_b(data_b_cpu),
		.data_h(data_h_cpu),
		.data_access(data_access_cpu)
	);

	// interrupt controller
	interrupt_controller int_control
	(
		.clock(clk_i),
		.reset(rst_i),
		.stall(stall_i),
		.stall_cpu(stall_cpu),
		.mwait_cpu(mwait_cpu),
		.irq_vector_cpu(irq_vector_cpu),
		.irq_cpu(irq_cpu),
		.irq_ack_cpu(irq_ack_cpu),
		.exception_cpu(exception_cpu),
		.inst_addr_cpu(inst_addr_cpu),
		.inst_in_cpu(inst_in_cpu),
		.data_addr_cpu(data_addr_cpu),
		.data_in_cpu(data_in_cpu),
		.data_out_cpu(data_out_cpu),
		.data_w_cpu(data_w_cpu),
		.data_access_cpu(data_access_cpu),
		.addr_mem(addr_o),
		.data_read_mem(data_i),
		.data_write_mem(data_o),
		.data_we_mem(data_w_o),
		.extio_in(extio_in),
		.extio_out(extio_out)
	);


endmodule
