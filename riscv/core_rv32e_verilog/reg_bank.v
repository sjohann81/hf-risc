module reg_bank
(
	input wire clock,
	input wire [3:0] read_reg1,
	input wire [3:0] read_reg2,
	input wire [3:0] write_reg,
	input wire wreg,
	input wire [31:0] write_data,
	output wire [31:0] read_data1,
	output wire [31:0] read_data2
);

	reg [31:0] registers[0:15];

	always @(posedge clock) begin
		if (write_reg != 4'b0000 && wreg == 1'b1) begin
			registers[write_reg] <= write_data;
		end
	end

	assign read_data1 = read_reg1 != 4'b0000 ? registers[read_reg1] : {32{1'b0}};
	assign read_data2 = read_reg2 != 4'b0000 ? registers[read_reg2] : {32{1'b0}};

endmodule
