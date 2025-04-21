#include "thread_cache.h"
#include <iostream>

thread_cache::thread_cache(size_t num_buckets, size_t bucket_cache_capacity){

    for(size_t i = 0; i < num_buckets; i++) {
        bucket_cache_list_.push_back(bucket_cache(bucket_cache_capacity, i));
    }

}

thread_cache::bucket_cache::bucket_cache(size_t capacity, int index) 
    : size_(0),
    capacity_(std::max(MAX_CACHE_CAPACITY, capacity)),
    index_(index)
{}

// return pointer to bucket cache
// used during deallocation
void thread_cache::bucket_cache::free_to_bucket_cache(void* pointer){

    stack[size_] = pointer;
    size_++;
    
}

// get pointer from bucket cache
// used during allocation
void* thread_cache::bucket_cache::alloc_from_bucket_cache(){

    void* p = stack[size_-1];
    size_--;
    std::cout << p << "\n";
    return p;
}
