# pragma once

#include "pool.h"
#include <stddef.h>
#include <stdint.h>
#include <iterator>
#include <vector>
#include <algorithm>

class minimalloc {

    private:

        struct thread_cache{

                struct bucket_cache;
                std::vector<thread_cache::bucket_cache> bucket_caches;
                thread_cache(size_t num_buckets, size_t bucket_cache_capacity);  
        };

        struct bucket {
                struct node;
                bucket(uint8_t* const pBuffer, uint8_t* const pBufferEnd, const size_t block_size);
                void* allocate();
                void free(uint8_t* const pointer);
                const uint8_t* pBuffer;
                const uint8_t* pBufferEnd;
                node* head;
        };

        std::unordered_map<size_t, size_t> block_sizes;
        size_t max_block;
        size_t min_block;
        size_t bucket_count;
        uint64_t memory_pool_size;
        std::vector<bucket> pool_buckets;
        std::vector<thread_cache> thread_caches;
        uint8_t* p_global_pool;

    public:
        constexpr static uint32_t DEFAULT_BLOCK_SIZES[8] = {16, 32, 64, 128, 256, 512, 1024, 2048};
        constexpr static uint8_t DEFAULT_BUCKET_COUNT = 8;
        constexpr static uint64_t DEFAULT_MEMORY_POOL_SIZE = 1<<20;
        constexpr static uint8_t MAX_CACHE_CAPACITY = 16; 

        minimalloc(std::vector<size_t> block_sizes, uint64_t memory_pool_size); 
        void* allocate(size_t bytes);
        void deallocate(void* pointer);
        size_t get_bucket(size_t n);
        int get_bucket(void* pointer);
        void return_from_bucket_cache(size_t n, thread_cache::bucket_cache& b);
        void release_to_bucket_cache(size_t n, thread_cache::bucket_cache& b);

};

struct minimalloc::thread_cache::bucket_cache {

    bucket_cache(size_t capacity, int index);
    size_t size;
    size_t capacity;
    void* stack[MAX_CACHE_CAPACITY];
    int index;

};


struct minimalloc::bucket::node{
    size_t offset{0};
    static constexpr uint64_t Invalid = UINT64_MAX;
    node* next{nullptr};
};



