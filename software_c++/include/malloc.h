typedef unsigned long			size_t;

#define align4(x) ((((x) + 3) >> 2) << 2)

struct mem_block_s {
	struct mem_block_s *next;		/* pointer to the next block */
	size_t size;				/* aligned block size. the LSB is used to define if the block is used */
};

struct mem_block_s *first_free;
struct mem_block_s *last_free;

void free(void *ptr);
void *malloc(uint32_t size);
void heap_init(uint32_t *zone, uint32_t len);
