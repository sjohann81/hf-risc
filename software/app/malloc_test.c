#include <hf-risc.h>

char mem_pool[8192];

int main(){
	int s = 0;
	void *p;

	heap_init((uint32_t *)&mem_pool, sizeof(mem_pool));

	while (1) {
		p = malloc(1024);
		if (p) {
			s += 1024;
			printf("malloc() %d bytes\n", s);
		} else {
			printf("malloc() failed\n");
			break;
		}
	}

	return 0;
}
