BAUDRATE=57600
SERIAL_DEV=/dev/ttyUSB0

CFLAGS_FEW_REGS = -ffixed-t0 -ffixed-t1 -ffixed-t2 -ffixed-t3 -ffixed-t4 -ffixed-t5 -ffixed-t6 -ffixed-t7 -ffixed-s0 -ffixed-s1 -ffixed-s2 -ffixed-s3 -ffixed-s4 -ffixed-s5 -ffixed-s6 -ffixed-s7
CFLAGS_FEW_FEW_REGS = -ffixed-t8 -ffixed-t9 -ffixed-a0 -ffixed-a1 -ffixed-v0 -ffixed-v1 -ffixed-fp -ffixed-gp
CFLAGS_NO_HW_MULDIV = -mnohwmult -mnohwdiv -ffixed-lo -ffixed-hi
CFLAGS_NO_HW_DIV = -mnohwdiv
# recycle $gp as a temporary register, do not put instructions on the BDS
CFLAGS_OTHER = -fcall-used-gp -fno-delayed-branch
# more aggressive optimizations, also turns -frename-registers on by default.
CFLAGS_XTREME = -funroll-all-loops -fgcse-sm -finline-limit=500 -fno-schedule-insns # used on many mips cores (ImgTech) ==> -funroll-all-loops -falign-jumps=16 -falign-loops=16 -fgcse-sm -fgcse-las -finline-functions -finline-limit=300 #

#remove unreferenced functions
CFLAGS_STRIP = -fdata-sections -ffunction-sections
LDFLAGS_STRIP = --gc-sections

GCC_MIPS = mips-elf-gcc -O2 -c -mips2 -mno-branch-likely -mpatfree -mfix-r4000 -mno-check-zero-division -msoft-float -fshort-double -ffreestanding -nostdlib -fomit-frame-pointer -G 8 -I ../../hf_risc_test/include -DCPU_SPEED=25000000 -DBIG_ENDIAN $(CFLAGS_STRIP) $(CFLAGS_NO_HW_DIV) -DDEBUG_PORT #-DOPENSSL_SMALL_FOOTPRINT #-fPIC -mabicalls #$(CFLAGS_FEW_REGS) -DDEBUG_PORT

AS_MIPS = mips-elf-as -mips1
LD_MIPS = mips-elf-ld -mips1 $(LDFLAGS_STRIP)
DUMP_MIPS = mips-elf-objdump
READ_MIPS = mips-elf-readelf
OBJ_MIPS = mips-elf-objcopy
SIZE_MIPS = mips-elf-size

all:

serial:
	stty ${BAUDRATE} raw cs8 -parenb -crtscts clocal cread ignpar ignbrk -ixon -ixoff -ixany -brkint -icrnl -imaxbel -opost -onlcr -isig -icanon -iexten -echo -echoe -echok -echoctl -echoke -F ${SERIAL_DEV}

load: serial
	cp code.bin ${SERIAL_DEV}

debug: serial
	cat ${SERIAL_DEV}

aes:
	$(AS_MIPS) -o boot.o ../../hf_risc_test/crt0.s
	$(GCC_MIPS) -o libc.o ../../hf_risc_test/libc.c
	$(GCC_MIPS) -o aes.o aes.c
	$(LD_MIPS) -T../../hf_risc_test/hf_risc.ld -Map test.map -N -o test.axf \
		boot.o libc.o aes.o
	$(DUMP_MIPS) --disassemble --reloc test.axf > test.lst
	$(DUMP_MIPS) -h test.axf > test.sec
	$(DUMP_MIPS) -s test.axf > test.cnt
	$(OBJ_MIPS) -O binary test.axf test.bin
	$(SIZE_MIPS) test.axf
	mv test.axf code.axf
	mv test.bin code.bin
	mv test.lst code.lst
	mv test.sec code.sec
	mv test.cnt code.cnt
	mv test.map code.map
	hexdump -v -e '4/1 "%02x" "\n"' code.bin > code.txt

clean:
	-rm -rf *.o *.axf *.map *.lst *.sec *.cnt *.txt *.bin *~

