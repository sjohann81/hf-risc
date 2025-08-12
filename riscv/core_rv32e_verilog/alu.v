module alu
(
	input wire [31:0] op1,
	input wire [31:0] op2,
	input wire [3:0] alu_op,
	output wire [31:0] result,
	output wire zero,
	output wire less_than
);

	reg [31:0] r; wire [31:0] shift;
	wire [4:0] shift_op2;
	wire [32:0] addsub;
	wire less; wire lshift; wire logical;

	always @(op1, op2, alu_op, addsub, less, shift_op2, shift) begin
		case (alu_op)
		4'b0000 : begin r <= op1 & op2; end
		4'b0001 : begin r <= op1 | op2; end
		4'b0010 : begin r <= op1 ^ op2; end
		4'b0100, 4'b0101 : begin r <= addsub[31:0]; end
		4'b0110 : begin r <= op2; end
		4'b0111,4'b1000 : begin r <= {28'h0000000,3'b000,less}; end
		default : begin r <= shift; end
		endcase
	end

	assign addsub = alu_op > 4'b0100 ? ({1'b0,op1}) - ({1'b0,op2}) : ({1'b0,op1}) + ({1'b0,op2});
	assign less = op1[31] == op2[31] || alu_op == 4'b1000 ? addsub[32] : op1[31];
	assign less_than = less;
	assign zero =  ~(r[31] | r[30] | r[29] | r[28] | r[27] | r[26] | 
		r[25] | r[24] | r[23] | r[22] | r[21] | r[20] | r[19] | 
		r[18] | r[17] | r[16] | r[15] | r[14] | r[13] | r[12] | 
		r[11] | r[10] | r[9] | r[8] | r[7] | r[6] | r[5] | r[4] | 
		r[3] | r[2] | r[1] | r[0]);
	assign shift_op2 = op2[4:0];
	assign lshift = alu_op[0] == 1'b1 ? 1'b1 : 1'b0;
	assign logical = alu_op[2] == 1'b0 ? 1'b1 : 1'b0;
	
	bshift barrel_shifter
	(
		.lshift(lshift),
		.logical(logical),
		.shift(shift_op2),
		.in(op1),
		.out(shift)
	);

	assign result = r;

endmodule
