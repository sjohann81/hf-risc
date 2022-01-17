library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_unsigned.all;
use ieee.std_logic_arith.all;

entity datapath is
	port (	clock:		in std_logic;
		reset:		in std_logic;

		stall:		in std_logic;
		mwait:		in std_logic;

		irq_vector:	in std_logic_vector(31 downto 0);
		irq:		in std_logic;
		irq_ack:	out std_logic;
		exception:	out std_logic;

		inst_addr:	out std_logic_vector(31 downto 0);
		inst_in:	in std_logic_vector(31 downto 0);

		data_addr:	out std_logic_vector(31 downto 0);
		data_in:	in std_logic_vector(31 downto 0);
		data_out:	out std_logic_vector(31 downto 0);
		data_w:		out std_logic_vector(3 downto 0);
		data_b:		out std_logic;
		data_h:		out std_logic;
		data_access:	out std_logic
	);
end datapath;

architecture arch_datapath of datapath is
-- datapath signals
	signal inst_in_s, data_in_s, pc, pc_last, pc_last2, pc_plus4, pc_next, result, branch, ext32b, ext32h, alu_src1, alu_src2: std_logic_vector(31 downto 0);
	signal ext32: std_logic_vector(31 downto 12);
	signal opcode, funct7: std_logic_vector(6 downto 0);
	signal funct3: std_logic_vector(2 downto 0);
	signal read_reg1, read_reg2, write_reg: std_logic_vector(4 downto 0);
	signal rs1, rs2, rd: std_logic_vector(4 downto 0);
	signal write_data, read_data1, read_data2: std_logic_vector(31 downto 0);
	signal imm_i, imm_s, imm_sb, imm_uj, branch_src1, branch_src2: std_logic_vector(31 downto 0);
	signal imm_u: std_logic_vector(31 downto 12);
	signal wreg, zero, less_than, branch_taken, jump_taken, stall_reg: std_logic;
	signal irq_ack_s, irq_ack_s_dly, bds, data_access_s, data_access_s_dly: std_logic;

-- control signals
	signal reg_write_ctl, alu_src1_ctl, sig_read_ctl, reg_to_mem, mem_to_reg, except: std_logic;
	signal jump_ctl, mem_write_ctl, mem_read_ctl: std_logic_vector(1 downto 0);
	signal alu_src2_ctl, branch_ctl: std_logic_vector(2 downto 0);
	signal alu_op_ctl: std_logic_vector(3 downto 0);

	signal rs1_r, rs2_r, rd_r: std_logic_vector(4 downto 0);
	signal imm_i_r, imm_s_r, imm_sb_r, imm_uj_r: std_logic_vector(31 downto 0);
	signal imm_u_r: std_logic_vector(31 downto 12);
	signal reg_write_ctl_r, alu_src1_ctl_r, sig_read_ctl_r, reg_to_mem_r, mem_to_reg_r: std_logic;
	signal jump_ctl_r, mem_write_ctl_r, mem_read_ctl_r: std_logic_vector(1 downto 0);
	signal alu_src2_ctl_r, branch_ctl_r: std_logic_vector(2 downto 0);
	signal alu_op_ctl_r: std_logic_vector(3 downto 0);
begin

--
-- FETCH STAGE
--
-- 1st stage, instruction memory access, PC update, interrupt acknowledge logic

	-- program counter logic
	process(clock, reset, reg_to_mem_r, mem_to_reg_r, stall, stall_reg)
	begin
		if reset = '1' then
			pc <= (others => '0');
			pc_last <= (others => '0');
			pc_last2 <= (others => '0');
		elsif clock'event and clock = '1' then
			if stall = '0' then
				pc <= pc_next;
				pc_last <= pc;
				pc_last2 <= pc_last;
			end if;
		end if;
	end process;

	pc_plus4 <=	pc + 4;

	pc_next <=	irq_vector when (irq = '1' and irq_ack_s = '1') or except = '1' else
			branch when branch_taken = '1' or jump_taken = '1' else
			pc_last when data_access_s = '1' else
			pc_plus4;

	-- interrupt acknowledge logic
	irq_ack_s <= '1' when irq = '1' and
		bds = '0' and branch_taken = '0' and jump_taken = '0' and
		reg_to_mem_r = '0' and mem_to_reg_r = '0' else '0';

	irq_ack <= irq_ack_s_dly;

	exception <= '1' when except = '1' else '0';

	process(clock, reset, irq, irq_ack_s, mem_to_reg_r, stall, mwait)
	begin
		if reset = '1' then
			irq_ack_s_dly <= '0';
			bds <= '0';
			data_access_s_dly <= '0';
			stall_reg <= '0';
		elsif clock'event and clock = '1' then
			stall_reg <= stall;
			if stall = '0' then
				data_access_s_dly <= data_access_s;
				if mwait = '0' then
					irq_ack_s_dly <= irq_ack_s;
					if branch_taken = '1' or jump_taken = '1' then
						bds <= '1';
					else
						bds <= '0';
					end if;
				end if;
			end if;
		end if;
	end process;

