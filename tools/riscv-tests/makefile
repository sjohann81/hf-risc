BAUDRATE=57600
F_CLK=25000000
SERIAL_DEV=/dev/ttyUSB0

#remove unreferenced functions
CFLAGS_STRIP = -fdata-sections -ffunction-sections
LDFLAGS_STRIP = --gc-sections

GCC_RISCV = riscv64-unknown-elf-gcc -march=RV32I -O2 -c -msoft-float -nostdinc -fno-builtin -ffixed-s10 -ffixed-s11 -I ./../../hf_risc_test/include -DCPU_SPEED=${F_CLK} $(CFLAGS_STRIP) -DDEBUG_PORT
AS_RISCV = riscv64-unknown-elf-as -m32
LD_RISCV = riscv64-unknown-elf-ld -melf32lriscv
DUMP_RISCV = riscv64-unknown-elf-objdump #-Mno-aliases
READ_RISCV = riscv64-unknown-elf-readelf
OBJ_RISCV = riscv64-unknown-elf-objcopy
SIZE_RISCV = riscv64-unknown-elf-size

TEST_OBJS = $(addsuffix .o,$(basename $(wildcard tests/*.S)))

all:

simple: $(TEST_OBJS)
	$(GCC_RISCV) -o test.o test.S
	$(LD_RISCV) -Thf-risc.ld -Map test.map -N -o test.axf \
		test.o $(TEST_OBJS)
	$(DUMP_RISCV) --disassemble --reloc test.axf > test.lst
	$(DUMP_RISCV) -h test.axf > test.sec
	$(DUMP_RISCV) -s test.axf > test.cnt
	$(OBJ_RISCV) -O binary test.axf test.bin
	$(SIZE_RISCV) test.axf
	mv test.axf code.axf
	mv test.bin code.bin
	mv test.lst code.lst
	mv test.sec code.sec
	mv test.cnt code.cnt
	mv test.map code.map
	hexdump -v -e '4/1 "%02x" "\n"' code.bin > code.txt

tests/%.o: tests/%.S tests/riscv_test.h tests/test_macros.h
	$(GCC_RISCV) -o $@ -DTEST_FUNC_NAME=$(notdir $(basename $<)) \
		-DTEST_FUNC_TXT='"$(notdir $(basename $<))"' -DTEST_FUNC_RET=$(notdir $(basename $<))_ret $<

clean:
	-rm -rf *.o *.axf *.map *.lst *.sec *.cnt *.txt *.bin *~
	-rm -rf tests/*.o
