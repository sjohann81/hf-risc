# architecture: mips or riscv
ARCH=mips

# board specific parameters
BAUDRATE=57600
F_CLK=25000000
SERIAL_DEV=/dev/ttyUSB0

# compiler flags
CFLAGS_STRIP = -fdata-sections -ffunction-sections
LDFLAGS_STRIP = --gc-sections
LD_SCRIPT = hf-risc.ld

CFLAGS_NO_HW_MULDIV = -mnohwmult -mnohwdiv -ffixed-lo -ffixed-hi
GCC_mips = mips-elf-gcc -Wall -O2 -c -mips2 -mno-branch-likely -mpatfree -mfix-r4000 -mno-check-zero-division -msoft-float -ffreestanding -nostdlib -fomit-frame-pointer -G 0 -I ./include -DCPU_SPEED=${F_CLK} -DBIG_ENDIAN $(CFLAGS_NO_HW_MULDIV) $(CFLAGS_STRIP) -DDEBUG_PORT
AS_mips = mips-elf-as -mips1 -msoft-float
LD_mips = mips-elf-ld -mips1 $(LDFLAGS_STRIP)
DUMP_mips = mips-elf-objdump
READ_mips = mips-elf-readelf
OBJ_mips = mips-elf-objcopy
SIZE_mips = mips-elf-size

GCC_riscv = riscv32-unknown-elf-gcc -march=rv32i -mabi=ilp32 -Wall -Os -c -mstrict-align -ffreestanding -nostdlib -ffixed-s10 -ffixed-s11 -I ./include -DCPU_SPEED=${F_CLK} -DLITTLE_ENDIAN $(CFLAGS_STRIP) -DDEBUG_PORT #-ffixed-s10 -ffixed-s11 -mno-div -mrvc -fPIC -DDEBUG_PORT
AS_riscv = riscv32-unknown-elf-as -march=rv32i -mabi=ilp32 #-fPIC
LD_riscv = riscv32-unknown-elf-ld -melf32lriscv $(LDFLAGS_STRIP)
DUMP_riscv = riscv32-unknown-elf-objdump -Mno-aliases
READ_riscv = riscv32-unknown-elf-readelf
OBJ_riscv = riscv32-unknown-elf-objcopy
SIZE_riscv = riscv32-unknown-elf-size

all:

serial:
	stty ${BAUDRATE} raw cs8 -parenb -crtscts clocal cread ignpar ignbrk -ixon -ixoff -ixany -brkint -icrnl -imaxbel -opost -onlcr -isig -icanon -iexten -echo -echoe -echok -echoctl -echoke -F ${SERIAL_DEV}

load: serial
	cp code.bin ${SERIAL_DEV}

debug: serial
	cat ${SERIAL_DEV}

crt:
	$(AS_$(ARCH)) -o crt0.o lib/$(ARCH)/crt0.s
	$(GCC_$(ARCH)) -o libc.o lib/libc.c
	$(GCC_$(ARCH)) -o interrupt.o lib/interrupt.c
	$(GCC_$(ARCH)) -o math.o lib/math.c
	$(GCC_$(ARCH)) -o malloc.o lib/malloc.c

link:
	$(LD_$(ARCH)) -Tlib/$(ARCH)/$(LD_SCRIPT) -Map test.map -N -o test.elf *.o
	$(DUMP_$(ARCH)) --disassemble --reloc test.elf > test.lst
	$(DUMP_$(ARCH)) -h test.elf > test.sec
	$(DUMP_$(ARCH)) -s test.elf > test.cnt
	$(OBJ_$(ARCH)) -O binary test.elf test.bin
	$(SIZE_$(ARCH)) test.elf
	mv test.elf code.elf
	mv test.bin code.bin
	mv test.lst code.lst
	mv test.sec code.sec
	mv test.cnt code.cnt
	mv test.map code.map
	hexdump -v -e '4/1 "%02x" "\n"' code.bin > code.txt