--
-- DECODE STAGE
--
-- 2nd stage, instruction decode, control unit operation, pipeline bubble insertion logic on load/store and branches

	-- pipeline bubble insertion on loads/stores, exceptions, branches and interrupts
	inst_in_s <= x"00000000" when data_access_s = '1' or except = '1' or
		branch_taken = '1' or jump_taken = '1' or bds = '1' or irq_ack_s = '1' else
		inst_in(7 downto 0) & inst_in(15 downto 8) & inst_in(23 downto 16) & inst_in(31 downto 24);

	-- instruction decode
	opcode <= inst_in_s(6 downto 0);
	funct3 <= inst_in_s(14 downto 12);
	funct7 <= inst_in_s(31 downto 25);
	rd <= inst_in_s(11 downto 7);
	rs1 <= inst_in_s(19 downto 15);
	rs2 <= inst_in_s(24 downto 20);
	imm_i <= ext32(31 downto 12) & inst_in_s(31 downto 20);
	imm_s <= ext32(31 downto 12) & inst_in_s(31 downto 25) & inst_in_s(11 downto 7);
	imm_sb <= ext32(31 downto 13) & inst_in_s(31) & inst_in_s(7) & inst_in_s(30 downto 25) & inst_in_s(11 downto 8) & '0';
	imm_u <= inst_in_s(31 downto 12);
	imm_uj <= ext32(31 downto 21) & inst_in_s(31) & inst_in_s(19 downto 12) & inst_in_s(20) & inst_in_s(30 downto 21) & '0';
	ext32 <= (others => '1') when inst_in_s(31) = '1' else (others => '0');

	-- control unit
	control_unit: entity work.control
	port map(	opcode => opcode,
			funct3 => funct3,
			funct7 => funct7,
			reg_write => reg_write_ctl,
			alu_src1 => alu_src1_ctl,
			alu_src2 => alu_src2_ctl,
			alu_op => alu_op_ctl,
			jump => jump_ctl,
			branch => branch_ctl,
			mem_write => mem_write_ctl,
			mem_read => mem_read_ctl,
			sig_read => sig_read_ctl
	);

	reg_to_mem <= '1' when mem_write_ctl /= "00" else '0';
	mem_to_reg <= '1' when mem_read_ctl /= "00" else '0';

	process(clock, reset, irq_ack_s, bds, stall, mwait)
	begin
		if reset = '1' then
			rd_r <= (others => '0');
			rs1_r <= (others => '0');
			rs2_r <= (others => '0');
			imm_i_r <= (others => '0');
			imm_s_r <= (others => '0');
			imm_sb_r <= (others => '0');
			imm_u_r <= (others => '0');
			imm_uj_r <= (others => '0');
			reg_write_ctl_r <= '0';
			alu_src1_ctl_r <= '0';
			alu_src2_ctl_r <= (others => '0');
			alu_op_ctl_r <= (others => '0');
			jump_ctl_r <= (others => '0');
			branch_ctl_r <= (others => '0');
			mem_write_ctl_r <= (others => '0');
			mem_read_ctl_r <= (others => '0');
			sig_read_ctl_r <= '0';
			reg_to_mem_r <= '0';
			mem_to_reg_r <= '0';
		elsif clock'event and clock = '1' then
			if stall = '0' and mwait = '0' then
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
			end if;
		end if;
	end process;

--
-- EXECUTE STAGE
--

-- 3rd stage (a) register file access (read)
	-- the register file
	register_bank: entity work.reg_bank
	port map(	clock => clock,
			read_reg1 => read_reg1,
			read_reg2 => read_reg2,
			write_reg => write_reg,
			wreg => wreg,
			write_data => write_data,
			read_data1 => read_data1,
			read_data2 => read_data2
	);

	-- register file read/write selection and write enable
	read_reg1 <= rs1_r;
	read_reg2 <= rs2_r;
	write_reg <= rd_r;
	wreg <= (reg_write_ctl_r or mem_to_reg_r) and not mwait and not stall_reg;

