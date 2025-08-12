module bshift
(
	input wire lshift,
	input wire logical,
	input wire [4:0] shift,
	input wire [31:0] in,
	output wire [31:0] out
);

	wire [31:0] shift1l; wire [31:0] shift2l; wire [31:0] shift4l; wire [31:0] shift8l; wire [31:0] shift16l;
	wire [31:0] shift1r; wire [31:0] shift2r; wire [31:0] shift4r; wire [31:0] shift8r; wire [31:0] shift16r;
	wire [31:16] fill;

	assign fill = logical == 1'b0 ? {16{in[31]}} : 16'h0000;
	assign shift1l = shift[0] == 1'b1 ? {in[30:0],1'b0} : in;
	assign shift2l = shift[1] == 1'b1 ? {shift1l[29:0],2'b00} : shift1l;
	assign shift4l = shift[2] == 1'b1 ? {shift2l[27:0],4'h0} : shift2l;
	assign shift8l = shift[3] == 1'b1 ? {shift4l[23:0],8'h00} : shift4l;
	assign shift16l = shift[4] == 1'b1 ? {shift8l[15:0],16'h0000} : shift8l;
	assign shift1r = shift[0] == 1'b1 ? {fill[31],in[31:1]} : in;
	assign shift2r = shift[1] == 1'b1 ? {fill[31:30],shift1r[31:2]} : shift1r;
	assign shift4r = shift[2] == 1'b1 ? {fill[31:28],shift2r[31:4]} : shift2r;
	assign shift8r = shift[3] == 1'b1 ? {fill[31:24],shift4r[31:8]} : shift4r;
	assign shift16r = shift[4] == 1'b1 ? {fill[31:16],shift8r[31:16]} : shift8r;
	assign out = lshift == 1'b0 ? shift16r : shift16l;

endmodule
