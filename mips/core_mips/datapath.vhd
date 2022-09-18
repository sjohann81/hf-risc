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
	signal inst_in_s, data_in_s, pc, pc_last, pc_plus4, pc_next, result, branch, jump, ext32, ext32b, ext32h, alu_src: std_logic_vector(31 downto 0);
	signal opcode, funct: std_logic_vector(5 downto 0);
	signal read_reg1, read_reg2, write_reg: std_logic_vector(4 downto 0);
	signal rs, rt, rd, target: std_logic_vector(4 downto 0);
	signal write_data, read_data1, read_data2: std_logic_vector(31 downto 0);
	signal imm: std_logic_vector(15 downto 0);
	signal wreg, zero, less_than, br_link_ctl, branch_taken, jump_taken, stall_reg: std_logic;
	signal irq_ack_s, irq_ack_s_dly, bds, data_access_s, data_access_s_dly: std_logic;

-- control signals
	signal reg_dst_ctl, reg_write_ctl, alu_src_ctl, reg_to_mem_ctl, mem_to_reg_ctl, signed_imm_ctl, signed_rd_ctl, shift_ctl: std_logic;
	signal jump_ctl, mem_read_ctl, mem_write_ctl: std_logic_vector(1 downto 0);
	signal branch_ctl: std_logic_vector(2 downto 0);
	signal alu_op_ctl: std_logic_vector(3 downto 0);


	signal reg_dst_ctl_r, reg_write_ctl_r, alu_src_ctl_r, reg_to_mem_ctl_r, mem_to_reg_ctl_r, signed_imm_ctl_r, signed_rd_ctl_r, shift_ctl_r, br_link_ctl_r: std_logic;
	signal jump_ctl_r, mem_read_ctl_r, mem_write_ctl_r: std_logic_vector(1 downto 0);
	signal branch_ctl_r: std_logic_vector(2 downto 0);
	signal alu_op_ctl_r: std_logic_vector(3 downto 0);
	signal rs_r, rt_r, rd_r: std_logic_vector(4 downto 0);
	signal imm_r: std_logic_vector(15 downto 0);
begin

--
-- FETCH STAGE
--
-- 1st stage, instruction memory access, PC update, interrupt acknowledge logic

	-- program counter logic
	process(clock, reset, stall_reg)
	begin
		if reset = '1' then
			pc <= (others => '0');
			pc_last <= (others => '0');
		elsif clock'event and clock = '1' then
			if stall = '0' then
				pc <= pc_next;
				pc_last <= pc;
			end if;
		end if;
	end process;

	pc_plus4 <=	pc + 4;

	pc_next <=	irq_vector when (irq = '1' and irq_ack_s = '1') else
			branch when branch_taken = '1' else
			jump when jump_taken = '1' else
			pc_last when data_access_s = '1' else
			pc_plus4;

	-- interrupt acknowledge logic
	irq_ack_s <= '1' when irq = '1' and
		bds = '0' and branch_taken = '0' and jump_taken = '0' and
		reg_to_mem_ctl_r = '0' and mem_to_reg_ctl_r = '0' else '0';

	irq_ack <= irq_ack_s_dly;
	
	exception <= '0';

	process(clock, reset, irq, irq_ack_s, mem_to_reg_ctl_r, stall, mwait)
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
-- 2nd stage, instruction decode, control unit operation, pipeline bubble insertion logic on load/store and 2nd branch delay slot

	-- pipeline bubble insertion on loads/stores, branches and interrupts
	inst_in_s <= x"00000000" when data_access_s = '1' or bds = '1' or irq_ack_s = '1' else inst_in;

	-- instruction decode
	opcode <= inst_in_s(31 downto 26);
	rs <= inst_in_s(25 downto 21);
	rt <= inst_in_s(20 downto 16);
	rd <= "11111" when br_link_ctl = '1' else inst_in_s(15 downto 11);					-- FIXME: this will not work for the 'jalr rd, rs' format
	funct <= inst_in_s(5 downto 0);
	imm <= inst_in_s(15 downto 0);

	-- control unit
	control_unit: entity work.control
	port map(	opcode => opcode,
			funct => funct,
			rtx => rt,
			reg_dst => reg_dst_ctl,
			reg_write => reg_write_ctl,
			alu_src => alu_src_ctl,
			alu_op => alu_op_ctl,
			jump => jump_ctl,
			branch => branch_ctl,
			br_link => br_link_ctl,
			reg_to_mem => reg_to_mem_ctl,
			mem_to_reg => mem_to_reg_ctl,
			signed_imm => signed_imm_ctl,
			mem_write => mem_write_ctl,
			mem_read => mem_read_ctl,
			signed_rd => signed_rd_ctl,
			shift => shift_ctl
	);

	process(clock, reset, stall, mwait)
	begin
		if reset = '1' then
			rs_r <= (others => '0');
			rt_r <= (others => '0');
			rd_r <= (others => '0');
			imm_r <= (others => '0');
			reg_dst_ctl_r <= '0';
			reg_write_ctl_r <= '0';
			alu_src_ctl_r <= '0';
			alu_op_ctl_r <= (others => '0');
			jump_ctl_r <= (others => '0');
			branch_ctl_r <= (others => '0');
			br_link_ctl_r <= '0';
			reg_to_mem_ctl_r <= '0';
			mem_to_reg_ctl_r <= '0';
			signed_imm_ctl_r <= '0';
			mem_write_ctl_r <= "00";
			mem_read_ctl_r <= "00";
			signed_rd_ctl_r <= '0';
			shift_ctl_r <= '0';
		elsif clock'event and clock = '1' then
			if stall = '0' and mwait = '0' then
				rs_r <= rs;
				rt_r <= rt;
				rd_r <= rd;
				imm_r <= imm;
				reg_dst_ctl_r <= reg_dst_ctl;
				reg_write_ctl_r <= reg_write_ctl;
				alu_src_ctl_r <= alu_src_ctl;
				alu_op_ctl_r <= alu_op_ctl;
				jump_ctl_r <= jump_ctl;
				branch_ctl_r <= branch_ctl;
				br_link_ctl_r <= br_link_ctl;
				reg_to_mem_ctl_r <= reg_to_mem_ctl;
				mem_to_reg_ctl_r <= mem_to_reg_ctl;
				signed_imm_ctl_r <= signed_imm_ctl;
				mem_write_ctl_r <= mem_write_ctl;
				mem_read_ctl_r <= mem_read_ctl;
				signed_rd_ctl_r <= signed_rd_ctl;
				shift_ctl_r <= shift_ctl;
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
	read_reg1 <= rs_r when shift_ctl_r = '0' else rt_r;						-- source for shifts or normal operations
	read_reg2 <= "00000" when branch_ctl_r > "010" else						-- source for branch and link (for zero operations)
			rs_r when shift_ctl_r = '1' else rt_r;						-- source for register based shifts or normal operations
	write_reg <= target when mem_to_reg_ctl_r = '0' else rt_r;
	ext32 <= x"0000" & imm_r when (imm_r(15) = '0' or signed_imm_ctl_r = '0') else x"ffff" & imm_r;
	target <= rt_r when reg_dst_ctl_r = '0' else rd_r;						-- target register selection
	wreg <= (reg_write_ctl_r or mem_to_reg_ctl_r) and not mwait and not stall_reg;			-- enable the register bank for write back also

