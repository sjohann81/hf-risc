# architecture: mips or riscv
ARCH=riscv

# board specific parameters
BAUDRATE=57600
F_CLK=25000000
SERIAL_DEV=/dev/ttyUSB0

# compiler flags
CFLAGS_STRIP = -fdata-sections -ffunction-sections
LDFLAGS_STRIP = --gc-sections

CFLAGS_NO_HW_MULDIV = -mnohwmult -mnohwdiv -ffixed-lo -ffixed-hi
GCC_mips = mips-elf-gcc -O2 -c -mips1 -mpatfree -mfix-r4000 -mno-check-zero-division -msoft-float -fshort-double -ffreestanding -nostdlib -fomit-frame-pointer -G 0 -I ./include -DCPU_SPEED=${F_CLK} -DBIG_ENDIAN $(CFLAGS_STRIP) $(CFLAGS_NO_HW_MULDIV) # -DDEBUG_PORT

AS_mips = mips-elf-as -mips1 -msoft-float
LD_mips = mips-elf-ld -mips1 $(LDFLAGS_STRIP)
DUMP_mips = mips-elf-objdump
READ_mips = mips-elf-readelf
OBJ_mips = mips-elf-objcopy
SIZE_mips = mips-elf-size

GCC_riscv = riscv64-unknown-elf-gcc -march=RV32I -O2 -c -msoft-float -fshort-double -ffreestanding -nostdlib -ffixed-s10 -ffixed-s11 -I ./include -DCPU_SPEED=${F_CLK} -DLITTLE_ENDIAN $(CFLAGS_STRIP) #-ffixed-s10 -ffixed-s11 -mrvc -fPIC -DDEBUG_PORT
AS_riscv = riscv64-unknown-elf-as -m32 -msoft-float #-fPIC
LD_riscv = riscv64-unknown-elf-ld -melf32lriscv $(LDFLAGS_STRIP)
DUMP_riscv = riscv64-unknown-elf-objdump -Mno-aliases
READ_riscv = riscv64-unknown-elf-readelf
OBJ_riscv = riscv64-unknown-elf-objcopy
SIZE_riscv = riscv64-unknown-elf-size

all:

serial:
	stty ${BAUDRATE} raw cs8 -parenb -crtscts clocal cread ignpar ignbrk -ixon -ixoff -ixany -brkint -icrnl -imaxbel -opost -onlcr -isig -icanon -iexten -echo -echoe -echok -echoctl -echoke -F ${SERIAL_DEV}

load: serial
	cp code.bin ${SERIAL_DEV}

debug: serial
	cat ${SERIAL_DEV}
	
monitor:
	$(AS_$(ARCH)) -o boot_rom.o lib/$(ARCH)/boot_rom.s
	$(GCC_$(ARCH)) -o libc.o lib/libc.c
	$(GCC_$(ARCH)) -o fp-math.o lib/fp-math.c
	$(GCC_$(ARCH)) -o math.o lib/math.c
	$(GCC_$(ARCH)) -o monitor.o boot/monitor.c
	$(LD_$(ARCH)) -Tlib/$(ARCH)/hf-risc_bootloader.ld -Map test.map -N -o test.axf \
		boot_rom.o libc.o monitor.o
	$(DUMP_$(ARCH)) --disassemble --reloc test.axf > test.lst
	$(DUMP_$(ARCH)) -h test.axf > test.sec
	$(DUMP_$(ARCH)) -s test.axf > test.cnt
	$(OBJ_$(ARCH)) -O binary test.axf test.bin --pad-to 4096
	$(SIZE_$(ARCH)) test.axf
	mv test.axf code.axf
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
	$(GCC_$(ARCH)) -o fp-math.o lib/fp-math.c
	$(GCC_$(ARCH)) -o math.o lib/math.c
	$(GCC_$(ARCH)) -o bootloader_sim.o boot/bootloader_sim.c
	$(LD_$(ARCH)) -Tlib/$(ARCH)/hf-risc_bootloader.ld -Map test.map -N -o test.axf \
		boot_rom.o libc.o bootloader_sim.o
	$(DUMP_$(ARCH)) --disassemble --reloc test.axf > test.lst
	$(DUMP_$(ARCH)) -h test.axf > test.sec
	$(DUMP_$(ARCH)) -s test.axf > test.cnt
	$(OBJ_$(ARCH)) -O binary test.axf test.bin
	$(SIZE_$(ARCH)) test.axf
	mv test.axf code.axf
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