monitor:
	$(AS_$(ARCH)) -o boot_rom.o lib/$(ARCH)/boot_rom.s
	$(GCC_$(ARCH)) -o libc.o lib/libc.c
	$(GCC_$(ARCH)) -o math.o lib/math.c
		$(GCC_$(ARCH)) -o monitor.o boot/monitor.c
	$(LD_$(ARCH)) -Tlib/$(ARCH)/hf-risc_bootloader.ld -Map test.map -N -o test.elf \
		boot_rom.o libc.o monitor.o
	$(DUMP_$(ARCH)) --disassemble --reloc test.elf > test.lst
	$(DUMP_$(ARCH)) -h test.elf > test.sec
	$(DUMP_$(ARCH)) -s test.elf > test.cnt
	$(OBJ_$(ARCH)) -O binary test.elf test.bin --pad-to 4096
	$(SIZE_$(ARCH)) test.elf
	mv test.elf code.elf
	mv test.bin code.bin
	mv test.lst code.lst
	mv test.sec code.sec
	mv test.cnt code.cnt
	mv test.map code.map
	hexdump -v -e '4/1 "%02x" "\n"' code.bin > boot.txt
	xxd -b -c 4 code.bin | awk '{print $$2 $$3 $$4 $$5}' > boot_bin.txt
	./../tools/xilinx/ram_image ./../tools/xilinx/ram_xilinx.vhd boot.txt boot_ram.vhd

boot_sim:
	$(AS_$(ARCH)) -o boot_rom.o lib/$(ARCH)/boot_rom.s
	$(GCC_$(ARCH)) -o libc.o lib/libc.c
	$(GCC_$(ARCH)) -o math.o lib/math.c
	$(GCC_$(ARCH)) -o bootloader_sim.o boot/bootloader_sim.c
	$(LD_$(ARCH)) -Tlib/$(ARCH)/hf-risc_bootloader.ld -Map test.map -N -o test.elf \
		boot_rom.o libc.o bootloader_sim.o
	$(DUMP_$(ARCH)) --disassemble --reloc test.elf > test.lst
	$(DUMP_$(ARCH)) -h test.elf > test.sec
	$(DUMP_$(ARCH)) -s test.elf > test.cnt
	$(OBJ_$(ARCH)) -O binary test.elf test.bin
	$(SIZE_$(ARCH)) test.elf
	mv test.elf code.elf
	mv test.bin code.bin
	mv test.lst code.lst
	mv test.sec code.sec
	mv test.cnt code.cnt
	mv test.map code.map
	hexdump -v -e '4/1 "%02x" "\n"' code.bin > boot.txt

eeprom_sim:
	echo "b16b00b5" > eeprom.txt
	printf "%08x\n" `stat -L -c %s code.txt` >> eeprom.txt
	cat code.txt >> eeprom.txt

ann: crt
	$(GCC_$(ARCH)) -o ann.o app/ann.c
	@$(MAKE) --no-print-directory link

ann2: crt
	$(GCC_$(ARCH)) -o ann2.o app/ann2.c
	@$(MAKE) --no-print-directory link

base64: crt
	$(GCC_$(ARCH)) -o base64.o app/base64.c
	@$(MAKE) --no-print-directory link

coremark: crt
	$(GCC_$(ARCH)) -o core_list_join.o app/coremark/core_list_join.c
	$(GCC_$(ARCH)) -o core_main.o app/coremark/core_main.c -DFLAGS_STR=\"'$(GCC_$(ARCH))'\" -Dee_printf=printf
	$(GCC_$(ARCH)) -o core_matrix.o app/coremark/core_matrix.c
	$(GCC_$(ARCH)) -o core_state.o app/coremark/core_state.c
	$(GCC_$(ARCH)) -o core_util.o app/coremark/core_util.c
	$(GCC_$(ARCH)) -o core_portme.o app/coremark/core_portme.c -Dee_printf=printf -DPERFORMANCE_RUN=1 -DITERATIONS=1000
	$(GCC_$(ARCH)) -o coremark.o app/coremark/coremark.c
	@$(MAKE) --no-print-directory link

core_test: crt
	$(GCC_$(ARCH)) -o core_test.o app/core_test.c
	@$(MAKE) --no-print-directory link

encode: crt
	$(GCC_$(ARCH)) -o encode.o app/encode.c
	@$(MAKE) --no-print-directory link

euler: crt
	$(GCC_$(ARCH)) -o euler.o app/euler.c
	@$(MAKE) --no-print-directory link

euler_fixed: crt
	$(GCC_$(ARCH)) -o euler.o app/euler_fixed.c
	@$(MAKE) --no-print-directory link

exp_io: crt
	$(GCC_$(ARCH)) -o spi.o lib/spi.c
	$(GCC_$(ARCH)) -o mcp23s17.o lib/mcp23s17.c
	$(GCC_$(ARCH)) -o exp_io.o app/exp_io.c
	@$(MAKE) --no-print-directory link

