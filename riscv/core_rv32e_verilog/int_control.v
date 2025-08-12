module interrupt_controller
(
	input wire clock,
	input wire reset,
	input wire stall,
	output wire stall_cpu,
	output wire mwait_cpu,
	output wire [31:0] irq_vector_cpu,
	output wire irq_cpu,
	input wire irq_ack_cpu,
	input wire exception_cpu,
	input wire [31:0] inst_addr_cpu,
	output wire [31:0] inst_in_cpu,
	input wire [31:0] data_addr_cpu,
	output wire [31:0] data_in_cpu,
	input wire [31:0] data_out_cpu,
	input wire [3:0] data_w_cpu,
	input wire data_access_cpu,
	output wire [31:0] addr_mem,
	input wire [31:0] data_read_mem,
	output wire [31:0] data_write_mem,
	output wire [3:0] data_we_mem,
	input wire [7:0] extio_in,
	output wire [7:0] extio_out
);

	wire [7:0] irq_cause; reg [7:0] irq_mask_reg; reg [7:0] irq_status_reg; reg [7:0] extio_out_reg;
	reg [31:0] periph_data; reg [31:0] irq_vector_reg; reg [31:0] irq_epc_reg; reg [31:0] inst_reg;
	wire interrupt; reg irq; reg data_access_cpu_dly;
	parameter [2:0] irq_idle = 0, irq_int = 1, irq_req = 2, irq_ackn = 3, irq_done = 4;

	reg [2:0] pulse_state;
	reg [2:0] pulse_next_state;
	wire periph_access; reg periph_access_dly; wire periph_access_we;
	wire [3:0] data_we_mem_s;

	// address decoder, read from peripheral registers
	always @(posedge clock or posedge reset) begin
		if (reset == 1'b1) begin
			periph_data <= {32{1'b0}};
		end else begin
			if (periph_access == 1'b1) begin
			case (data_addr_cpu[7:4])
			4'b0000 : begin periph_data <= irq_vector_reg; end			// IRQ_VECTOR		(RW)
			4'b0001 : begin periph_data <= {irq_cause[7:0],24'h000000}; end		// IRQ_CAUSE		(RO)
			4'b0010 : begin periph_data <= {irq_mask_reg[7:0],24'h000000}; end	// IRQ_MASK		(RW)
			4'b0011 : begin periph_data <= {irq_status_reg,24'h000000}; end		// IRQ_STATUS		(RW)
			4'b0100 : begin 							// IRQ_EPC		(RO)
				periph_data <= {irq_epc_reg[7:0],irq_epc_reg[15:8],irq_epc_reg[23:16],irq_epc_reg[31:24]};
			end
			4'b1000 : begin periph_data <= {extio_in,24'h000000}; end		// EXTIO_IN		(RO)
			4'b1001 : begin periph_data <= {extio_out_reg,24'h000000}; end		// EXTIO_OUT		(RW)
			default : begin periph_data <= data_read_mem; end
			endcase
			end
		end
	end

	assign inst_in_cpu = data_access_cpu == 1'b0 ? data_read_mem : inst_reg;
	assign data_in_cpu = periph_access_dly == 1'b0 ? data_read_mem : periph_data;

	// peripheral register logic, write to peripheral registers
	always @(posedge clock or posedge reset) begin
		if (reset == 1'b1) begin
			irq_vector_reg <= {32{1'b0}};
			irq_mask_reg <= {8{1'b0}};
			irq_status_reg <= {8{1'b0}};
			extio_out_reg <= {8{1'b0}};
		end else begin
			if (periph_access == 1'b1 && periph_access_we == 1'b1) begin
				case (data_addr_cpu[7:4])
				4'b0000 : begin				// IRQ_VECTOR
					irq_vector_reg <= {data_out_cpu[7:0],data_out_cpu[15:8],data_out_cpu[23:16],data_out_cpu[31:24]};
				end
				4'b0010 : begin				// IRQ_MASK
					irq_mask_reg <= data_out_cpu[31:24];
				end
				4'b0011 : begin				// IRQ_STATUS
					irq_status_reg <= data_out_cpu[31:24];
				end
				4'b1001 : begin				// EXTIO_OUT
					extio_out_reg <= data_out_cpu[31:24];
				end
				default : begin
				end
				endcase
			end
			if (irq_ack_cpu == 1'b1 || exception_cpu == 1'b1) begin
				irq_status_reg[0] <= 1'b0;		// IRQ_STATUS (clear master int bit on interrupt)
			end
		end
	end

	// EPC register register load on interrupts
	always @(posedge clock or posedge reset) begin
		if (reset == 1'b1) begin
			irq_epc_reg <= 32'h00000000;
		end else begin
			if (((irq == 1'b1 && irq_ack_cpu == 1'b0) || exception_cpu == 1'b1)) begin
				irq_epc_reg <= inst_addr_cpu;
			end
		end
	end

	// interrupt state machine
	always @(posedge clock or posedge reset) begin
		if (reset == 1'b1) begin
			pulse_state <= irq_idle;
			pulse_next_state <= irq_idle;
			irq <= 1'b0;
		end else begin
			if (stall == 1'b0) begin
				pulse_state <= pulse_next_state;
				case (pulse_state)
				irq_idle : begin
					if (interrupt == 1'b1 && irq_status_reg[0] == 1'b1) begin
						pulse_next_state <= irq_int;
					end
				end
				irq_int : begin
					irq <= 1'b1;
					pulse_next_state <= irq_req;
				end
				irq_req : begin
					if (irq_ack_cpu == 1'b1) begin
						irq <= 1'b0;
						pulse_next_state <= irq_ackn;
					end
				end
				irq_ackn : begin
					pulse_next_state <= irq_done;
				end
				irq_done : begin
					if (irq_status_reg[0] == 1'b1) begin
						pulse_next_state <= irq_idle;
					end
				end
				default : begin
					pulse_next_state <= irq_idle;
				end
				endcase
			end
		end
	end

	// data / peripheral access delay
	always @(posedge clock or posedge reset) begin
		if (reset == 1'b1) begin
			periph_access_dly <= 1'b0;
			data_access_cpu_dly <= 1'b0;
			inst_reg <= {32{1'b0}};
		end else begin
			if (stall == 1'b0) begin
				periph_access_dly <= periph_access;
				data_access_cpu_dly <= data_access_cpu;
				if ((data_access_cpu == 1'b0)) begin
					inst_reg <= data_read_mem;
				end
			end
		end
	end

	assign periph_access = data_addr_cpu[31:27] == 5'b11110 && data_access_cpu == 1'b1 ? 1'b1 : 1'b0;
	assign periph_access_we = periph_access <= 1'b1 && data_w_cpu != 4'b0000 ? 1'b1 : 1'b0;

	// memory address / write enable muxes
	assign addr_mem = data_access_cpu == 1'b1 ? data_addr_cpu : inst_addr_cpu;
	assign data_write_mem = data_out_cpu;
	assign data_we_mem_s = data_access_cpu == 1'b1 && periph_access == 1'b0 ? data_w_cpu : 4'b0000;
	assign data_we_mem = data_we_mem_s;
	assign mwait_cpu = data_access_cpu == 1'b1 && data_access_cpu_dly == 1'b0 ? 1'b1 : 1'b0;
	assign stall_cpu = stall;

	// interrupts masking
	assign interrupt = (irq_cause & irq_mask_reg) == 16'h0000 ? 1'b0 : 1'b1;
	assign irq_cause = extio_in;
	assign irq_cpu = irq;
	assign irq_vector_cpu = irq_vector_reg;
	assign extio_out = extio_out_reg;

endmodule
