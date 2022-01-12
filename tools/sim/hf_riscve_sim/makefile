CC = gcc
CFLAGS = -O2 -Wall -lm $$(pkg-config --cflags --libs sdl2)

all:
	$(CC) $(CFLAGS) hf_riscve_sim.c -o hf_riscve_sim
	
clean:
	rm -rf *.o *~ hf_riscv_sim
