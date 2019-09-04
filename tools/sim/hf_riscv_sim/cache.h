struct cache_set_way_s {
	uint8_t valid;
	uint8_t dirty;
	uint32_t tag;
	uint32_t *block;
	uint32_t counter;
};

struct cache_set_s {
	struct cache_set_way_s *ways;
	uint32_t last_way;
};

struct miss_penalty_s {
	uint32_t setup_time;
	uint32_t burst_time;
};

struct access_s {
	uint64_t hits;
	uint64_t misses;
	uint64_t reads;
	uint64_t writes;
};

struct config_masks_s {
	uint32_t tag_mask;
	uint32_t tag_shift;
	uint32_t set_mask;
	uint32_t set_shift;
	uint32_t block_mask;
};

struct cache_s {
	uint8_t level;
	int8_t *policy;
	uint32_t sets;
	uint32_t block_size;
	uint32_t ways;
	int8_t replacement_policy;
	struct miss_penalty_s miss_penalty;
	struct config_masks_s config;
	struct access_s access;
	struct cache_set_s *cache_sets;
	struct cache_s *next_level;
};

int32_t cache_init(struct cache_s *cache, int8_t *policy, uint32_t sets, uint32_t block_size, uint32_t ways, uint32_t miss_setup, uint32_t miss_burst);
uint32_t cache_read(struct cache_s *cache, uint32_t address);
uint32_t cache_write(struct cache_s *cache, uint32_t address);
void cache_finish(struct cache_s *cache);