fib: crt
	$(GCC_$(ARCH)) -o fib.o app/fib.c
	@$(MAKE) --no-print-directory link

float_fixed: crt
	$(GCC_$(ARCH)) -o float_fixed.o app/float_fixed.c
	@$(MAKE) --no-print-directory link

hanoi: crt
	$(GCC_$(ARCH)) -o hanoi.o app/hanoi.c
	@$(MAKE) --no-print-directory link

hello: crt
	$(GCC_$(ARCH)) -o hello.o app/hello.c
	@$(MAKE) --no-print-directory link

interrupt_test: crt
	$(GCC_$(ARCH)) -o interrupt_test.o app/interrupt_test.c
	@$(MAKE) --no-print-directory link

labyrinth: crt
	$(GCC_$(ARCH)) -o labyrinth.o app/labyrinth.c
	@$(MAKE) --no-print-directory link

malloc_test: crt
	$(GCC_$(ARCH)) -o malloc_test.o app/malloc_test.c
	@$(MAKE) --no-print-directory link

mem_test: crt
	$(GCC_$(ARCH)) -o mem_test.o app/mem_test.c
	@$(MAKE) --no-print-directory link

newton: crt
	$(GCC_$(ARCH)) -o newton.o app/newton.c
	@$(MAKE) --no-print-directory link

peripherals_test: crt
	$(GCC_$(ARCH)) -o peripherals_test.o app/peripherals_test.c
	@$(MAKE) --no-print-directory link

pi: crt
	$(GCC_$(ARCH)) -o pi.o app/pi.c
	@$(MAKE) --no-print-directory link

sort: crt
	$(GCC_$(ARCH)) -o sort.o app/sort.c
	@$(MAKE) --no-print-directory link

sudoku: crt
	$(GCC_$(ARCH)) -o sudoku.o app/sudoku.c
	@$(MAKE) --no-print-directory link

test64: crt
	$(GCC_$(ARCH)) -o test64.o app/test64.c
	@$(MAKE) --no-print-directory link

test_crc: crt
	$(GCC_$(ARCH)) -o test_crc.o app/test_crc.c
	@$(MAKE) --no-print-directory link

test_fp: crt
	$(GCC_$(ARCH)) -o test_fp.o app/test_fp.c
	@$(MAKE) --no-print-directory link

test_fixed: crt
	$(GCC_$(ARCH)) -o test_fixed.o app/test_fixed.c
	@$(MAKE) --no-print-directory link

test_trig: crt
	$(GCC_$(ARCH)) -o test_trig.o app/test_trig.c
	@$(MAKE) --no-print-directory link

test_spi: crt
	$(GCC_$(ARCH)) -o spi.o lib/spi.c
	$(GCC_$(ARCH)) -o test_spi.o app/test_spi.c
	@$(MAKE) --no-print-directory link

xtea: crt
	$(GCC_$(ARCH)) -o xtea.o app/xtea.c
	@$(MAKE) --no-print-directory link

xtea2: crt
	$(GCC_$(ARCH)) -o xtea2.o app/xtea2.c
	@$(MAKE) --no-print-directory link

xtea_hw: crt
	$(GCC_$(ARCH)) -o xtea_hw.o app/xtea_hw.c
	@$(MAKE) --no-print-directory link

xtea_hw_vec: crt
	$(GCC_$(ARCH)) -o xtea_hw_vec.o app/xtea_hw_vec.c
	@$(MAKE) --no-print-directory link

aes_tiny: crt
	$(GCC_$(ARCH)) -o aes.o app/aes_tiny/aes.c
	$(GCC_$(ARCH)) -o test.o app/aes_tiny/test.c
	@$(MAKE) --no-print-directory link

aes_lite: crt
	$(GCC_$(ARCH)) -o aes.o app/aes_lite/aes.c
	@$(MAKE) --no-print-directory link

test: crt
	$(GCC_$(ARCH)) -o test.o app/test.c
	@$(MAKE) --no-print-directory link

include app/malardalen/malardalen.mak
include app/powerstone/powerstone.mak

clean:
	-rm -rf *~ *.o *.elf *.map *.lst *.sec *.cnt *.txt *.bin *.vhd
	-rm -rf app/*~
	-rm -rf boot/*~
	-rm -rf include/*~
	-rm -rf lib/*~