core_test:
	$(AS_$(ARCH)) -o crt0.o lib/$(ARCH)/crt0.s
	$(GCC_$(ARCH)) -o libc.o lib/libc.c
	$(GCC_$(ARCH)) -o fp-math.o lib/fp-math.c
	$(GCC_$(ARCH)) -o math.o lib/math.c
	$(GCC_$(ARCH)) -o core_test.o app/core_test.c
	$(LD_$(ARCH)) -Tlib/$(ARCH)/hf-risc.ld -Map test.map -N -o test.axf *.o
	$(DUMP_$(ARCH)) --disassemble --reloc test.axf > test.lst
	$(DUMP_$(ARCH)) -h test.axf > test.sec
	$(DUMP_$(ARCH)) -s test.axf > test.cnt
	$(OBJ_$(ARCH)) -O binary test.axf test.bin
	$(SIZE_$(ARCH)) test.axf
	mv test.axf code.axf
	mv test.bin code.bin
	mv test.lst code.lst
	mv test.sec code.sec
	mv test.cnt code.cnt
	mv test.map code.map
	hexdump -v -e '4/1 "%02x" "\n"' code.bin > code.txt

euler:
	$(AS_$(ARCH)) -o crt0.o lib/$(ARCH)/crt0.s
	$(GCC_$(ARCH)) -o libc.o lib/libc.c
	$(GCC_$(ARCH)) -o fp-math.o lib/fp-math.c
	$(GCC_$(ARCH)) -o math.o lib/math.c
	$(GCC_$(ARCH)) -o euler.o app/euler.c
	$(LD_$(ARCH)) -Tlib/$(ARCH)/hf-risc.ld -Map test.map -N -o test.axf *.o
	$(DUMP_$(ARCH)) --disassemble --reloc test.axf > test.lst
	$(DUMP_$(ARCH)) -h test.axf > test.sec
	$(DUMP_$(ARCH)) -s test.axf > test.cnt
	$(OBJ_$(ARCH)) -O binary test.axf test.bin
	$(SIZE_$(ARCH)) test.axf
	mv test.axf code.axf
	mv test.bin code.bin
	mv test.lst code.lst
	mv test.sec code.sec
	mv test.cnt code.cnt
	mv test.map code.map
	hexdump -v -e '4/1 "%02x" "\n"' code.bin > code.txt

exp_io:
	$(AS_$(ARCH)) -o crt0.o lib/$(ARCH)/crt0.s
	$(GCC_$(ARCH)) -o libc.o lib/libc.c
	$(GCC_$(ARCH)) -o fp-math.o lib/fp-math.c
	$(GCC_$(ARCH)) -o math.o lib/math.c
	$(GCC_$(ARCH)) -o spi.o lib/spi.c
	$(GCC_$(ARCH)) -o mcp23s17.o lib/mcp23s17.c
	$(GCC_$(ARCH)) -o exp_io.o app/exp_io.c
	$(LD_$(ARCH)) -Tlib/$(ARCH)/hf-risc.ld -Map test.map -N -o test.axf *.o
	$(DUMP_$(ARCH)) --disassemble --reloc test.axf > test.lst
	$(DUMP_$(ARCH)) -h test.axf > test.sec
	$(DUMP_$(ARCH)) -s test.axf > test.cnt
	$(OBJ_$(ARCH)) -O binary test.axf test.bin
	$(SIZE_$(ARCH)) test.axf
	mv test.axf code.axf
	mv test.bin code.bin
	mv test.lst code.lst
	mv test.sec code.sec
	mv test.cnt code.cnt
	mv test.map code.map
	hexdump -v -e '4/1 "%02x" "\n"' code.bin > code.txt

