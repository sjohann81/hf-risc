#include <hf-risc.h>

int main(){
	int s = 0;
	void *p;
	
	printf("heap_init(), zone size: %d bytes\n", (uint32_t)&_heap_end - (uint32_t)&_heap_start);
	heap_init((uint32_t *)&_heap_start, (uint32_t)&_heap_end - (uint32_t)&_heap_start);
	
	while (1) {
		p = malloc(1024);
		if (p) {
			s += 1024;
			printf("malloc() %d bytes ", s);
		} else {
			printf("malloc() failed\n");
			break;
		}
	}
	
	return 0;
}
