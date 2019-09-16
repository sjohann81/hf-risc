-- control signals for HF-RISCV
--
-- alu_op:			alu_src1:		mem_write:		jump:
-- 0000 -> and			0 -> r[rs1]		00 -> no mem write	00 -> no jump
-- 0001 -> or			1 -> pc_last2		01 -> sb		01 -> don't care
-- 0010 -> xor						10 -> sh		10 -> jal
-- 0011 -> don't care		alu_src2:		11 -> sw		11 -> jalr
-- 0100 -> add			000 -> imm_u
-- 0101 -> sub			001 -> imm_i		mem_read:		branch:
-- 0110 -> lui, jal, jalr	010 -> imm_s		00 -> no mem read	000 -> no branch
-- 0111 -> slt			011 -> pc		01 -> lb		001 -> beq
-- 1000 -> sltu			100 -> rs2		10 -> lh		010 -> bne
-- 1001 -> sll			101 -> r[rs2]		11 -> lw		011 -> blt
-- 1010 -> srl			110 -> don't care				100 -> bge
-- 1011 -> sra			111 -> don't care				101 -> bltu
-- 1100 -> mul									110 -> bgeu
-- 1101 -> mulh			reg_write:		sig_read:		111 -> system
-- 1110 -> mulhsu		0 -> no write		0 -> unsigned
-- 1111 -> mulhu		1 -> write register	1 -> signed

library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_unsigned.all;

entity control is
	port (	opcode:			in std_logic_vector(6 downto 0);
		funct3:			in std_logic_vector(2 downto 0);
		funct7:			in std_logic_vector(6 downto 0);
		reg_write:		out std_logic;
		alu_src1:		out std_logic;
		alu_src2:		out std_logic_vector(2 downto 0);
		alu_op:			out std_logic_vector(3 downto 0);
		jump:			out std_logic_vector(1 downto 0);
		branch:			out std_logic_vector(2 downto 0);
		mem_write:		out std_logic_vector(1 downto 0);
		mem_read:		out std_logic_vector(1 downto 0);
		sig_read:		out std_logic
	);
end control;

