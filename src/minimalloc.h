# pragma once

#include <vector>
#include <algorithm>
#include <iostream>
#include <unordered_map>
#include <thread>

#include <stddef.h>
#include <stdint.h>

#include "thread_cache.h"
#include "bucket.h"
#include "internal/config.h"
#include "internal/stats.h"
#include "internal/mmap.h"

namespace minimalloc {
class minimalloc {

    private:

        uint64_t global_pool_size_;

        Pool global_pool_;
        std::vector<size_t>block_sizes_;
        std::vector<bucket> global_pool_buckets_;
        std::unordered_map<size_t, size_t> block_indices_;

        void cache_warmup(thread_cache* tc);
        size_t get_bucket(size_t n) const;
        int get_bucket(void* pointer) const;
        void return_from_bucket_cache(size_t n, thread_cache::bucket_cache& b);
        void release_to_bucket_cache(size_t n, thread_cache::bucket_cache& b);

    public:

        minimalloc(); 
        minimalloc(const std::vector<size_t>& block_sizes, const uint64_t memory_pool_size); 

        void* allocate(size_t bytes);
        void deallocate(void* pointer);
        minimalloc_stats get_stats();
};

};



