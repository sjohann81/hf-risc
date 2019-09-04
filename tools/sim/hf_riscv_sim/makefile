CC = gcc
CFLAGS = -O2 -Wall

all:
	$(CC) $(CFLAGS) hf_riscv_sim.c -o hf_riscv_sim
	$(CC) $(CFLAGS) -c cache.c -o cache.o
	$(CC) $(CFLAGS) -c hf_riscv_sim_cache.c -o hf_riscv_sim_cache.o
	$(CC) $(CFLAGS) cache.o hf_riscv_sim_cache.o -o hf_riscv_sim_cache
	
clean:
	rm -rf *.o *~ hf_riscv_sim hf_riscv_sim_full hf_riscv_sim_cache