-- 3rd stage (b) ALU operation
	alu: entity work.alu
	port map(	op1 => read_data1,
			op2 => alu_src,
			alu_op => alu_op_ctl_r,
			result => result,
			zero => zero,
			less_than => less_than
	);

	alu_src <= read_data2 when alu_src_ctl_r = '0' else ext32;

	branch <= (ext32(29 downto 0) & "00") + pc_last;
	jump <= read_data1 when jump_ctl_r = "10" else pc_last(31 downto 28) & rs_r & rt_r & imm_r & "00";

	branch_taken <= '1' when (zero = '1' and branch_ctl_r = "001") or						-- BEQ
				(zero = '0' and branch_ctl_r = "010") or						-- BNE
				((zero = '1' or less_than = '1') and branch_ctl_r = "011") or				-- BLEZ
				((zero = '0' and less_than = '0') and branch_ctl_r = "100") or				-- BGTZ
				((zero = '0' and less_than = '1') and branch_ctl_r = "101") or				-- BLTZ, BLTZAL
				((zero = '1' or less_than = '0') and branch_ctl_r = "110")				-- BGEZ, BGEZAL
				else '0';
	jump_taken <= '1' when jump_ctl_r /= "00" else '0';								-- J, JAL, JR, JALR

	inst_addr <= pc;
	data_addr <= result;
	data_b <= '1' when mem_read_ctl_r = "01" or mem_write_ctl_r = "01" else '0';
	data_h <= '1' when mem_read_ctl_r = "10" or mem_write_ctl_r = "10" else '0';
	data_access_s <= '1' when reg_to_mem_ctl_r = '1' or mem_to_reg_ctl_r = '1' else '0';
	data_access <= '1' when data_access_s = '1' and data_access_s_dly = '0' else '0';


-- 3rd stage (c) data memory / write back operation, register file access (write)
	-- memory access, store operations
	process(mem_write_ctl_r, result, read_data2)
	begin
		case mem_write_ctl_r is
			when "11" =>			-- store word
				data_out <= read_data2;
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
				data_out <= read_data2(15 downto 0) & read_data2(15 downto 0);
				case result(1) is
					when '1' => data_w <= "0011";
					when others => data_w <= "1100";
				end case;
			when others =>			-- WTF??
				data_out <= read_data2;
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
					when '1' => data_in_s <= x"0000" & data_in(15 downto 0);
					when others => data_in_s <= x"0000" & data_in(31 downto 16);
				end case;
			when others =>			-- load word
				data_in_s <= data_in;
		end case;
	end process;

	-- write back
	ext32b <= x"000000" & data_in_s(7 downto 0) when (data_in_s(7) = '0' or signed_rd_ctl_r = '0') else x"ffffff" & data_in_s(7 downto 0);
	ext32h <= x"0000" & data_in_s(15 downto 0) when (data_in_s(15) = '0' or signed_rd_ctl_r = '0') else x"ffff" & data_in_s(15 downto 0);

	write_data <= data_in_s when mem_read_ctl_r = "11" else
			ext32b when mem_read_ctl_r = "01" else
			ext32h when mem_read_ctl_r = "10" else
			pc when br_link_ctl_r = '1' else result;

end arch_datapath;

