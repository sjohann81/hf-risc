# architecture: mips or riscv
ARCH=riscv

# board specific parameters
BAUDRATE=57600
F_CLK=25000000
SERIAL_DEV=/dev/ttyUSB0

# compiler flags
CFLAGS_STRIP = -fdata-sections -ffunction-sections
LDFLAGS_STRIP = --gc-sections
LD_SCRIPT = hf-risc.ld

GCC_riscv = riscv32-unknown-elf-gcc -march=rv32i -mabi=ilp32 -Wall -O2 -c -mstrict-align -ffreestanding -nostdlib -ffixed-s10 -ffixed-s11 -I ../../include -DCPU_SPEED=${F_CLK} -DLITTLE_ENDIAN $(CFLAGS_STRIP) -DDEBUG_PORT #-ffixed-s10 -ffixed-s11 -mno-div -mrvc -fPIC -DDEBUG_PORT
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
	$(AS_$(ARCH)) -o crt0.o ../../lib/$(ARCH)/crt0.s
	$(GCC_$(ARCH)) -o libc.o ../../lib/libc.c
	$(GCC_$(ARCH)) -o interrupt.o ../../lib/interrupt.c
	$(GCC_$(ARCH)) -o math.o ../../lib/math.c
	$(GCC_$(ARCH)) -o malloc.o ../../lib/malloc.c

link:
	$(LD_$(ARCH)) -T../../lib/$(ARCH)/$(LD_SCRIPT) -Map test.map -N -o test.elf *.o
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

assoc_test: crt
	$(GCC_$(ARCH)) -o assoc_test.o assoc_test.c
	@$(MAKE) --no-print-directory link

clean:
	-rm -rf *~ *.o *.elf *.map *.lst *.sec *.cnt *.txt *.bin *.vhd
	-rm -rf app/*~
	-rm -rf boot/*~
	-rm -rf include/*~
	-rm -rf lib/*~