architecture arch_control of control is
begin
	process(opcode, funct3, funct7)
	begin
		case opcode is							-- load immediate / jumps
			when "0110111" =>					-- LUI
				reg_write <= '1';
				alu_src1 <= '0';
				alu_src2 <= "000";
				alu_op <= "0110";
				jump <= "00";
				branch <= "000";
				mem_write <= "00";
				mem_read <= "00";
				sig_read <= '0';
			when "0010111" =>					-- AUIPC
				reg_write <= '1';
				alu_src1 <= '1';
				alu_src2 <= "000";
				alu_op <= "0100";
				jump <= "00";
				branch <= "000";
				mem_write <= "00";
				mem_read <= "00";
				sig_read <= '0';
			when "1101111" =>					-- JAL
				reg_write <= '1';
				alu_src1 <= '0';
				alu_src2 <= "011";
				alu_op <= "0110";
				jump <= "10";
				branch <= "000";
				mem_write <= "00";
				mem_read <= "00";
				sig_read <= '0';
			when "1100111" =>					-- JALR
				reg_write <= '1';
				alu_src1 <= '0';
				alu_src2 <= "011";
				alu_op <= "0110";
				jump <= "11";
				branch <= "000";
				mem_write <= "00";
				mem_read <= "00";
				sig_read <= '0';
			when "1100011" =>					-- branches
				case funct3 is
					when "000" =>				-- BEQ
						reg_write <= '0';
						alu_src1 <= '0';
						alu_src2 <= "101";
						alu_op <= "0101";
						jump <= "00";
						branch <= "001";
						mem_write <= "00";
						mem_read <= "00";
						sig_read <= '0';
					when "001" =>				-- BNE
						reg_write <= '0';
						alu_src1 <= '0';
						alu_src2 <= "101";
						alu_op <= "0101";
						jump <= "00";
						branch <= "010";
						mem_write <= "00";
						mem_read <= "00";
						sig_read <= '0';
					when "100" =>				-- BLT
						reg_write <= '0';
						alu_src1 <= '0';
						alu_src2 <= "101";
						alu_op <= "0111";
						jump <= "00";
						branch <= "011";
						mem_write <= "00";
						mem_read <= "00";
						sig_read <= '0';
					when "101" =>				-- BGE
						reg_write <= '0';
						alu_src1 <= '0';
						alu_src2 <= "101";
						alu_op <= "0111";
						jump <= "00";
						branch <= "100";
						mem_write <= "00";
						mem_read <= "00";
						sig_read <= '0';
					when "110" =>				-- BLTU
						reg_write <= '0';
						alu_src1 <= '0';
						alu_src2 <= "101";
						alu_op <= "1000";
						jump <= "00";
						branch <= "101";
						mem_write <= "00";
						mem_read <= "00";
						sig_read <= '0';
					when "111" =>				-- BGEU
						reg_write <= '0';
						alu_src1 <= '0';
						alu_src2 <= "101";
						alu_op <= "1000";
						jump <= "00";
						branch <= "110";
						mem_write <= "00";
						mem_read <= "00";
						sig_read <= '0';
					when others =>
						reg_write <= '0';
						alu_src1 <= '0';
						alu_src2 <= "000";
						alu_op <= "0000";
						jump <= "00";
						branch <= "000";
						mem_write <= "00";
						mem_read <= "00";
						sig_read <= '0';
				end case;
			when "0000011" => 					-- loads
				case funct3 is
					when "000" =>				-- LB
						reg_write <= '1';
						alu_src1 <= '0';
						alu_src2 <= "001";
						alu_op <= "0100";
						jump <= "00";
						branch <= "000";
						mem_write <= "00";
						mem_read <= "01";
						sig_read <= '1';
					when "001" => 				-- LH
						reg_write <= '1';
						alu_src1 <= '0';
						alu_src2 <= "001";
						alu_op <= "0100";
						jump <= "00";
						branch <= "000";
						mem_write <= "00";
						mem_read <= "10";
						sig_read <= '1';
					when "010" =>				-- LW
						reg_write <= '1';
						alu_src1 <= '0';
						alu_src2 <= "001";
						alu_op <= "0100";
						jump <= "00";
						branch <= "000";
						mem_write <= "00";
						mem_read <= "11";
						sig_read <= '1';
					when "100" =>				-- LBU
						reg_write <= '1';
						alu_src1 <= '0';
						alu_src2 <= "001";
						alu_op <= "0100";
						jump <= "00";
						branch <= "000";
						mem_write <= "00";
						mem_read <= "01";
						sig_read <= '0';
					when "101" =>				-- LHU
						reg_write <= '1';
						alu_src1 <= '0';
						alu_src2 <= "001";
						alu_op <= "0100";
						jump <= "00";
						branch <= "000";
						mem_write <= "00";
						mem_read <= "10";
						sig_read <= '0';
					when others =>
						reg_write <= '0';
						alu_src1 <= '0';
						alu_src2 <= "000";
						alu_op <= "0000";
						jump <= "00";
						branch <= "000";
						mem_write <= "00";
						mem_read <= "00";
						sig_read <= '0';
				end case;
			when "0100011" =>					-- stores
				case funct3 is
					when "000" =>				-- SB
						reg_write <= '0';
						alu_src1 <= '0';
						alu_src2 <= "010";
						alu_op <= "0100";
						jump <= "00";
						branch <= "000";
						mem_write <= "01";
						mem_read <= "00";
						sig_read <= '0';
					when "001" =>				-- SH
						reg_write <= '0';
						alu_src1 <= '0';
						alu_src2 <= "010";
						alu_op <= "0100";
						jump <= "00";
						branch <= "000";
						mem_write <= "10";
						mem_read <= "00";
						sig_read <= '0';
					when "010" =>				-- SW
						reg_write <= '0';
						alu_src1 <= '0';
						alu_src2 <= "010";
						alu_op <= "0100";
						jump <= "00";
						branch <= "000";
						mem_write <= "11";
						mem_read <= "00";
						sig_read <= '0';
					when others =>
						reg_write <= '0';
						alu_src1 <= '0';
						alu_src2 <= "000";
						alu_op <= "0000";
						jump <= "00";
						branch <= "000";
						mem_write <= "00";
						mem_read <= "00";
						sig_read <= '0';
				end case;
			when "0010011" =>					-- imm computation
				case funct3 is
					when "000" =>				-- ADDI
						reg_write <= '1';
						alu_src1 <= '0';
						alu_src2 <= "001";
						alu_op <= "0100";
						jump <= "00";
						branch <= "000";
						mem_write <= "00";
						mem_read <= "00";
						sig_read <= '0';
					when "010" =>				-- SLTI
						reg_write <= '1';
						alu_src1 <= '0';
						alu_src2 <= "001";
						alu_op <= "0111";
						jump <= "00";
						branch <= "000";
						mem_write <= "00";
						mem_read <= "00";
						sig_read <= '0';
					when "011" =>				-- SLTIU
						reg_write <= '1';
						alu_src1 <= '0';
						alu_src2 <= "001";
						alu_op <= "1000";
						jump <= "00";
						branch <= "000";
						mem_write <= "00";
						mem_read <= "00";
						sig_read <= '0';
					when "100" =>				-- XORI
						reg_write <= '1';
						alu_src1 <= '0';
						alu_src2 <= "001";
						alu_op <= "0010";
						jump <= "00";
						branch <= "000";
						mem_write <= "00";
						mem_read <= "00";
						sig_read <= '0';
					when "110" =>				-- ORI
						reg_write <= '1';
						alu_src1 <= '0';
						alu_src2 <= "001";
						alu_op <= "0001";
						jump <= "00";
						branch <= "000";
						mem_write <= "00";
						mem_read <= "00";
						sig_read <= '0';
					when "111" =>				-- ANDI
						reg_write <= '1';
						alu_src1 <= '0';
						alu_src2 <= "001";
						alu_op <= "0000";
						jump <= "00";
						branch <= "000";
						mem_write <= "00";
						mem_read <= "00";
						sig_read <= '0';
					when "001" =>				-- SLLI
						reg_write <= '1';
						alu_src1 <= '0';
						alu_src2 <= "100";
						alu_op <= "1001";
						jump <= "00";
						branch <= "000";
						mem_write <= "00";
						mem_read <= "00";
						sig_read <= '0';
					when "101" =>
						case funct7 is
							when "0000000" =>	-- SRLI
								reg_write <= '1';
								alu_src1 <= '0';
								alu_src2 <= "100";
								alu_op <= "1010";
								jump <= "00";
								branch <= "000";
								mem_write <= "00";
								mem_read <= "00";
								sig_read <= '0';
							when "0100000" =>	-- SRAI
								reg_write <= '1';
								alu_src1 <= '0';
								alu_src2 <= "100";
								alu_op <= "1011";
								jump <= "00";
								branch <= "000";
								mem_write <= "00";
								mem_read <= "00";
								sig_read <= '0';
							when others =>
								reg_write <= '0';
								alu_src1 <= '0';
								alu_src2 <= "000";
								alu_op <= "0000";
								jump <= "00";
								branch <= "000";
								mem_write <= "00";
								mem_read <= "00";
								sig_read <= '0';
						end case;
					when others =>
						reg_write <= '0';
						alu_src1 <= '0';
						alu_src2 <= "000";
						alu_op <= "0000";
						jump <= "00";
						branch <= "000";
						mem_write <= "00";
						mem_read <= "00";
						sig_read <= '0';
				end case;
			when "0110011" =>					-- computation
				case funct3 is
					when "000" =>
						case funct7 is
							when "0000000" =>	-- ADD
								reg_write <= '1';
								alu_src1 <= '0';
								alu_src2 <= "101";
								alu_op <= "0100";
								jump <= "00";
								branch <= "000";
								mem_write <= "00";
								mem_read <= "00";
								sig_read <= '0';
							when "0100000" =>	-- SUB
								reg_write <= '1';
								alu_src1 <= '0';
								alu_src2 <= "101";
								alu_op <= "0101";
								jump <= "00";
								branch <= "000";
								mem_write <= "00";
								mem_read <= "00";
								sig_read <= '0';
							when "0000001" =>	-- MUL
								reg_write <= '1';
								alu_src1 <= '0';
								alu_src2 <= "101";
								alu_op <= "1100";
								jump <= "00";
								branch <= "000";
								mem_write <= "00";
								mem_read <= "00";
								sig_read <= '0';
							when others =>
								reg_write <= '0';
								alu_src1 <= '0';
								alu_src2 <= "000";
								alu_op <= "0000";
								jump <= "00";
								branch <= "000";
								mem_write <= "00";
								mem_read <= "00";
								sig_read <= '0';
						end case;
					when "001" =>
						case funct7 is
							when "0000001" =>	-- MULH
								reg_write <= '1';
								alu_src1 <= '0';
								alu_src2 <= "101";
								alu_op <= "1101";
								jump <= "00";
								branch <= "000";
								mem_write <= "00";
								mem_read <= "00";
								sig_read <= '0';
							when others =>		-- SLL
								reg_write <= '1';
								alu_src1 <= '0';
								alu_src2 <= "101";
								alu_op <= "1001";
								jump <= "00";
								branch <= "000";
								mem_write <= "00";
								mem_read <= "00";
								sig_read <= '0';
						end case;
					when "010" =>
						case funct7 is
							when "0000001" =>	-- MULHSU
								reg_write <= '1';
								alu_src1 <= '0';
								alu_src2 <= "101";
								alu_op <= "1110";
								jump <= "00";
								branch <= "000";
								mem_write <= "00";
								mem_read <= "00";
								sig_read <= '0';
							when others =>		-- SLT
								reg_write <= '1';
								alu_src1 <= '0';
								alu_src2 <= "101";
								alu_op <= "0111";
								jump <= "00";
								branch <= "000";
								mem_write <= "00";
								mem_read <= "00";
								sig_read <= '0';
						end case;
					when "011" =>
						case funct7 is
							when "0000001" =>	-- MULHU
								reg_write <= '1';
								alu_src1 <= '0';
								alu_src2 <= "101";
								alu_op <= "1111";
								jump <= "00";
								branch <= "000";
								mem_write <= "00";
								mem_read <= "00";
								sig_read <= '0';
							when others =>		-- SLTU
								reg_write <= '1';
								alu_src1 <= '0';
								alu_src2 <= "101";
								alu_op <= "1000";
								jump <= "00";
								branch <= "000";
								mem_write <= "00";
								mem_read <= "00";
								sig_read <= '0';
						end case;
					when "100" =>				-- XOR
						reg_write <= '1';
						alu_src1 <= '0';
						alu_src2 <= "101";
						alu_op <= "0010";
						jump <= "00";
						branch <= "000";
						mem_write <= "00";
						mem_read <= "00";
						sig_read <= '0';
					when "101" =>
						case funct7 is
							when "0000000" =>	-- SRL
								reg_write <= '1';
								alu_src1 <= '0';
								alu_src2 <= "101";
								alu_op <= "1010";
								jump <= "00";
								branch <= "000";
								mem_write <= "00";
								mem_read <= "00";
								sig_read <= '0';
							when "0100000" =>	-- SRA
								reg_write <= '1';
								alu_src1 <= '0';
								alu_src2 <= "101";
								alu_op <= "1011";
								jump <= "00";
								branch <= "000";
								mem_write <= "00";
								mem_read <= "00";
								sig_read <= '0';
							when others =>
								reg_write <= '0';
								alu_src1 <= '0';
								alu_src2 <= "000";
								alu_op <= "0000";
								jump <= "00";
								branch <= "000";
								mem_write <= "00";
								mem_read <= "00";
								sig_read <= '0';
						end case;
					when "110" =>				-- OR
						reg_write <= '1';
						alu_src1 <= '0';
						alu_src2 <= "101";
						alu_op <= "0001";
						jump <= "00";
						branch <= "000";
						mem_write <= "00";
						mem_read <= "00";
						sig_read <= '0';
					when "111" =>				-- AND
						reg_write <= '1';
						alu_src1 <= '0';
						alu_src2 <= "101";
						alu_op <= "0000";
						jump <= "00";
						branch <= "000";
						mem_write <= "00";
						mem_read <= "00";
						sig_read <= '0';
					when others =>
						reg_write <= '0';
						alu_src1 <= '0';
						alu_src2 <= "000";
						alu_op <= "0000";
						jump <= "00";
						branch <= "000";
						mem_write <= "00";
						mem_read <= "00";
						sig_read <= '0';
				end case;
			when "1110011" =>					-- SYSTEM
				reg_write <= '0';
				alu_src1 <= '0';
				alu_src2 <= "000";
				alu_op <= "0000";
				jump <= "00";
				branch <= "111";
				mem_write <= "00";
				mem_read <= "00";
				sig_read <= '0';
			when others =>
				reg_write <= '0';
				alu_src1 <= '0';
				alu_src2 <= "000";
				alu_op <= "0000";
				jump <= "00";
				branch <= "000";
				mem_write <= "00";
				mem_read <= "00";
				sig_read <= '0';
		end case;
	end process;
end arch_control;

