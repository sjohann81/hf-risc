module control
(
	input wire [6:0] opcode,
	input wire [2:0] funct3,
	input wire [6:0] funct7,
	output reg reg_write,
	output reg alu_src1,
	output reg [2:0] alu_src2,
	output reg [3:0] alu_op,
	output reg [1:0] jump,
	output reg [2:0] branch,
	output reg [1:0] mem_write,
	output reg [1:0] mem_read,
	output reg sig_read
);

	always @(opcode, funct3, funct7) begin
		case (opcode)			// load immediate / jumps
		7'b0110111 : begin		// LUI
			reg_write <= 1'b1;
			alu_src1 <= 1'b0;
			alu_src2 <= 3'b000;
			alu_op <= 4'b0110;
			jump <= 2'b00;
			branch <= 3'b000;
			mem_write <= 2'b00;
			mem_read <= 2'b00;
			sig_read <= 1'b0;
		end
		7'b0010111 : begin		// AUIPC
			reg_write <= 1'b1;
			alu_src1 <= 1'b1;
			alu_src2 <= 3'b000;
			alu_op <= 4'b0100;
			jump <= 2'b00;
			branch <= 3'b000;
			mem_write <= 2'b00;
			mem_read <= 2'b00;
			sig_read <= 1'b0;
		end
		7'b1101111 : begin		// JAL
			reg_write <= 1'b1;
			alu_src1 <= 1'b0;
			alu_src2 <= 3'b011;
			alu_op <= 4'b0110;
			jump <= 2'b10;
			branch <= 3'b000;
			mem_write <= 2'b00;
			mem_read <= 2'b00;
			sig_read <= 1'b0;
		end
		7'b1100111 : begin		// JALR
			reg_write <= 1'b1;
			alu_src1 <= 1'b0;
			alu_src2 <= 3'b011;
			alu_op <= 4'b0110;
			jump <= 2'b11;
			branch <= 3'b000;
			mem_write <= 2'b00;
			mem_read <= 2'b00;
			sig_read <= 1'b0;
		end
		7'b1100011 : begin		// branches
			case (funct3)
			3'b000 : begin		// BEQ
				reg_write <= 1'b0;
				alu_src1 <= 1'b0;
				alu_src2 <= 3'b101;
				alu_op <= 4'b0101;
				jump <= 2'b00;
				branch <= 3'b001;
				mem_write <= 2'b00;
				mem_read <= 2'b00;
				sig_read <= 1'b0;
			end
			3'b001 : begin		// BNE
				reg_write <= 1'b0;
				alu_src1 <= 1'b0;
				alu_src2 <= 3'b101;
				alu_op <= 4'b0101;
				jump <= 2'b00;
				branch <= 3'b010;
				mem_write <= 2'b00;
				mem_read <= 2'b00;
				sig_read <= 1'b0;
			end
			3'b100 : begin		// BLT
				reg_write <= 1'b0;
				alu_src1 <= 1'b0;
				alu_src2 <= 3'b101;
				alu_op <= 4'b0111;
				jump <= 2'b00;
				branch <= 3'b011;
				mem_write <= 2'b00;
				mem_read <= 2'b00;
				sig_read <= 1'b0;
			end
			3'b101 : begin		// BGE
				reg_write <= 1'b0;
				alu_src1 <= 1'b0;
				alu_src2 <= 3'b101;
				alu_op <= 4'b0111;
				jump <= 2'b00;
				branch <= 3'b100;
				mem_write <= 2'b00;
				mem_read <= 2'b00;
				sig_read <= 1'b0;
			end
			3'b110 : begin		// BLTU
				reg_write <= 1'b0;
				alu_src1 <= 1'b0;
				alu_src2 <= 3'b101;
				alu_op <= 4'b1000;
				jump <= 2'b00;
				branch <= 3'b101;
				mem_write <= 2'b00;
				mem_read <= 2'b00;
				sig_read <= 1'b0;
			end
			3'b111 : begin		// BGEU
				reg_write <= 1'b0;
				alu_src1 <= 1'b0;
				alu_src2 <= 3'b101;
				alu_op <= 4'b1000;
				jump <= 2'b00;
				branch <= 3'b110;
				mem_write <= 2'b00;
				mem_read <= 2'b00;
				sig_read <= 1'b0;
			end
			default : begin
				reg_write <= 1'b0;
				alu_src1 <= 1'b0;
				alu_src2 <= 3'b000;
				alu_op <= 4'b0000;
				jump <= 2'b00;
				branch <= 3'b000;
				mem_write <= 2'b00;
				mem_read <= 2'b00;
				sig_read <= 1'b0;
			end
			endcase
		end
		7'b0000011 : begin		// loads
			case (funct3)
			3'b000 : begin		// LB
				reg_write <= 1'b1;
				alu_src1 <= 1'b0;
				alu_src2 <= 3'b001;
				alu_op <= 4'b0100;
				jump <= 2'b00;
				branch <= 3'b000;
				mem_write <= 2'b00;
				mem_read <= 2'b01;
				sig_read <= 1'b1;
			end
			3'b001 : begin		// LH
				reg_write <= 1'b1;
				alu_src1 <= 1'b0;
				alu_src2 <= 3'b001;
				alu_op <= 4'b0100;
				jump <= 2'b00;
				branch <= 3'b000;
				mem_write <= 2'b00;
				mem_read <= 2'b10;
				sig_read <= 1'b1;
			end
			3'b010 : begin		// LW
				reg_write <= 1'b1;
				alu_src1 <= 1'b0;
				alu_src2 <= 3'b001;
				alu_op <= 4'b0100;
				jump <= 2'b00;
				branch <= 3'b000;
				mem_write <= 2'b00;
				mem_read <= 2'b11;
				sig_read <= 1'b1;
			end
			3'b100 : begin		// LBU
				reg_write <= 1'b1;
				alu_src1 <= 1'b0;
				alu_src2 <= 3'b001;
				alu_op <= 4'b0100;
				jump <= 2'b00;
				branch <= 3'b000;
				mem_write <= 2'b00;
				mem_read <= 2'b01;
				sig_read <= 1'b0;
			end
			3'b101 : begin		// LHU
				reg_write <= 1'b1;
				alu_src1 <= 1'b0;
				alu_src2 <= 3'b001;
				alu_op <= 4'b0100;
				jump <= 2'b00;
				branch <= 3'b000;
				mem_write <= 2'b00;
				mem_read <= 2'b10;
				sig_read <= 1'b0;
			end
			default : begin
				reg_write <= 1'b0;
				alu_src1 <= 1'b0;
				alu_src2 <= 3'b000;
				alu_op <= 4'b0000;
				jump <= 2'b00;
				branch <= 3'b000;
				mem_write <= 2'b00;
				mem_read <= 2'b00;
				sig_read <= 1'b0;
			end
			endcase
		end
		7'b0100011 : begin		// stores
			case (funct3)
			3'b000 : begin		// SB
				reg_write <= 1'b0;
				alu_src1 <= 1'b0;
				alu_src2 <= 3'b010;
				alu_op <= 4'b0100;
				jump <= 2'b00;
				branch <= 3'b000;
				mem_write <= 2'b01;
				mem_read <= 2'b00;
				sig_read <= 1'b0;
			end
			3'b001 : begin		// SH
				reg_write <= 1'b0;
				alu_src1 <= 1'b0;
				alu_src2 <= 3'b010;
				alu_op <= 4'b0100;
				jump <= 2'b00;
				branch <= 3'b000;
				mem_write <= 2'b10;
				mem_read <= 2'b00;
				sig_read <= 1'b0;
			end
			3'b010 : begin		// SW
				reg_write <= 1'b0;
				alu_src1 <= 1'b0;
				alu_src2 <= 3'b010;
				alu_op <= 4'b0100;
				jump <= 2'b00;
				branch <= 3'b000;
				mem_write <= 2'b11;
				mem_read <= 2'b00;
				sig_read <= 1'b0;
			end
			default : begin
				reg_write <= 1'b0;
				alu_src1 <= 1'b0;
				alu_src2 <= 3'b000;
				alu_op <= 4'b0000;
				jump <= 2'b00;
				branch <= 3'b000;
				mem_write <= 2'b00;
				mem_read <= 2'b00;
				sig_read <= 1'b0;
			end
			endcase
		end
		7'b0010011 : begin		// imm computation
			case (funct3)
			3'b000 : begin		// ADDI
				reg_write <= 1'b1;
				alu_src1 <= 1'b0;
				alu_src2 <= 3'b001;
				alu_op <= 4'b0100;
				jump <= 2'b00;
				branch <= 3'b000;
				mem_write <= 2'b00;
				mem_read <= 2'b00;
				sig_read <= 1'b0;
			end
			3'b010 : begin		// SLTI
				reg_write <= 1'b1;
				alu_src1 <= 1'b0;
				alu_src2 <= 3'b001;
				alu_op <= 4'b0111;
				jump <= 2'b00;
				branch <= 3'b000;
				mem_write <= 2'b00;
				mem_read <= 2'b00;
				sig_read <= 1'b0;
			end
			3'b011 : begin		// SLTIU
				reg_write <= 1'b1;
				alu_src1 <= 1'b0;
				alu_src2 <= 3'b001;
				alu_op <= 4'b1000;
				jump <= 2'b00;
				branch <= 3'b000;
				mem_write <= 2'b00;
				mem_read <= 2'b00;
				sig_read <= 1'b0;
			end
			3'b100 : begin		// XORI
				reg_write <= 1'b1;
				alu_src1 <= 1'b0;
				alu_src2 <= 3'b001;
				alu_op <= 4'b0010;
				jump <= 2'b00;
				branch <= 3'b000;
				mem_write <= 2'b00;
				mem_read <= 2'b00;
				sig_read <= 1'b0;
			end
			3'b110 : begin		// ORI
				reg_write <= 1'b1;
				alu_src1 <= 1'b0;
				alu_src2 <= 3'b001;
				alu_op <= 4'b0001;
				jump <= 2'b00;
				branch <= 3'b000;
				mem_write <= 2'b00;
				mem_read <= 2'b00;
				sig_read <= 1'b0;
			end
			3'b111 : begin		// ANDI
				reg_write <= 1'b1;
				alu_src1 <= 1'b0;
				alu_src2 <= 3'b001;
				alu_op <= 4'b0000;
				jump <= 2'b00;
				branch <= 3'b000;
				mem_write <= 2'b00;
				mem_read <= 2'b00;
				sig_read <= 1'b0;
			end
			3'b001 : begin		// SLLI
				reg_write <= 1'b1;
				alu_src1 <= 1'b0;
				alu_src2 <= 3'b100;
				alu_op <= 4'b1001;
				jump <= 2'b00;
				branch <= 3'b000;
				mem_write <= 2'b00;
				mem_read <= 2'b00;
				sig_read <= 1'b0;
			end
			3'b101 : begin
				case (funct7)
				7'b0000000 : begin	// SRLI
					reg_write <= 1'b1;
					alu_src1 <= 1'b0;
					alu_src2 <= 3'b100;
					alu_op <= 4'b1010;
					jump <= 2'b00;
					branch <= 3'b000;
					mem_write <= 2'b00;
					mem_read <= 2'b00;
					sig_read <= 1'b0;
				end
				7'b0100000 : begin	// SRAI
					reg_write <= 1'b1;
					alu_src1 <= 1'b0;
					alu_src2 <= 3'b100;
					alu_op <= 4'b1100;
					jump <= 2'b00;
					branch <= 3'b000;
					mem_write <= 2'b00;
					mem_read <= 2'b00;
					sig_read <= 1'b0;
				end
				default : begin
					reg_write <= 1'b0;
					alu_src1 <= 1'b0;
					alu_src2 <= 3'b000;
					alu_op <= 4'b0000;
					jump <= 2'b00;
					branch <= 3'b000;
					mem_write <= 2'b00;
					mem_read <= 2'b00;
					sig_read <= 1'b0;
				end
				endcase
			end
			default : begin
				reg_write <= 1'b0;
				alu_src1 <= 1'b0;
				alu_src2 <= 3'b000;
				alu_op <= 4'b0000;
				jump <= 2'b00;
				branch <= 3'b000;
				mem_write <= 2'b00;
				mem_read <= 2'b00;
				sig_read <= 1'b0;
			end
			endcase
		end
		7'b0110011 : begin		// computation
			case (funct3)
			3'b000 : begin
				case (funct7)
				7'b0000000 : begin
				// ADD
				reg_write <= 1'b1;
				alu_src1 <= 1'b0;
				alu_src2 <= 3'b101;
				alu_op <= 4'b0100;
				jump <= 2'b00;
				branch <= 3'b000;
				mem_write <= 2'b00;
				mem_read <= 2'b00;
				sig_read <= 1'b0;
				end
				7'b0100000 : begin
				// SUB
				reg_write <= 1'b1;
				alu_src1 <= 1'b0;
				alu_src2 <= 3'b101;
				alu_op <= 4'b0101;
				jump <= 2'b00;
				branch <= 3'b000;
				mem_write <= 2'b00;
				mem_read <= 2'b00;
				sig_read <= 1'b0;
				end
				default : begin
				reg_write <= 1'b0;
				alu_src1 <= 1'b0;
				alu_src2 <= 3'b000;
				alu_op <= 4'b0000;
				jump <= 2'b00;
				branch <= 3'b000;
				mem_write <= 2'b00;
				mem_read <= 2'b00;
				sig_read <= 1'b0;
				end
				endcase
			end
			3'b001 : begin			// SLL
				reg_write <= 1'b1;
				alu_src1 <= 1'b0;
				alu_src2 <= 3'b101;
				alu_op <= 4'b1001;
				jump <= 2'b00;
				branch <= 3'b000;
				mem_write <= 2'b00;
				mem_read <= 2'b00;
				sig_read <= 1'b0;
			end
			3'b010 : begin			// SLT
				reg_write <= 1'b1;
				alu_src1 <= 1'b0;
				alu_src2 <= 3'b101;
				alu_op <= 4'b0111;
				jump <= 2'b00;
				branch <= 3'b000;
				mem_write <= 2'b00;
				mem_read <= 2'b00;
				sig_read <= 1'b0;
			end
			3'b011 : begin			// SLTU
				reg_write <= 1'b1;
				alu_src1 <= 1'b0;
				alu_src2 <= 3'b101;
				alu_op <= 4'b1000;
				jump <= 2'b00;
				branch <= 3'b000;
				mem_write <= 2'b00;
				mem_read <= 2'b00;
				sig_read <= 1'b0;
			end
			3'b100 : begin			// XOR
				reg_write <= 1'b1;
				alu_src1 <= 1'b0;
				alu_src2 <= 3'b101;
				alu_op <= 4'b0010;
				jump <= 2'b00;
				branch <= 3'b000;
				mem_write <= 2'b00;
				mem_read <= 2'b00;
				sig_read <= 1'b0;
			end
			3'b101 : begin
				case (funct7)		
				7'b0000000 : begin	// SRL
					reg_write <= 1'b1;
					alu_src1 <= 1'b0;
					alu_src2 <= 3'b101;
					alu_op <= 4'b1010;
					jump <= 2'b00;
					branch <= 3'b000;
					mem_write <= 2'b00;
					mem_read <= 2'b00;
					sig_read <= 1'b0;
				end
				7'b0100000 : begin	// SRA
					reg_write <= 1'b1;
					alu_src1 <= 1'b0;
					alu_src2 <= 3'b101;
					alu_op <= 4'b1100;
					jump <= 2'b00;
					branch <= 3'b000;
					mem_write <= 2'b00;
					mem_read <= 2'b00;
					sig_read <= 1'b0;
				end
				default : begin
					reg_write <= 1'b0;
					alu_src1 <= 1'b0;
					alu_src2 <= 3'b000;
					alu_op <= 4'b0000;
					jump <= 2'b00;
					branch <= 3'b000;
					mem_write <= 2'b00;
					mem_read <= 2'b00;
					sig_read <= 1'b0;
				end
				endcase
			end
			3'b110 : begin			// OR
				reg_write <= 1'b1;
				alu_src1 <= 1'b0;
				alu_src2 <= 3'b101;
				alu_op <= 4'b0001;
				jump <= 2'b00;
				branch <= 3'b000;
				mem_write <= 2'b00;
				mem_read <= 2'b00;
				sig_read <= 1'b0;
			end
			3'b111 : begin			// AND
				reg_write <= 1'b1;
				alu_src1 <= 1'b0;
				alu_src2 <= 3'b101;
				alu_op <= 4'b0000;
				jump <= 2'b00;
				branch <= 3'b000;
				mem_write <= 2'b00;
				mem_read <= 2'b00;
				sig_read <= 1'b0;
			end
			default : begin
				reg_write <= 1'b0;
				alu_src1 <= 1'b0;
				alu_src2 <= 3'b000;
				alu_op <= 4'b0000;
				jump <= 2'b00;
				branch <= 3'b000;
				mem_write <= 2'b00;
				mem_read <= 2'b00;
				sig_read <= 1'b0;
			end
			endcase
		end
		7'b1110011 : begin		// SYSTEM
			reg_write <= 1'b0;
			alu_src1 <= 1'b0;
			alu_src2 <= 3'b000;
			alu_op <= 4'b0000;
			jump <= 2'b00;
			branch <= 3'b111;
			mem_write <= 2'b00;
			mem_read <= 2'b00;
			sig_read <= 1'b0;
		end
		default : begin
			reg_write <= 1'b0;
			alu_src1 <= 1'b0;
			alu_src2 <= 3'b000;
			alu_op <= 4'b0000;
			jump <= 2'b00;
			branch <= 3'b000;
			mem_write <= 2'b00;
			mem_read <= 2'b00;
			sig_read <= 1'b0;
		end
		endcase
	end

endmodule
