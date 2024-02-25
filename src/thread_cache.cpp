#include "thread_cache.h"

thread_cache::thread_cache(size_t num_buckets, size_t bucket_cache_capacity){

    for(size_t i = 0; i < num_buckets; i++) {
        this->bucket_caches.push_back(bucket_cache(bucket_cache_capacity));
    }

};

void* thread_cache::alloc_from_thread_cache(int i_bucket){

    bucket_cache& b = this->bucket_caches[i_bucket];
    
    if (b.size == 0){
        // ask for more from gloabl memory pool
    }

    void* p = b.stack[b.size-1];
    b.size-=1;

    return p;

};

void thread_cache::free_from_thread_cache(void* pointer){

    bucket_cache& b = this->bucket_caches[i_bucket];
    
    if (b.size == 0){
        // ask for more from gloabl memory pool
    }

    void* p = b.stack[b.size-1];
    b.size-=1;

    return p;

};










thread_cache::bucket_cache::bucket_cache(size_t capacity) {
    this->size = 0;
    this->capacity = (capacity > MAX_CACHE_CAPACITY) ? MAX_CACHE_CAPACITY : capacity;

    for (size_t i = 0; i < capacity; i++) {
        this->stack[i] = nullptr;
    }
}

void thread_cache::bucket_cache::set_size(size_t size) {
    this->size = size;
}
void thread_cache::bucket_cache::set_stack(size_t i, void* pointer){
    this->stack[i] = pointer;
}

size_t thread_cache::bucket_cache::get_size() {
    return this->size;
}

void* thread_cache::bucket_cache::get_stack(size_t i){
    return this->stack[i];
}
