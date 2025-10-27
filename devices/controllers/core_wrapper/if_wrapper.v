module if_wrapper
(
	// CPU bus signals
	input wire clk_i,
	input wire rst_i,
	input wire [31:0] addr_i,
	input wire [31:0] data_i,
	output wire [31:0] data_o,
	output wire data_access_o,
	input wire data_w_i
);

	parameter [31:0] SOC_SEGMENT='He2;		// SoC segment address is 0xe2
	parameter [31:0] SOC_CLASS='H00;		// Class address is 0x00

	// wrapper internal signals
	wire data_access;
	wire [15:0] device;
	
	// registers
	reg [31:0] data_read_wrapper;
	reg [11:0] control;
	reg [127:0] data;

	assign device = addr_i[15:0];
	assign data_access = addr_i[31:24] == (SOC_SEGMENT) && addr_i[23:16] == (SOC_CLASS) ? 1'b1 : 1'b0;
	assign data_access_o = data_access;
	assign data_o = data_read_wrapper;

	// MMIO register read operations
	always @(posedge clk_i, posedge rst_i) begin
		if (rst_i == 1'b1) begin
			data_read_wrapper <= {32{1'b0}};
		end else begin
			if((data_access == 1'b1)) begin
				case (device[7:4])
				4'b0000 : begin			// control	0xe2000000
					data_read_wrapper <= {16'h0000,4'b0000,control};
				end
				4'b0001 : begin			// data[0]	0xe2000010
					data_read_wrapper <= data[127:96];
				end
				4'b0010 : begin			// data[1]	0xe2000020
					data_read_wrapper <= data[95:64];
				end
				4'b0011 : begin			// data[2]	0xe2000030
					data_read_wrapper <= data[63:32];
				end
				4'b0100 : begin			// data[3]	0xe2000040
					data_read_wrapper <= data[31:0];
				end
				default : begin
					data_read_wrapper <= {32{1'b0}};
				end
				endcase
			end
		end
	end

	// MMIO register write operations
	always @(posedge clk_i, posedge rst_i) begin
		if (rst_i == 1'b1) begin
			control <= {12{1'b0}};
			data <= {128{1'b0}};
		end else begin
			if ((data_access == 1'b1 && data_w_i == 1'b1)) begin
				case (device[7:4])
				4'b0000 : begin			// control	0xe2000000
					control <= data_i[11:0];
				end
				4'b0001 : begin			// data[0]	0xe2000010
					data[127:96] <= data_i;
				end
				4'b0010 : begin			// data[1]	0xe2000020
					data[95:64] <= data_i;
				end
				4'b0011 : begin			// data[2]	0xe2000030
					data[63:32] <= data_i;
				end
				4'b0100 : begin			// data[3]	0xe2000040
					data[31:0] <= data_i;
				end
				default : begin
				end
				endcase
			end
		end
	end

	// user logic ...

endmodule
