module datapath
(
	input wire clock,
	input wire reset,
	input wire stall,
	input wire mwait,
	input wire [31:0] irq_vector,
	input wire irq,
	output wire irq_ack,
	output wire exception,
	output wire [31:0] inst_addr,
	input wire [31:0] inst_in,
	output wire [31:0] data_addr,
	input wire [31:0] data_in,
	output reg [31:0] data_out,
	output reg [3:0] data_w,
	output wire data_b,
	output wire data_h,
	output wire data_access
);

	// datapath signals
	wire [31:0] inst_in_s; reg [31:0] data_in_s; reg [31:0] pc; reg [31:0] pc_last;
	reg [31:0] pc_last2; wire [31:0] pc_plus4; wire [31:0] pc_next;
	wire [31:0] result; wire [31:0] branch; wire [31:0] ext32b;
	wire [31:0] ext32h; wire [31:0] alu_src1; wire [31:0] alu_src2;
	wire [31:12] ext32;
	wire [6:0] opcode; wire [6:0] funct7;
	wire [2:0] funct3;
	wire [3:0] read_reg1; wire [3:0] read_reg2; wire [3:0] write_reg;
	wire [4:0] rs1; wire [4:0] rs2; wire [4:0] rd;
	wire [31:0] write_data; wire [31:0] read_data1; wire [31:0] read_data2;
	wire [31:0] imm_i; wire [31:0] imm_s; wire [31:0] imm_sb;
	wire [31:0] imm_uj; wire [31:0] branch_src1; wire [31:0] branch_src2;
	wire [31:12] imm_u;
	wire wreg; wire zero; wire less_than; wire branch_taken; wire jump_taken; reg stall_reg;
	wire irq_ack_s; reg irq_ack_s_dly; reg bds; wire data_access_s; reg data_access_s_dly;
	
	// control signals
	wire reg_write_ctl; wire alu_src1_ctl; wire sig_read_ctl; wire reg_to_mem; wire mem_to_reg; wire except;
	wire [1:0] jump_ctl; wire [1:0] mem_write_ctl; wire [1:0] mem_read_ctl;
	wire [2:0] alu_src2_ctl; wire [2:0] branch_ctl;
	wire [3:0] alu_op_ctl;
	reg [4:0] rs1_r; reg [4:0] rs2_r; reg [4:0] rd_r;
	reg [31:0] imm_i_r; reg [31:0] imm_s_r; reg [31:0] imm_sb_r; reg [31:0] imm_uj_r;
	reg [31:12] imm_u_r;
	reg reg_write_ctl_r; reg alu_src1_ctl_r; reg sig_read_ctl_r; reg reg_to_mem_r; reg mem_to_reg_r;
	reg [1:0] jump_ctl_r; reg [1:0] mem_write_ctl_r; reg [1:0] mem_read_ctl_r;
	reg [2:0] alu_src2_ctl_r; reg [2:0] branch_ctl_r;
	reg [3:0] alu_op_ctl_r;

	//
	// FETCH STAGE
	//
	// 1st stage, instruction memory access, PC update, interrupt acknowledge logic

	// program counter logic
	always @(posedge clock or posedge reset) begin
		if (reset == 1'b1) begin
			pc <= {32{1'b0}};
			pc_last <= {32{1'b0}};
			pc_last2 <= {32{1'b0}};
		end else begin
			if (stall == 1'b0) begin
				pc <= pc_next;
				pc_last <= pc;
				pc_last2 <= pc_last;
			end
		end
	end

	assign pc_plus4 = pc + 4;
	assign pc_next = (irq == 1'b1 && irq_ack_s == 1'b1) || except == 1'b1 ? 
	irq_vector : branch_taken == 1'b1 || jump_taken == 1'b1 ? 
	branch : data_access_s == 1'b1 ?
	pc_last : pc_plus4;

	// interrupt acknowledge logic
	assign irq_ack_s = irq == 1'b1 && bds == 1'b0 && branch_taken == 1'b0 && jump_taken == 1'b0 && reg_to_mem_r == 1'b0 && mem_to_reg_r == 1'b0 ? 1'b1 : 1'b0;
	assign irq_ack = irq_ack_s_dly;
	assign exception = except == 1'b1 ? 1'b1 : 1'b0;

	always @(posedge clock or posedge reset) begin
		if (reset == 1'b1) begin
			irq_ack_s_dly <= 1'b0;
			bds <= 1'b0;
			data_access_s_dly <= 1'b0;
			stall_reg <= 1'b0;
		end else begin
			stall_reg <= stall;
			if (stall == 1'b0) begin
				data_access_s_dly <= data_access_s;
				if (mwait == 1'b0) begin
					irq_ack_s_dly <= irq_ack_s;
					if (branch_taken == 1'b1 || jump_taken == 1'b1) begin
						bds <= 1'b1;
					end
					else begin
						bds <= 1'b0;
					end
				end
			end
		end
	end

	//
	// DECODE STAGE
	//
	// 2nd stage, instruction decode, control unit operation, pipeline bubble insertion logic on load/store and branches
	// pipeline bubble insertion on loads/stores, exceptions, branches and interrupts
	assign inst_in_s = data_access_s == 1'b1 || except == 1'b1 || branch_taken == 1'b1 || 
		jump_taken == 1'b1 || bds == 1'b1 || irq_ack_s == 1'b1 ? 32'h00000000 : 
		{inst_in[7:0],inst_in[15:8],inst_in[23:16],inst_in[31:24]};
	// instruction decode
	assign opcode = inst_in_s[6:0];
	assign funct3 = inst_in_s[14:12];
	assign funct7 = inst_in_s[31:25];
	assign rd = inst_in_s[11:7];
	assign rs1 = inst_in_s[19:15];
	assign rs2 = inst_in_s[24:20];
	assign imm_i = {ext32[31:12],inst_in_s[31:20]};
	assign imm_s = {ext32[31:12],inst_in_s[31:25],inst_in_s[11:7]};
	assign imm_sb = {ext32[31:13],inst_in_s[31],inst_in_s[7],inst_in_s[30:25],inst_in_s[11:8],1'b0};
	assign imm_u = inst_in_s[31:12];
	assign imm_uj = {ext32[31:21],inst_in_s[31],inst_in_s[19:12],inst_in_s[20],inst_in_s[30:21],1'b0};
	assign ext32 = inst_in_s[31] == 1'b1 ? {20{1'b1}} : {20{1'b0}};
	
	// control unit
	control control_unit
	(
		.opcode(opcode),
		.funct3(funct3),
		.funct7(funct7),
		.reg_write(reg_write_ctl),
		.alu_src1(alu_src1_ctl),
		.alu_src2(alu_src2_ctl),
		.alu_op(alu_op_ctl),
		.jump(jump_ctl),
		.branch(branch_ctl),
		.mem_write(mem_write_ctl),
		.mem_read(mem_read_ctl),
		.sig_read(sig_read_ctl)
	);

	assign reg_to_mem = mem_write_ctl != 2'b00 ? 1'b1 : 1'b0;
	assign mem_to_reg = mem_read_ctl != 2'b00 ? 1'b1 : 1'b0;

	always @(posedge clock or posedge reset) begin
		if (reset == 1'b1) begin
			rd_r <= {5{1'b0}};
			rs1_r <= {5{1'b0}};
			rs2_r <= {5{1'b0}};
			imm_i_r <= {32{1'b0}};
			imm_s_r <= {32{1'b0}};
			imm_sb_r <= {32{1'b0}};
			imm_u_r <= {20{1'b0}};
			imm_uj_r <= {32{1'b0}};
			reg_write_ctl_r <= 1'b0;
			alu_src1_ctl_r <= 1'b0;
			alu_src2_ctl_r <= {3{1'b0}};
			alu_op_ctl_r <= {4{1'b0}};
			jump_ctl_r <= {2{1'b0}};
			branch_ctl_r <= {3{1'b0}};
			mem_write_ctl_r <= {2{1'b0}};
			mem_read_ctl_r <= {2{1'b0}};
			sig_read_ctl_r <= 1'b0;
			reg_to_mem_r <= 1'b0;
			mem_to_reg_r <= 1'b0;
		end else begin
			if (stall == 1'b0 && mwait == 1'b0) begin
				rd_r <= rd;
				rs1_r <= rs1;
				rs2_r <= rs2;
				imm_i_r <= imm_i;
				imm_s_r <= imm_s;
				imm_sb_r <= imm_sb;
				imm_u_r <= imm_u;
				imm_uj_r <= imm_uj;
				reg_write_ctl_r <= reg_write_ctl;
				alu_src1_ctl_r <= alu_src1_ctl;
				alu_src2_ctl_r <= alu_src2_ctl;
				alu_op_ctl_r <= alu_op_ctl;
				jump_ctl_r <= jump_ctl;
				branch_ctl_r <= branch_ctl;
				mem_write_ctl_r <= mem_write_ctl;
				mem_read_ctl_r <= mem_read_ctl;
				sig_read_ctl_r <= sig_read_ctl;
				reg_to_mem_r <= reg_to_mem;
				mem_to_reg_r <= mem_to_reg;
			end
		end
	end

	//
	// EXECUTE STAGE
	//
	// 3rd stage (a) register file access (read)
	// the register file
	reg_bank register_bank
	(
		.clock(clock),
		.read_reg1(read_reg1),
		.read_reg2(read_reg2),
		.write_reg(write_reg),
		.wreg(wreg),
		.write_data(write_data),
		.read_data1(read_data1),
		.read_data2(read_data2)
	);

	// register file read/write selection and write enable
	assign read_reg1 = rs1_r[3:0];
	assign read_reg2 = rs2_r[3:0];
	assign write_reg = rd_r[3:0];
	assign wreg = (reg_write_ctl_r | mem_to_reg_r) &  ~mwait &  ~stall_reg;
	
	// 3rd stage (b) ALU operation
	alu alu
	(
		.op1(alu_src1),
		.op2(alu_src2),
		.alu_op(alu_op_ctl_r),
		.result(result),
		.zero(zero),
		.less_than(less_than)
	);

	assign alu_src1 = alu_src1_ctl_r == 1'b0 ? read_data1 : pc_last2;
	assign alu_src2 = alu_src2_ctl_r == 3'b000 ? {imm_u_r,12'h000} : alu_src2_ctl_r == 3'b001 ? 
		imm_i_r : alu_src2_ctl_r == 3'b010 ? imm_s_r : alu_src2_ctl_r == 3'b011 ? 
		pc : alu_src2_ctl_r == 3'b100 ? {24'h000000,3'b000,rs2_r} : read_data2;
	assign branch_src1 = jump_ctl_r == 2'b11 ? read_data1 : pc_last2;
	assign branch_src2 = jump_ctl_r == 2'b10 ? imm_uj_r : jump_ctl_r == 2'b11 ? imm_i_r : imm_sb_r;
	assign branch = branch_src1 + branch_src2;
	assign branch_taken = (zero == 1'b1 && branch_ctl_r == 3'b001) || 
		(zero == 1'b0 && branch_ctl_r == 3'b010) || (less_than == 1'b1 && branch_ctl_r == 3'b011) || 
		(less_than == 1'b0 && branch_ctl_r == 3'b100) || (less_than == 1'b1 && branch_ctl_r == 3'b101) || 
		(less_than == 1'b0 && branch_ctl_r == 3'b110) ? 1'b1 : 1'b0;
	assign except = branch_ctl_r == 3'b111 ? 1'b1 : 1'b0;
	assign jump_taken = jump_ctl_r != 2'b00 ? 1'b1 : 1'b0;
	assign inst_addr = pc;
	assign data_addr = result;
	assign data_b = mem_read_ctl_r == 2'b01 || mem_write_ctl_r == 2'b01 ? 1'b1 : 1'b0;
	assign data_h = mem_read_ctl_r == 2'b10 || mem_write_ctl_r == 2'b10 ? 1'b1 : 1'b0;
	assign data_access_s = reg_to_mem_r == 1'b1 || mem_to_reg_r == 1'b1 ? 1'b1 : 1'b0;
	assign data_access = data_access_s == 1'b1 && data_access_s_dly == 1'b0 ? 1'b1 : 1'b0;
  
	// 3rd stage (c) data memory / write back operation, register file access (write)
	// memory access, store operations
	always @(mem_write_ctl_r, result, read_data2) begin
		case (mem_write_ctl_r)
		2'b11 : begin
			// store word
			data_out <= {read_data2[7:0],read_data2[15:8],read_data2[23:16],read_data2[31:24]};
			data_w <= 4'b1111;
		end
		2'b01 : begin
			// store byte
			data_out <= {read_data2[7:0],read_data2[7:0],read_data2[7:0],read_data2[7:0]};
			case (result[1:0])
			2'b11 : begin data_w <= 4'b0001; end
			2'b10 : begin data_w <= 4'b0010; end
			2'b01 : begin data_w <= 4'b0100; end
			default : begin data_w <= 4'b1000; end
			endcase
		end
		2'b10 : begin
			// store half word
			data_out <= {read_data2[7:0],read_data2[15:8],read_data2[7:0],read_data2[15:8]};
			case (result[1])
			1'b1 : begin data_w <= 4'b0011; end
			default : begin data_w <= 4'b1100; end
			endcase
			end
		default : begin
			// WTF??
			data_out <= {read_data2[7:0],read_data2[15:8],read_data2[23:16],read_data2[31:24]};
			data_w <= 4'b0000;
		end
		endcase
	end

	// memory access, load operations
	always @(mem_read_ctl_r, result, data_in) begin
		case (mem_read_ctl_r)
		2'b01 : begin
			// load byte
			case (result[1:0])
			2'b11 : begin data_in_s <= {24'h000000,data_in[7:0]}; end
			2'b10 : begin data_in_s <= {24'h000000,data_in[15:8]}; end
			2'b01 : begin data_in_s <= {24'h000000,data_in[23:16]}; end
			default : begin data_in_s <= {24'h000000,data_in[31:24]}; end
			endcase
		end
		2'b10 : begin
			// load half word
			case (result[1])
			1'b1 : begin data_in_s <= {16'h0000,data_in[7:0],data_in[15:8]}; end
			default : begin data_in_s <= {16'h0000,data_in[23:16],data_in[31:24]}; end
			endcase
		end
		default : begin
			// load word
			data_in_s <= {data_in[7:0],data_in[15:8],data_in[23:16],data_in[31:24]};
		end
		endcase
	end

	// write back
	assign ext32b = (data_in_s[7] == 1'b0 || sig_read_ctl_r == 1'b0) ? 
		{24'h000000,data_in_s[7:0]} : {24'hffffff,data_in_s[7:0]};
	assign ext32h = (data_in_s[15] == 1'b0 || sig_read_ctl_r == 1'b0) ? 
		{16'h0000,data_in_s[15:0]} : {16'hffff,data_in_s[15:0]};
	assign write_data = mem_read_ctl_r == 2'b11 ? data_in_s : mem_read_ctl_r == 2'b01 ? 
		ext32b : mem_read_ctl_r == 2'b10 ? ext32h : jump_taken == 1'b1 ? 
		pc_last : result;

endmodule
