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
CFLAGS_MIPS = -Wall -O2 -c -mips2 -mno-branch-likely -mpatfree -mfix-r4000 -mno-check-zero-division -msoft-float -ffreestanding -nostdlib -fomit-frame-pointer -G 0 -I ./include -DCPU_SPEED=${F_CLK} -DBIG_ENDIAN $(CFLAGS_NO_HW_MULDIV) #-DDEBUG_PORT
GCC_mips = mips-elf-gcc  $(CFLAGS_STRIP)
AS_mips = mips-elf-as -mips1 -msoft-float
LD_mips = mips-elf-ld -mips1 #$(LDFLAGS_STRIP)
DUMP_mips = mips-elf-objdump
READ_mips = mips-elf-readelf
OBJ_mips = mips-elf-objcopy
SIZE_mips = mips-elf-size

CFLAGS_RISCV = -march=rv32i -mabi=ilp32 -Wall -O2 -c -mstrict-align -ffreestanding -nostdlib -ffixed-s10 -ffixed-s11 -I ./include -DCPU_SPEED=${F_CLK} -DLITTLE_ENDIAN #-DDEBUG_PORT #-mno-div -mrvc -fPIC -DDEBUG_PORT
GCC_riscv = riscv32-unknown-elf-gcc $(CFLAGS_RISCV) $(CFLAGS_STRIP)
GXX_riscv = riscv32-unknown-elf-g++ $(CFLAGS_RISCV) -fno-exceptions -fno-unwind-tables -fno-rtti $(CFLAGS_STRIP)
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

test_cpp:
	$(AS_$(ARCH)) -o crt0.o lib/riscv/crt0.s
	$(GCC_$(ARCH)) -o libc.o lib/libc.c
	$(GCC_$(ARCH)) -o malloc.o lib/malloc.c
	$(GXX_$(ARCH)) -o startup.o lib/riscv/startup.cpp
	$(GXX_$(ARCH)) -o test_cpp.o app/test_cpp.cpp

	$(LD_$(ARCH)) -Tlib/$(ARCH)/riscv.ld -o test.elf \
		crt0.o libc.o malloc.o startup.o test_cpp.o
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
	hexdump -v -e '4/1 "%02x" "\n"' code.bin > boot.txt

test_cpp2:
	$(AS_$(ARCH)) -o crt0.o lib/riscv/crt0.s
	$(GCC_$(ARCH)) -o libc.o lib/libc.c
	$(GCC_$(ARCH)) -o malloc.o lib/malloc.c
	$(GXX_$(ARCH)) -o startup.o lib/riscv/startup.cpp
	$(GXX_$(ARCH)) -o test_cpp2.o app/test_cpp2.cpp

	$(LD_$(ARCH)) -Tlib/$(ARCH)/riscv.ld -o test.elf \
		crt0.o libc.o malloc.o startup.o test_cpp2.o
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
	hexdump -v -e '4/1 "%02x" "\n"' code.bin > boot.txt

clean:
	-rm -rf *~ *.o *.elf *.map *.lst *.sec *.cnt *.txt *.bin *.vhd
	-rm -rf app/*~
	-rm -rf boot/*~
	-rm -rf include/*~
	-rm -rf lib/*~