fib:
	$(AS_$(ARCH)) -o crt0.o lib/$(ARCH)/crt0.s
	$(GCC_$(ARCH)) -o libc.o lib/libc.c
	$(GCC_$(ARCH)) -o fp-math.o lib/fp-math.c
	$(GCC_$(ARCH)) -o math.o lib/math.c
	$(GCC_$(ARCH)) -o fib.o app/fib.c
	$(LD_$(ARCH)) -Tlib/$(ARCH)/hf-risc.ld -Map test.map -N -o test.axf *.o
	$(DUMP_$(ARCH)) --disassemble --reloc test.axf > test.lst
	$(DUMP_$(ARCH)) -h test.axf > test.sec
	$(DUMP_$(ARCH)) -s test.axf > test.cnt
	$(OBJ_$(ARCH)) -O binary test.axf test.bin
	$(SIZE_$(ARCH)) test.axf
	mv test.axf code.axf
	mv test.bin code.bin
	mv test.lst code.lst
	mv test.sec code.sec
	mv test.cnt code.cnt
	mv test.map code.map
	hexdump -v -e '4/1 "%02x" "\n"' code.bin > code.txt

hanoi:
	$(AS_$(ARCH)) -o crt0.o lib/$(ARCH)/crt0.s
	$(GCC_$(ARCH)) -o libc.o lib/libc.c
	$(GCC_$(ARCH)) -o fp-math.o lib/fp-math.c
	$(GCC_$(ARCH)) -o math.o lib/math.c
	$(GCC_$(ARCH)) -o hanoi.o app/hanoi.c
	$(LD_$(ARCH)) -Tlib/$(ARCH)/hf-risc.ld -Map test.map -N -o test.axf *.o
	$(DUMP_$(ARCH)) --disassemble --reloc test.axf > test.lst
	$(DUMP_$(ARCH)) -h test.axf > test.sec
	$(DUMP_$(ARCH)) -s test.axf > test.cnt
	$(OBJ_$(ARCH)) -O binary test.axf test.bin
	$(SIZE_$(ARCH)) test.axf
	mv test.axf code.axf
	mv test.bin code.bin
	mv test.lst code.lst
	mv test.sec code.sec
	mv test.cnt code.cnt
	mv test.map code.map
	hexdump -v -e '4/1 "%02x" "\n"' code.bin > code.txt

hello:
	$(AS_$(ARCH)) -o crt0.o lib/$(ARCH)/crt0.s
	$(GCC_$(ARCH)) -o libc.o lib/libc.c
	$(GCC_$(ARCH)) -o fp-math.o lib/fp-math.c
	$(GCC_$(ARCH)) -o math.o lib/math.c
	$(GCC_$(ARCH)) -o hello.o app/hello.c
	$(LD_$(ARCH)) -Tlib/$(ARCH)/hf-risc.ld -Map test.map -N -o test.axf *.o
	$(DUMP_$(ARCH)) --disassemble --reloc test.axf > test.lst
	$(DUMP_$(ARCH)) -h test.axf > test.sec
	$(DUMP_$(ARCH)) -s test.axf > test.cnt
	$(OBJ_$(ARCH)) -O binary test.axf test.bin
	$(SIZE_$(ARCH)) test.axf
	mv test.axf code.axf
	mv test.bin code.bin
	mv test.lst code.lst
	mv test.sec code.sec
	mv test.cnt code.cnt
	mv test.map code.map
	hexdump -v -e '4/1 "%02x" "\n"' code.bin > code.txt

interrupt_test:
	$(AS_$(ARCH)) -o crt0.o lib/$(ARCH)/crt0.s
	$(GCC_$(ARCH)) -o libc.o lib/libc.c
	$(GCC_$(ARCH)) -o fp-math.o lib/fp-math.c
	$(GCC_$(ARCH)) -o math.o lib/math.c
	$(GCC_$(ARCH)) -o interrupt_test.o app/interrupt_test.c
	$(LD_$(ARCH)) -Tlib/$(ARCH)/hf-risc.ld -Map test.map -N -o test.axf *.o
	$(DUMP_$(ARCH)) --disassemble --reloc test.axf > test.lst
	$(DUMP_$(ARCH)) -h test.axf > test.sec
	$(DUMP_$(ARCH)) -s test.axf > test.cnt
	$(OBJ_$(ARCH)) -O binary test.axf test.bin
	$(SIZE_$(ARCH)) test.axf
	mv test.axf code.axf
	mv test.bin code.bin
	mv test.lst code.lst
	mv test.sec code.sec
	mv test.cnt code.cnt
	mv test.map code.map
	hexdump -v -e '4/1 "%02x" "\n"' code.bin > code.txt

