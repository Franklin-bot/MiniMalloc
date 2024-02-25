#include "minimalloc.h"


minimalloc::minimalloc(std::vector<size_t> block_sizes, uint64_t memory_pool_size){

    this->bucket_count= block_sizes.size();;
    this->max_block = *std::max_element(block_sizes.begin(), block_sizes.end());
    this->min_block = *std::min_element(block_sizes.begin(), block_sizes.end());

    //initialize global pool buckets
    this->p_global_pool = new uint8_t[memory_pool_size*this->bucket_count];
    uint8_t* buffer_index = this->p_global_pool;

    // create pool bucket for each block size
    for (size_t i = 0; i < this->bucket_count; i++) {
        this->block_sizes.insert(block_sizes[i], i);
        this->pool_buckets.push_back(bucket(buffer_index, buffer_index + memory_pool_size, block_sizes[i]));
        buffer_index += memory_pool_size;
    }

    // just one thread cache for now
    for (size_t i = 0; i < 1; i++){
        this->thread_caches.push_back(thread_cache(this->bucket_count, bucket_cache::MAX_CACHE_CAPACITY));
    }
}

void* minimalloc::allocate(size_t bytes){


    // check current thread
    // check get associated thread cache

    size_t bucket_index = get_bucket(bytes);
    thread_cache tc = this->thread_caches[0];
    







}

size_t minimalloc::get_bucket(size_t n) {

    if (n > this->max_block) {
        return -1;
    }

    if (n <= this->min_block) {
        return 0;
    }

    n -= 1;

    n |= n >> 16;
    n |= n >> 8;
    n |= n >> 4;
    n |= n >> 2;
    n |= n >> 1;

    size_t chosen_bucket = n+1;
    return this->block_sizes.at(chosen_bucket);
}