-- 3rd stage (b) ALU operation
	alu: entity work.alu
	port map(	op1 => alu_src1,
			op2 => alu_src2,
			alu_op => alu_op_ctl_r,
			result => result,
			zero => zero,
			less_than => less_than
	);

	alu_src1 <= read_data1 when alu_src1_ctl_r = '0' else pc_last2;
	alu_src2 <= 	imm_u_r & x"000" when alu_src2_ctl_r = "000" else
			imm_i_r when alu_src2_ctl_r = "001" else
			imm_s_r when alu_src2_ctl_r = "010" else
			pc when alu_src2_ctl_r = "011" else
			x"000000" & "000" & rs2_r when alu_src2_ctl_r = "100" else
			read_data2;

	branch_src1 <= read_data1 when jump_ctl_r = "11" else pc_last2;
	branch_src2 <= imm_uj_r when jump_ctl_r = "10" else
			imm_i_r when jump_ctl_r = "11" else imm_sb_r;

	branch <= branch_src1 + branch_src2;

	branch_taken <= '1' when (zero = '1' and branch_ctl_r = "001") or						-- BEQ
				(zero = '0' and branch_ctl_r = "010") or						-- BNE
				(less_than = '1' and branch_ctl_r = "011") or						-- BLT
				(less_than = '0' and branch_ctl_r = "100") or						-- BGE
				(less_than = '1' and branch_ctl_r = "101") or						-- BLTU
				(less_than = '0' and branch_ctl_r = "110")						-- BGEU
				else '0';
	except <= '1' when branch_ctl_r = "111" else '0';
	jump_taken <= '1' when jump_ctl_r /= "00" else '0';

	inst_addr <= pc;
	data_addr <= result;
	data_b <= '1' when mem_read_ctl_r = "01" or mem_write_ctl_r = "01" else '0';
	data_h <= '1' when mem_read_ctl_r = "10" or mem_write_ctl_r = "10" else '0';
	data_access_s <= '1' when reg_to_mem_r = '1' or mem_to_reg_r = '1' else '0';
	data_access <= '1' when data_access_s = '1' and data_access_s_dly = '0' else '0';


-- 3rd stage (c) data memory / write back operation, register file access (write)
	-- memory access, store operations
	process(mem_write_ctl_r, result, read_data2)
	begin
		case mem_write_ctl_r is
			when "11" =>			-- store word
				data_out <= read_data2(7 downto 0) & read_data2(15 downto 8) & read_data2(23 downto 16) & read_data2(31 downto 24);
				data_w <= "1111";
			when "01" =>			-- store byte
				data_out <= read_data2(7 downto 0) & read_data2(7 downto 0) & read_data2(7 downto 0) & read_data2(7 downto 0);
				case result(1 downto 0) is
					when "11" => data_w <= "0001";
					when "10" => data_w <= "0010";
					when "01" => data_w <= "0100";
					when others => data_w <= "1000";
				end case;
			when "10" =>			-- store half word
				data_out <= read_data2(7 downto 0) & read_data2(15 downto 8) & read_data2(7 downto 0) & read_data2(15 downto 8);
				case result(1) is
					when '1' => data_w <= "0011";
					when others => data_w <= "1100";
				end case;
			when others =>			-- WTF??
				data_out <= read_data2(7 downto 0) & read_data2(15 downto 8) & read_data2(23 downto 16) & read_data2(31 downto 24);
				data_w <= "0000";
		end case;
	end process;

	-- memory access, load operations
	process(mem_read_ctl_r, result, data_in)
	begin
		case mem_read_ctl_r is
			when "01" =>			-- load byte
				case result(1 downto 0) is
					when "11" => data_in_s <= x"000000" & data_in(7 downto 0);
					when "10" => data_in_s <= x"000000" & data_in(15 downto 8);
					when "01" => data_in_s <= x"000000" & data_in(23 downto 16);
					when others => data_in_s <= x"000000" & data_in(31 downto 24);

				end case;
			when "10" =>			-- load half word
				case result(1) is
					when '1' => data_in_s <= x"0000" & data_in(7 downto 0) & data_in(15 downto 8);
					when others => data_in_s <= x"0000" & data_in(23 downto 16) & data_in(31 downto 24);
				end case;
			when others =>			-- load word
				data_in_s <= data_in(7 downto 0) & data_in(15 downto 8) & data_in(23 downto 16) & data_in(31 downto 24);
		end case;
	end process;

	-- write back
	ext32b <= x"000000" & data_in_s(7 downto 0) when (data_in_s(7) = '0' or sig_read_ctl_r = '0') else x"ffffff" & data_in_s(7 downto 0);
	ext32h <= x"0000" & data_in_s(15 downto 0) when (data_in_s(15) = '0' or sig_read_ctl_r = '0') else x"ffff" & data_in_s(15 downto 0);

	write_data <= data_in_s when mem_read_ctl_r = "11" else
			ext32b when mem_read_ctl_r = "01" else
			ext32h when mem_read_ctl_r = "10" else
			pc_last when jump_taken = '1' else result;

end arch_datapath;