newton:
	$(AS_$(ARCH)) -o crt0.o lib/$(ARCH)/crt0.s
	$(GCC_$(ARCH)) -o libc.o lib/libc.c
	$(GCC_$(ARCH)) -o fp-math.o lib/fp-math.c
	$(GCC_$(ARCH)) -o math.o lib/math.c
	$(GCC_$(ARCH)) -o newton.o app/newton.c
	$(LD_$(ARCH)) -Tlib/$(ARCH)/hf-risc.ld -Map test.map -N -o test.axf *.o
	$(DUMP_$(ARCH)) --disassemble --reloc test.axf > test.lst
	$(DUMP_$(ARCH)) -h test.axf > test.sec
	$(DUMP_$(ARCH)) -s test.axf > test.cnt
	$(OBJ_$(ARCH)) -O binary test.axf test.bin
	$(SIZE_$(ARCH)) test.axf
	mv test.axf code.axf
	mv test.bin code.bin
	mv test.lst code.lst
	mv test.sec code.sec
	mv test.cnt code.cnt
	mv test.map code.map
	hexdump -v -e '4/1 "%02x" "\n"' code.bin > code.txt

pi:
	$(AS_$(ARCH)) -o crt0.o lib/$(ARCH)/crt0.s
	$(GCC_$(ARCH)) -o libc.o lib/libc.c
	$(GCC_$(ARCH)) -o fp-math.o lib/fp-math.c
	$(GCC_$(ARCH)) -o math.o lib/math.c
	$(GCC_$(ARCH)) -o pi.o app/pi.c
	$(LD_$(ARCH)) -Tlib/$(ARCH)/hf-risc.ld -Map test.map -N -o test.axf *.o
	$(DUMP_$(ARCH)) --disassemble --reloc test.axf > test.lst
	$(DUMP_$(ARCH)) -h test.axf > test.sec
	$(DUMP_$(ARCH)) -s test.axf > test.cnt
	$(OBJ_$(ARCH)) -O binary test.axf test.bin
	$(SIZE_$(ARCH)) test.axf
	mv test.axf code.axf
	mv test.bin code.bin
	mv test.lst code.lst
	mv test.sec code.sec
	mv test.cnt code.cnt
	mv test.map code.map
	hexdump -v -e '4/1 "%02x" "\n"' code.bin > code.txt

sort:
	$(AS_$(ARCH)) -o crt0.o lib/$(ARCH)/crt0.s
	$(GCC_$(ARCH)) -o libc.o lib/libc.c
	$(GCC_$(ARCH)) -o fp-math.o lib/fp-math.c
	$(GCC_$(ARCH)) -o math.o lib/math.c
	$(GCC_$(ARCH)) -o sort.o app/sort.c
	$(LD_$(ARCH)) -Tlib/$(ARCH)/hf-risc.ld -Map test.map -N -o test.axf *.o
	$(DUMP_$(ARCH)) --disassemble --reloc test.axf > test.lst
	$(DUMP_$(ARCH)) -h test.axf > test.sec
	$(DUMP_$(ARCH)) -s test.axf > test.cnt
	$(OBJ_$(ARCH)) -O binary test.axf test.bin
	$(SIZE_$(ARCH)) test.axf
	mv test.axf code.axf
	mv test.bin code.bin
	mv test.lst code.lst
	mv test.sec code.sec
	mv test.cnt code.cnt
	mv test.map code.map
	hexdump -v -e '4/1 "%02x" "\n"' code.bin > code.txt

