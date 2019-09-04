/*
 * TODO:
 * -generalise the cache data structure to 1 way (DM) to N way (set associative)
 * -implement the replacement strategy to associative caches (use FIFO strategy initially)
 * -multi-level, configurable caches
 * -(optional) implement write buffers. this is very good for DM, no write allocate caches (reduce write penalty)
 * -(optional) implement victim cache.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "cache.h"

int32_t cache_init(struct cache_s *cache, int8_t *policy, uint32_t sets, uint32_t block_size, uint32_t ways, uint32_t miss_setup, uint32_t miss_burst){
	uint32_t i, j, size, mask, bit;
	
	// skip top cache levels (until cache->next_level == NULL), then proceed.
	cache->policy = policy;
	cache->block_size = block_size;
	cache->sets = sets;
	cache->miss_penalty.setup_time = miss_setup;
	cache->miss_penalty.burst_time = miss_burst;
	cache->cache_sets = (struct cache_set_s *)malloc(sets * sizeof(struct cache_set_s));
	for (i = 0; i < sets; i++){
		cache->cache_sets[i].last_way = 0;
		cache->cache_sets[i].ways = (struct cache_set_way_s *)malloc(ways * sizeof(struct cache_set_way_s));
		for (j = 0; j < ways; j++){
			cache->cache_sets[i].ways[j].valid = 0;
			cache->cache_sets[i].ways[j].dirty = 0;
			cache->cache_sets[i].ways[j].tag = 0;
			/* we are just simulating the cache, no need for block allocation */
			cache->cache_sets[i].ways[j].block = 0;
		}
	}
	cache->next_level = 0;
	
	mask = 0;
	bit = cache->block_size;
	while (bit >>= 1)
		mask = (mask << 1) | 1;
	cache->config.block_mask = mask;
	
	mask = 0;
	bit = cache->sets;
	while (bit >>= 1)
		mask = (mask << 1) | 1;
	cache->config.set_mask = mask;
	bit = cache->block_size;
	cache->config.set_shift = 0;
	while (bit >>= 1)
		cache->config.set_shift++;
	
	mask = 0xffffffff;
	bit = cache->block_size;
	while (bit >>= 1)
		mask >>= 1;
	bit = cache->sets;
	while (bit >>= 1)
		mask >>= 1;
	cache->config.tag_mask = mask;
	bit = cache->sets;
	cache->config.tag_shift = 0;
	while (bit >>= 1)
		cache->config.tag_shift++;
	cache->config.tag_shift += cache->config.set_shift;
	
	cache->access.hits = 0;
	cache->access.misses = 0;
	cache->access.reads = 0;
	cache->access.writes = 0;
	
	/* we assume word size is 32 bits */
	size = block_size * 4 * sets;
	
	if (strcmp((const char *)policy, "wt") == 0)
		printf("write through, no write allocate):\n");
	if (strcmp((const char *)policy, "wa") == 0)
		printf("write through, write allocate):\n");
	if (strcmp((const char *)policy, "wb") == 0)
		printf("write back):\n");
	printf("\nsets:				%d", sets);
	printf("\nblock size (words):		%d", block_size);
	printf("\nsize (bytes):			%d + tag", size);
	printf("\n");

	return 0;
}

