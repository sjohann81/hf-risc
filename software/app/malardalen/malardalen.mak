all:

malardalen-adpcm: crt
	$(GCC_$(ARCH)) -o adpcm.o app/malardalen/adpcm.c
	@$(MAKE) --no-print-directory link

malardalen-bs: crt
	$(GCC_$(ARCH)) -o bs.o app/malardalen/bs.c
	@$(MAKE) --no-print-directory link

malardalen-bsort100: crt
	$(GCC_$(ARCH)) -o bsort100.o app/malardalen/bsort100.c
	@$(MAKE) --no-print-directory link

malardalen-cnt: crt
	$(GCC_$(ARCH)) -o cnt.o app/malardalen/cnt.c
	@$(MAKE) --no-print-directory link

malardalen-compress: crt
	$(GCC_$(ARCH)) -o compress.o app/malardalen/compress.c
	@$(MAKE) --no-print-directory link

malardalen-cover: crt
	$(GCC_$(ARCH)) -o cover.o app/malardalen/cover.c
	@$(MAKE) --no-print-directory link

malardalen-crc: crt
	$(GCC_$(ARCH)) -o crc.o app/malardalen/crc.c
	@$(MAKE) --no-print-directory link

malardalen-duff: crt
	$(GCC_$(ARCH)) -o duff.o app/malardalen/duff.c
	@$(MAKE) --no-print-directory link

malardalen-edn: crt
	$(GCC_$(ARCH)) -o edn.o app/malardalen/edn.c
	@$(MAKE) --no-print-directory link

malardalen-expint: crt
	$(GCC_$(ARCH)) -o expint.o app/malardalen/expint.c
	@$(MAKE) --no-print-directory link

malardalen-fac: crt
	$(GCC_$(ARCH)) -o fac.o app/malardalen/fac.c
	@$(MAKE) --no-print-directory link

malardalen-fdct: crt
	$(GCC_$(ARCH)) -o fdct.o app/malardalen/fdct.c
	@$(MAKE) --no-print-directory link

malardalen-fft1: crt
	$(GCC_$(ARCH)) -o fft1.o app/malardalen/fft1.c
	@$(MAKE) --no-print-directory link

malardalen-fibcall: crt
	$(GCC_$(ARCH)) -o fibcall.o app/malardalen/fibcall.c
	@$(MAKE) --no-print-directory link

malardalen-fir: crt
	$(GCC_$(ARCH)) -o fir.o app/malardalen/fir.c
	@$(MAKE) --no-print-directory link

malardalen-insertsort: crt
	$(GCC_$(ARCH)) -o insertsort.o app/malardalen/insertsort.c
	@$(MAKE) --no-print-directory link

malardalen-janne_complex: crt
	$(GCC_$(ARCH)) -o janne_complex.o app/malardalen/janne_complex.c
	@$(MAKE) --no-print-directory link

malardalen-jfdctint: crt
	$(GCC_$(ARCH)) -o jfdctint.o app/malardalen/jfdctint.c
	@$(MAKE) --no-print-directory link

malardalen-lcdnum: crt
	$(GCC_$(ARCH)) -o lcdnum.o app/malardalen/lcdnum.c
	@$(MAKE) --no-print-directory link

malardalen-lms: crt
	$(GCC_$(ARCH)) -o lms.o app/malardalen/lms.c
	@$(MAKE) --no-print-directory link

malardalen-ludcmp: crt
	$(GCC_$(ARCH)) -o ludcmp.o app/malardalen/ludcmp.c
	@$(MAKE) --no-print-directory link

malardalen-matmult: crt
	$(GCC_$(ARCH)) -o matmult.o app/malardalen/matmult.c
	@$(MAKE) --no-print-directory link

malardalen-minver: crt
	$(GCC_$(ARCH)) -o minver.o app/malardalen/minver.c
	@$(MAKE) --no-print-directory link

malardalen-ndes: crt
	$(GCC_$(ARCH)) -o ndes.o app/malardalen/ndes.c
	@$(MAKE) --no-print-directory link

malardalen-ns: crt
	$(GCC_$(ARCH)) -o ns.o app/malardalen/ns.c
	@$(MAKE) --no-print-directory link

malardalen-nsichneu: crt
	$(GCC_$(ARCH)) -o nsichneu.o app/malardalen/nsichneu.c
	@$(MAKE) --no-print-directory link

malardalen-prime: crt
	$(GCC_$(ARCH)) -o prime.o app/malardalen/prime.c
	@$(MAKE) --no-print-directory link

malardalen-qsort-exam: crt
	$(GCC_$(ARCH)) -o qsort-exam.o app/malardalen/qsort-exam.c
	@$(MAKE) --no-print-directory link

malardalen-qurt: crt
	$(GCC_$(ARCH)) -o qurt.o app/malardalen/qurt.c
	@$(MAKE) --no-print-directory link

malardalen-recursion: crt
	$(GCC_$(ARCH)) -o recursion.o app/malardalen/recursion.c
	@$(MAKE) --no-print-directory link

malardalen-select: crt
	$(GCC_$(ARCH)) -o select.o app/malardalen/select.c
	@$(MAKE) --no-print-directory link

malardalen-sqrt: crt
	$(GCC_$(ARCH)) -o sqrt.o app/malardalen/sqrt.c
	@$(MAKE) --no-print-directory link

malardalen-st: crt
	$(GCC_$(ARCH)) -o st.o app/malardalen/st.c
	@$(MAKE) --no-print-directory link

malardalen-statemate: crt
	$(GCC_$(ARCH)) -o statemate.o app/malardalen/statemate.c
	@$(MAKE) --no-print-directory link

malardalen-ud: crt
	$(GCC_$(ARCH)) -o ud.o app/malardalen/ud.c
	@$(MAKE) --no-print-directory link

malardalen-whet: crt
	$(GCC_$(ARCH)) -o whet.o app/malardalen/whet.c
	@$(MAKE) --no-print-directory link