test64:
	$(AS_$(ARCH)) -o crt0.o lib/$(ARCH)/crt0.s
	$(GCC_$(ARCH)) -o libc.o lib/libc.c
	$(GCC_$(ARCH)) -o fp-math.o lib/fp-math.c
	$(GCC_$(ARCH)) -o math.o lib/math.c
	$(GCC_$(ARCH)) -o test64.o app/test64.c
	$(LD_$(ARCH)) -Tlib/$(ARCH)/hf-risc.ld -Map test.map -N -o test.axf *.o
	$(DUMP_$(ARCH)) --disassemble --reloc test.axf > test.lst
	$(DUMP_$(ARCH)) -h test.axf > test.sec
	$(DUMP_$(ARCH)) -s test.axf > test.cnt
	$(OBJ_$(ARCH)) -O binary test.axf test.bin
	$(SIZE_$(ARCH)) test.axf
	mv test.axf code.axf
	mv test.bin code.bin
	mv test.lst code.lst
	mv test.sec code.sec
	mv test.cnt code.cnt
	mv test.map code.map
	hexdump -v -e '4/1 "%02x" "\n"' code.bin > code.txt

test_crc:
	$(AS_$(ARCH)) -o crt0.o lib/$(ARCH)/crt0.s
	$(GCC_$(ARCH)) -o libc.o lib/libc.c
	$(GCC_$(ARCH)) -o fp-math.o lib/fp-math.c
	$(GCC_$(ARCH)) -o math.o lib/math.c
	$(GCC_$(ARCH)) -o test_crc.o app/test_crc.c
	$(LD_$(ARCH)) -Tlib/$(ARCH)/hf-risc.ld -Map test.map -N -o test.axf *.o
	$(DUMP_$(ARCH)) --disassemble --reloc test.axf > test.lst
	$(DUMP_$(ARCH)) -h test.axf > test.sec
	$(DUMP_$(ARCH)) -s test.axf > test.cnt
	$(OBJ_$(ARCH)) -O binary test.axf test.bin
	$(SIZE_$(ARCH)) test.axf
	mv test.axf code.axf
	mv test.bin code.bin
	mv test.lst code.lst
	mv test.sec code.sec
	mv test.cnt code.cnt
	mv test.map code.map
	hexdump -v -e '4/1 "%02x" "\n"' code.bin > code.txt

test_fp:
	$(AS_$(ARCH)) -o crt0.o lib/$(ARCH)/crt0.s
	$(GCC_$(ARCH)) -o libc.o lib/libc.c
	$(GCC_$(ARCH)) -o fp-math.o lib/fp-math.c
	$(GCC_$(ARCH)) -o math.o lib/math.c
	$(GCC_$(ARCH)) -o spi.o lib/spi.c
	$(GCC_$(ARCH)) -o test_fp.o app/test_fp.c
	$(LD_$(ARCH)) -Tlib/$(ARCH)/hf-risc.ld -Map test.map -N -o test.axf *.o
	$(DUMP_$(ARCH)) --disassemble --reloc test.axf > test.lst
	$(DUMP_$(ARCH)) -h test.axf > test.sec
	$(DUMP_$(ARCH)) -s test.axf > test.cnt
	$(OBJ_$(ARCH)) -O binary test.axf test.bin
	$(SIZE_$(ARCH)) test.axf
	mv test.axf code.axf
	mv test.bin code.bin
	mv test.lst code.lst
	mv test.sec code.sec
	mv test.cnt code.cnt
	mv test.map code.map
	hexdump -v -e '4/1 "%02x" "\n"' code.bin > code.txt

test_spi:
	$(AS_$(ARCH)) -o crt0.o lib/$(ARCH)/crt0.s
	$(GCC_$(ARCH)) -o libc.o lib/libc.c
	$(GCC_$(ARCH)) -o fp-math.o lib/fp-math.c
	$(GCC_$(ARCH)) -o math.o lib/math.c
	$(GCC_$(ARCH)) -o spi.o lib/spi.c
	$(GCC_$(ARCH)) -o test_spi.o app/test_spi.c
	$(LD_$(ARCH)) -Tlib/$(ARCH)/hf-risc.ld -Map test.map -N -o test.axf *.o
	$(DUMP_$(ARCH)) --disassemble --reloc test.axf > test.lst
	$(DUMP_$(ARCH)) -h test.axf > test.sec
	$(DUMP_$(ARCH)) -s test.axf > test.cnt
	$(OBJ_$(ARCH)) -O binary test.axf test.bin
	$(SIZE_$(ARCH)) test.axf
	mv test.axf code.axf
	mv test.bin code.bin
	mv test.lst code.lst
	mv test.sec code.sec
	mv test.cnt code.cnt
	mv test.map code.map
	hexdump -v -e '4/1 "%02x" "\n"' code.bin > code.txt

