TOP = top_tb

SRC_BASE = ../..

VERILOG_SRC = \
	$(SRC_BASE)/riscv/core_rv32e_verilog/bshifter.v \
	$(SRC_BASE)/riscv/core_rv32e_verilog/alu.v \
	$(SRC_BASE)/riscv/core_rv32e_verilog/reg_bank.v \
	$(SRC_BASE)/riscv/core_rv32e_verilog/control.v \
	$(SRC_BASE)/riscv/core_rv32e_verilog/datapath.v \
	$(SRC_BASE)/riscv/core_rv32e_verilog/int_control.v \
	$(SRC_BASE)/riscv/core_rv32e_verilog/cpu.v \
	$(SRC_BASE)/devices/controllers/uart/uart.v \
	$(SRC_BASE)/devices/peripherals/basic_soc.v \
	$(SRC_BASE)/riscv/sim/boot_ram.v \
	$(SRC_BASE)/riscv/sim/ram.v \
	$(SRC_BASE)/riscv/sim/hf-riscv_basic_standard_soc_tb.v

PROG_SRC = \
	$(SRC_BASE)/riscv/sim/boot.txt \
	$(SRC_BASE)/software/code.txt
	
TIME = 1ms

all:

# Command line simulation using ModelSim
# run simulation with 'make modelsim-top TIME=10ms' to run for 10ms
modelsim:
	@ cp $(PROG_SRC) .
	vlib mylib
	vmap work ./mylib
	vlog -work mylib $(VERILOG_SRC)

modelsim-top: modelsim
	vsim $(GUI) mylib.$(TOP) -voptargs=+acc -c -do "vcd file ${TOP}.vcd -compress; vcd add /*; run ${TIME}; quit"
	
modelsim-all: modelsim
	vsim $(GUI) mylib.$(TOP) -voptargs=+acc -c -do "vcd file ${TOP}.vcd -compress; vcd add -r /*; run ${TIME}; quit"
	
modelsim-cpu: modelsim
	vsim $(GUI) mylib.$(TOP) -voptargs=+acc -c -do "vcd file ${TOP}.vcd -compress; vcd add -r /tb/processor/*; run ${TIME}; quit"

modelsim-wave: modelsim
	vsim $(GUI) mylib.$(TOP) -voptargs=+acc -do "add wave -r /*; run ${TIME};"

# Command line simulation using Icarus Verilog 
iverilog:
	@ cp $(PROG_SRC) .
	iverilog -g2005-sv $(VHD_SRC) $(VERILOG_SRC)
	
iverilog-vcd: iverilog
	vvp a.out -fst #-lxt

# Display waveforms
wave-vcd:
	@if [ -f "$(TOP).vcd" ]; then \
		gtkwave $(TOP).vcd --rcvar 'fontname_signals Monospace 12' --rcvar 'fontname_waves Monospace 12'; \
	else \
		gtkwave $(TOP).vcd.gz --rcvar 'fontname_signals Monospace 12' --rcvar 'fontname_waves Monospace 12'; \
	fi

# Clean all generated files
clean:
	@ rm -rf simu work mylib a.out
	@ rm -f $(TOP)
	@ rm -f $(TOP).vcd.gz $(TOP).vcd
	@ rm -f $(TOP).vcd $(TOP).ghw
	@ rm -f *.txt *.ini *.vsim transcript
