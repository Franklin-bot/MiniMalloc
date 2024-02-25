# pragma once

#include "pool.h"
#include "thread_cache.h"
#include "bucket.h"
#include <stddef.h>
#include <stdint.h>
#include <iterator>
#include <vector>
#include <algorithm>

class minimalloc {

    private:
        std::unordered_map<size_t, size_t> block_sizes;
        size_t max_block;
        size_t min_block;
        size_t bucket_count;
        std::vector<bucket> pool_buckets;
        std::vector<thread_cache> thread_caches;
        uint8_t* p_global_pool;
        struct thread_cache;

    public:
        constexpr static uint32_t DEFAULT_BLOCK_SIZES[8] = {16, 32, 64, 128, 256, 512, 1024, 2048};
        constexpr static uint8_t DEFAULT_BUCKET_COUNT = 8;
        constexpr static uint64_t DEFAULT_MEMORY_POOL_SIZE = 1<<20;
        constexpr static size_t MAX_CACHE_CAPACITY = 16; 

        minimalloc(std::vector<size_t> block_sizes, uint64_t memory_pool_size); 
        void* allocate(size_t bytes);
        void deallocate(void* pointer);
        size_t get_bucket(size_t n);
};

struct minimalloc::thread_cache{

    private:
        struct bucket_cache;

    public:
        std::vector<thread_cache::bucket_cache> bucket_caches;
        thread_cache(size_t num_buckets, size_t bucket_cache_capacity);  
        void* alloc_from_thread_cache(int i_bucket);
        void free_from_thread_cache(void* pointer);
        void request_from_global_pool(size_t num, size_t block_size);
        void return_to_global_pool(size_t num, size_t block_size);
};

struct minimalloc::thread_cache::bucket_cache {

    bucket_cache(size_t capacity);
    size_t size;
    size_t capacity;
    void* stack[MAX_CACHE_CAPACITY];

};