xtea:
	$(AS_$(ARCH)) -o crt0.o lib/$(ARCH)/crt0.s
	$(GCC_$(ARCH)) -o libc.o lib/libc.c
	$(GCC_$(ARCH)) -o fp-math.o lib/fp-math.c
	$(GCC_$(ARCH)) -o math.o lib/math.c
	$(GCC_$(ARCH)) -o xtea.o app/xtea.c
	$(LD_$(ARCH)) -Tlib/$(ARCH)/hf-risc.ld -Map test.map -N -o test.axf *.o
	$(DUMP_$(ARCH)) --disassemble --reloc test.axf > test.lst
	$(DUMP_$(ARCH)) -h test.axf > test.sec
	$(DUMP_$(ARCH)) -s test.axf > test.cnt
	$(OBJ_$(ARCH)) -O binary test.axf test.bin
	$(SIZE_$(ARCH)) test.axf
	mv test.axf code.axf
	mv test.bin code.bin
	mv test.lst code.lst
	mv test.sec code.sec
	mv test.cnt code.cnt
	mv test.map code.map
	hexdump -v -e '4/1 "%02x" "\n"' code.bin > code.txt
	
xtea_hw:
	$(AS_$(ARCH)) -o crt0.o lib/$(ARCH)/crt0.s
	$(GCC_$(ARCH)) -o libc.o lib/libc.c
	$(GCC_$(ARCH)) -o fp-math.o lib/fp-math.c
	$(GCC_$(ARCH)) -o math.o lib/math.c
	$(GCC_$(ARCH)) -o xtea_hw.o app/xtea_hw.c
	$(LD_$(ARCH)) -Tlib/$(ARCH)/hf-risc.ld -Map test.map -N -o test.axf *.o
	$(DUMP_$(ARCH)) --disassemble --reloc test.axf > test.lst
	$(DUMP_$(ARCH)) -h test.axf > test.sec
	$(DUMP_$(ARCH)) -s test.axf > test.cnt
	$(OBJ_$(ARCH)) -O binary test.axf test.bin
	$(SIZE_$(ARCH)) test.axf
	mv test.axf code.axf
	mv test.bin code.bin
	mv test.lst code.lst
	mv test.sec code.sec
	mv test.cnt code.cnt
	mv test.map code.map
	hexdump -v -e '4/1 "%02x" "\n"' code.bin > code.txt

xtea_hw_vec:
	$(AS_$(ARCH)) -o crt0.o lib/$(ARCH)/crt0.s
	$(GCC_$(ARCH)) -o libc.o lib/libc.c
	$(GCC_$(ARCH)) -o fp-math.o lib/fp-math.c
	$(GCC_$(ARCH)) -o math.o lib/math.c
	$(GCC_$(ARCH)) -o xtea_hw_vec.o app/xtea_hw_vec.c
	$(LD_$(ARCH)) -Tlib/$(ARCH)/hf-risc.ld -Map test.map -N -o test.axf *.o
	$(DUMP_$(ARCH)) --disassemble --reloc test.axf > test.lst
	$(DUMP_$(ARCH)) -h test.axf > test.sec
	$(DUMP_$(ARCH)) -s test.axf > test.cnt
	$(OBJ_$(ARCH)) -O binary test.axf test.bin
	$(SIZE_$(ARCH)) test.axf
	mv test.axf code.axf
	mv test.bin code.bin
	mv test.lst code.lst
	mv test.sec code.sec
	mv test.cnt code.cnt
	mv test.map code.map
	hexdump -v -e '4/1 "%02x" "\n"' code.bin > code.txt

clean:
	-rm -rf *~ *.o *.axf *.map *.lst *.sec *.cnt *.txt *.bin *.vhd
	-rm -rf app/*~
	-rm -rf boot/*~
	-rm -rf include/*~
	-rm -rf lib/*~
