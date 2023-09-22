# to install ghdl and gtkwave:
#
# sudo apt install gnat
# git clone https://github.com/ghdl/ghdl.git
# cd ghdl-master
# ./configure --prefix=/usr/local
# make
# sudo make install
# sudo apt install gtkwave

TOP = tb

VHDL_BASE = ../..
SRC_BASE = ../..

VHD_SRC = \
	$(VHDL_BASE)/mips/core_mips/bshifter.vhd \
	$(VHDL_BASE)/mips/core_mips/alu.vhd \
	$(VHDL_BASE)/mips/core_mips/reg_bank.vhd \
	$(VHDL_BASE)/mips/core_mips/control.vhd \
	$(VHDL_BASE)/mips/core_mips/datapath.vhd \
	$(VHDL_BASE)/mips/core_mips/int_control.vhd \
	$(VHDL_BASE)/mips/core_mips/cpu.vhd \
	$(VHDL_BASE)/devices/controllers/uart/uart.vhd \
	$(VHDL_BASE)/devices/peripherals/basic_soc.vhd \
	$(VHDL_BASE)/mips/sim/boot_ram.vhd \
	$(VHDL_BASE)/mips/sim/ram.vhd \
	$(VHDL_BASE)/mips/sim/hf-risc_basic_standard_soc_tb.vhd

PROG_SRC = \
	$(SRC_BASE)/mips/sim/boot.txt \
	$(SRC_BASE)/software/code.txt

TIME = 1ms

all:

# Command line simulation using ModelSim
# run simulation with 'make modelsim-top TIME=10ms' to run for 10ms
modelsim:
	@ cp $(PROG_SRC) .
	vlib mylib
	vmap work ./mylib
	vcom -work mylib $(VHD_SRC)

modelsim-top: modelsim
	vsim $(GUI) mylib.$(TOP) -c -do "vcd file ${TOP}.vcd -compress; vcd add /*; run ${TIME}; quit"
	
modelsim-all: modelsim
	vsim $(GUI) mylib.$(TOP) -c -do "vcd file ${TOP}.vcd -compress; vcd add -r /*; run ${TIME}; quit"
	
modelsim-cpu: modelsim
	vsim $(GUI) mylib.$(TOP) -c -do "vcd file ${TOP}.vcd -compress; vcd add -r /tb/processor/*; run ${TIME}; quit"

modelsim-wave: modelsim
	vsim $(GUI) mylib.$(TOP) -do "add wave -r /*; run ${TIME};"

# Command line simulation using the free ghdl
# run simulation with 'make ghdl-vcd TIME=10ms' to run for 10ms
ghdl:
	@ cp $(PROG_SRC) .
	@ mkdir -p simu
	ghdl -a --ieee=synopsys --warn-no-vital-generic --workdir=simu --work=work -fexplicit $(VHD_SRC)
	
ghdl-vcd: ghdl
	ghdl -r --ieee=synopsys --warn-no-vital-generic --workdir=simu --work=work -fexplicit $(TOP) --assert-level=failure --stop-time=$(TIME) --vcdgz=$(TOP).vcd.gz

ghdl-ghw: ghdl
	ghdl -r --ieee=synopsys --warn-no-vital-generic --workdir=simu --work=work -fexplicit $(TOP) --assert-level=failure --stop-time=$(TIME) --wave=$(TOP).ghw


# Display waveforms
wave-vcd:
	gtkwave $(TOP).vcd.gz
	
wave-ghw:
	gtkwave $(TOP).ghw


# Clean all generated files
clean:
	@ rm -rf simu work mylib
	@ rm -f $(TOP)
	@ rm -f $(TOP).vcd.gz
	@ rm -f $(TOP).vcd $(TOP).ghw
	@ rm -f *.txt *.ini *.vsim transcript
