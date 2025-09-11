module bram #(
	parameter [31:0] ADDR_WIDTH=15
) (
	input wire clk,
	input wire [ADDR_WIDTH - 1:2] addr,
	input wire [3:0] cs_n,
	input wire [3:0] we_n,
	input wire [31:0] data_i,
	output reg [31:0] data_o
);

	reg [31:0] ram1[2 ** (ADDR_WIDTH - 2)-1:0];

	always @(posedge clk) begin
		if ((cs_n[3] == 1'b0)) begin
			if ((we_n[3] == 1'b0)) begin
				ram1[addr[ADDR_WIDTH - 1:2]][31:24] <= data_i[31:24];
			end
			else begin
				data_o[31:24] <= ram1[addr[ADDR_WIDTH - 1:2]][31:24];
			end
		end
		if ((cs_n[2] == 1'b0)) begin
			if ((we_n[2] == 1'b0)) begin
				ram1[addr[ADDR_WIDTH - 1:2]][23:16] <= data_i[23:16];
			end
			else begin
				data_o[23:16] <= ram1[addr[ADDR_WIDTH - 1:2]][23:16];
			end
		end
		if ((cs_n[1] == 1'b0)) begin
			if ((we_n[1] == 1'b0)) begin
				ram1[addr[ADDR_WIDTH - 1:2]][15:8] <= data_i[15:8];
			end
			else begin
				data_o[15:8] <= ram1[addr[ADDR_WIDTH - 1:2]][15:8];
			end
		end
		if ((cs_n[0] == 1'b0)) begin
			if ((we_n[0] == 1'b0)) begin
				ram1[addr[ADDR_WIDTH - 1:2]][7:0] <= data_i[7:0];
			end
			else begin
				data_o[7:0] <= ram1[addr[ADDR_WIDTH - 1:2]][7:0];
			end
		end
	end
	
endmodule
