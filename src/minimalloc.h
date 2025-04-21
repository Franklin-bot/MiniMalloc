# pragma once

#include <vector>
#include <algorithm>
#include <iostream>
#include <unordered_map>

#include <stddef.h>
#include <stdint.h>

#include "thread_cache.h"
#include "bucket.h"
#include "config.h"

class minimalloc {

    private:

        std::unordered_map<size_t, size_t> block_indices_;
        std::vector<size_t>block_sizes_;
        size_t max_block_;
        size_t min_block_;
        size_t bucket_count_;
        uint64_t num_blocks_;
        uint64_t memory_pool_size_;
        std::vector<bucket> pool_buckets_;
        std::vector<thread_cache> thread_caches_;
        std::unique_ptr<uint8_t[]> p_global_pool_;

        void cache_warmup(thread_cache& bc);
        size_t get_bucket(size_t n) const;
        int get_bucket(void* pointer) const;
        void return_from_bucket_cache(size_t n, thread_cache::bucket_cache& b);
        void release_to_bucket_cache(size_t n, thread_cache::bucket_cache& b);

    public:

        minimalloc(const std::vector<size_t>& block_sizes, uint64_t memory_pool_size); 
        ~minimalloc();

        void* allocate(size_t bytes);
        void deallocate(void* pointer);
        void get_stats();
};

