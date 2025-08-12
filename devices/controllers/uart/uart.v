module uart
(
	input wire clk,
	input wire reset,
	input wire [11:0] divisor,
	input wire enable_read,
	input wire enable_write,
	input wire [7:0] data_in,
	output wire [7:0] data_out,
	input wire uart_read,
	output wire uart_write,
	output wire busy_write,
	output wire data_avail
);

	reg [11:0] delay_write_reg;
	reg [3:0] bits_write_reg;
	reg [8:0] data_write_reg;
	reg [11:0] delay_read_reg;
	reg [3:0] bits_read_reg;
	reg [7:0] data_read_reg;
	reg [8:0] data_save_reg;
	wire busy_write_sig;
	reg [7:0] read_value_reg;
	wire uart_read2;

	always @(posedge clk or posedge reset) begin
		if (reset == 1'b1) begin
			data_write_reg <= {8'h00,1'b1};
			bits_write_reg <= 4'b0000;
			delay_write_reg <= {12{1'b0}};
			read_value_reg <= {8{1'b1}};
			data_read_reg <= {8{1'b0}};
			bits_read_reg <= 4'b0000;
			delay_read_reg <= {12{1'b0}};
			data_save_reg <= {9{1'b0}};
		end else begin
			// UART write
			if (bits_write_reg == 4'b0000) begin					//nothing left to write?
				if (enable_write == 1'b1) begin
					delay_write_reg <= {12{1'b0}};				//delay before next bit
					bits_write_reg <= 4'b1010;				//number of bits to write
					data_write_reg <= {data_in,1'b0};			//remember data & start bit
				end
			end
			else begin
				if (delay_write_reg != divisor) begin
					delay_write_reg <= delay_write_reg + 1;			//delay before next bit
				end
				else begin
					delay_write_reg <= {12{1'b0}};				//reset delay
					bits_write_reg <= bits_write_reg - 1;			//bits left to write
					data_write_reg <= {1'b1,data_write_reg[8:1]};
				end
			end
			
			// average uart_read signal
			if (uart_read == 1'b1) begin
				if (read_value_reg != 8'hff) begin
					read_value_reg <= read_value_reg + 1;
				end
			end
			else begin
				if (read_value_reg != 8'h00) begin
					read_value_reg <= read_value_reg - 1;
				end
			end
			
			// UART read
			if (delay_read_reg == 16'h0000) begin					//done delay for read?
				if (bits_read_reg == 4'b0000) begin				//nothing left to read?
					if (uart_read2 == 1'b0) begin				//wait for start bit
						delay_read_reg <= {1'b0,divisor[11:1]};		//half period
						bits_read_reg <= 4'b1001;			//bits left to read
					end
				end
				else begin
					delay_read_reg <= divisor;				//initialize delay
					bits_read_reg <= bits_read_reg - 1;			//bits left to read
					data_read_reg <= {uart_read2,data_read_reg[7:1]};
				end
			end
			else begin
				delay_read_reg <= delay_read_reg - 1;				//delay
			end
			
			if (bits_read_reg == 4'b0000 && delay_read_reg == divisor) begin
				data_save_reg <= {1'b1,data_read_reg};
			end
			else if (enable_read == 1'b1) begin
				data_save_reg[8] <= 1'b0;					//data_available
			end
		end
	end

	assign uart_read2 = read_value_reg[7];
	assign busy_write_sig = bits_write_reg != 4'b0000 ? 1'b1 : 1'b0;
	assign uart_write = data_write_reg[0];
	assign busy_write = busy_write_sig;
	assign data_avail = data_save_reg[8];
	assign data_out = data_save_reg[7:0];

endmodule