uint32_t cache_read(struct cache_s *cache, uint32_t address){
	uint32_t tag, set, dirty_penalty = 0;//, block;
	
	/* we assume word size is 32 bits */
//	block = (address >> 2) & cache->config.block_mask;
	set = (address >> cache->config.set_shift >> 2) & cache->config.set_mask;
	tag = (address >> cache->config.tag_shift >> 2) & cache->config.tag_mask;
//	printf("\naddr: [r] %08x tag: %08x set: %08x block: %08x", address, tag, set, block);

	// on misses, proceed to the next level, calling cache_read() recursively
	if (strcmp((const char *)cache->policy, "wb") == 0){
		/* write back */
		if (cache->cache_sets[set].ways[0].valid && cache->cache_sets[set].ways[0].tag == tag){
			cache->access.hits++;
			cache->access.reads++;
			/* read hit, no penalty */
		}else{
			if (cache->cache_sets[set].ways[0].dirty)
				dirty_penalty = cache->miss_penalty.setup_time + cache->miss_penalty.burst_time;
			cache->access.misses++;
			cache->cache_sets[set].ways[0].valid = 1;
			cache->cache_sets[set].ways[0].dirty = 0;
			cache->cache_sets[set].ways[0].tag = tag;
			cache->access.reads++;
			/* if not dirty, it's just a miss. if dirty, we must account for the time to write a whole block back to memory */
			return dirty_penalty + cache->miss_penalty.setup_time + cache->miss_penalty.burst_time;
		}
	}else{
		/* write through */
		if (cache->cache_sets[set].ways[0].valid && cache->cache_sets[set].ways[0].tag == tag){
			cache->access.hits++;
			cache->access.reads++;
			/* read hit, no penalty */
		}else{
			cache->access.misses++;
			cache->cache_sets[set].ways[0].valid = 1;
			cache->cache_sets[set].ways[0].tag = tag;
			cache->access.reads++;
			/* read miss, one block fetch penalty */
			return cache->miss_penalty.setup_time + cache->miss_penalty.burst_time;
		}
	}
	
	return 0;
}

uint32_t cache_write(struct cache_s *cache, uint32_t address){
	uint32_t tag, set;//, block;
	
	/* we assume word size is 32 bits */
//	block = (address >> 2) & cache->config.block_mask;
	set = (address >> cache->config.set_shift >> 2) & cache->config.set_mask;
	tag = (address >> cache->config.tag_shift >> 2) & cache->config.tag_mask;
//	printf("\naddr: [w] %08x tag: %08x set: %08x block: %08x", address, tag, set, block);

	// on misses/no cached ops, proceed to the next level, calling cache_write() recursively

	/* write through, no write allocate */
	if (strcmp((const char *)cache->policy, "wt") == 0){
		cache->access.writes++;
		/* no writes are cached. update the word in memory. */
		return cache->miss_penalty.setup_time + cache->miss_penalty.burst_time / cache->block_size;
	}else{
		/* write through, write allocate */
		if (strcmp((const char *)cache->policy, "wa") == 0){
			if (cache->cache_sets[set].ways[0].valid && cache->cache_sets[set].ways[0].tag == tag){
				cache->access.hits++;
				cache->access.writes++;
				/* cached writes. write to cache and update the changed word in memory */
				return cache->miss_penalty.setup_time + cache->miss_penalty.burst_time / cache->block_size;
			}else{
				cache->access.misses++;
				cache->cache_sets[set].ways[0].valid = 1;
				cache->cache_sets[set].ways[0].tag = tag;
				cache->access.writes++;
				/* write the changed word in memory and allocate the cache set (read) */
				return (cache->miss_penalty.setup_time + cache->miss_penalty.burst_time / cache->block_size)
				 + cache->miss_penalty.setup_time + cache->miss_penalty.burst_time;
			}
		/* write back */
		}else{
			if (cache->cache_sets[set].ways[0].valid && cache->cache_sets[set].ways[0].tag == tag){
				cache->access.hits++;
				cache->access.writes++;
				cache->cache_sets[set].ways[0].dirty = 1;
				/* write hit, no penalty. set is now dirty, however. */
				return 0;
			}else{
				cache->access.misses++;
				cache->cache_sets[set].ways[0].valid = 1;
				cache->cache_sets[set].ways[0].dirty = 0;
				cache->cache_sets[set].ways[0].tag = tag;
				cache->access.writes++;
				/* write the changed word in memory and allocate the cache set (read, write allocate) */
				return (cache->miss_penalty.setup_time + cache->miss_penalty.burst_time / cache->block_size)
				 + cache->miss_penalty.setup_time + cache->miss_penalty.burst_time;
			}
		}
	}

	return 0;
}

void cache_finish(struct cache_s *cache){
	// do this for each cache level, showing statistics and freeing data structures accordingly
	printf("\nhits:			%ld", cache->access.hits);
	printf("\nmisses:			%ld", cache->access.misses);
	printf("\nhit ratio:		%ld%%", (cache->access.hits * 100) / (cache->access.hits + cache->access.misses));
	printf("\nreads:			%ld", cache->access.reads);
	printf("\nwrites:			%ld\n", cache->access.writes);
	
	free(cache->cache_sets->ways);
	free(cache->cache_sets);
};
