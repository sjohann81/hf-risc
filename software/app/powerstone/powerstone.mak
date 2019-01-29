all:

powerstone-adpcm: crt
	$(GCC_$(ARCH)) -o adpcm.o app/powerstone/adpcm.c
	@$(MAKE) --no-print-directory link

powerstone-bcnt: crt
	$(GCC_$(ARCH)) -o adpcm.o app/powerstone/bcnt.c
	@$(MAKE) --no-print-directory link

powerstone-blit: crt
	$(GCC_$(ARCH)) -o adpcm.o app/powerstone/blit.c
	@$(MAKE) --no-print-directory link
	
powerstone-compress: crt
	$(GCC_$(ARCH)) -o adpcm.o app/powerstone/compress.c
	@$(MAKE) --no-print-directory link
	
powerstone-crc: crt
	$(GCC_$(ARCH)) -o adpcm.o app/powerstone/crc.c
	@$(MAKE) --no-print-directory link

powerstone-engine: crt
	$(GCC_$(ARCH)) -o adpcm.o app/powerstone/engine.c
	@$(MAKE) --no-print-directory link

powerstone-fir: crt
	$(GCC_$(ARCH)) -o adpcm.o app/powerstone/fir.c
	@$(MAKE) --no-print-directory link
	
powerstone-g3fax: crt
	$(GCC_$(ARCH)) -o adpcm.o app/powerstone/g3fax.c
	@$(MAKE) --no-print-directory link
	
powerstone-huff: crt
	$(GCC_$(ARCH)) -o adpcm.o app/powerstone/huff.c
	@$(MAKE) --no-print-directory link
	
powerstone-jpeg: crt
	$(GCC_$(ARCH)) -o adpcm.o app/powerstone/jpeg.c
	@$(MAKE) --no-print-directory link
	
powerstone-pocsag: crt
	$(GCC_$(ARCH)) -o adpcm.o app/powerstone/pocsag.c
	@$(MAKE) --no-print-directory link
	
powerstone-qurt: crt
	$(GCC_$(ARCH)) -o adpcm.o app/powerstone/qurt.c
	@$(MAKE) --no-print-directory link
	
powerstone-ucbqsort: crt
	$(GCC_$(ARCH)) -o adpcm.o app/powerstone/ucbqsort.c
	@$(MAKE) --no-print-directory link
	
powerstone-v42: crt
	$(GCC_$(ARCH)) -o adpcm.o app/powerstone/v42.c
	@$(MAKE) --no-print-directory link
