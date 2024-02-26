#include "minimalloc.h"

// minimalloc constructor
minimalloc::minimalloc(std::vector<size_t> block_sizes, uint64_t memory_pool_size){

    this->bucket_count= block_sizes.size();;
    this->max_block = *std::max_element(block_sizes.begin(), block_sizes.end());
    this->min_block = *std::min_element(block_sizes.begin(), block_sizes.end());
    this->memory_pool_size = memory_pool_size;

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
        this->thread_caches.push_back(thread_cache(this->bucket_count, MAX_CACHE_CAPACITY));
    }
}


void* minimalloc::allocate(size_t bytes){

    // check current thread
    // check get associated thread cache

    size_t bucket_index = get_bucket(bytes);
    thread_cache& tc = this->thread_caches[0];

    thread_cache::bucket_cache& bc = tc.bucket_caches[bucket_index];

    if (bc.size == 0) {
        // allocate more
    }

    return bc.alloc_from_thread_cache();
}

void minimalloc::deallocate(void* pointer){

    // check current thread
    // get associaetd thread cache
    size_t bucket_index = get_bucket(pointer);
    thread_cache& tc = this->thread_caches[0];

    thread_cache::bucket_cache& bc = tc.bucket_caches[bucket_index];

    if (bc.size == bc.capacity) {
        // free half
    }
    return bc.free_to_thread_cache(pointer);

}




void minimalloc::return_from_bucket_cache(size_t n, thread_cache::bucket_cache& b){






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

int minimalloc::get_bucket(void* pointer) {

    size_t offset = (uint8_t*)pointer - this->p_global_pool;
    return offset/this->memory_pool_size;

}


void minimalloc::return_from_bucket_cache(size_t n, thread_cache::bucket_cache& bc){
    if (bc.size < n) {
        return;
    }

    // correct bucket
    bucket& b = this->pool_buckets[bc.index];

    for (size_t i = 0; i < n; i++){
        void* p = bc.stack[bc.size-1];
    
    }



}

void minimalloc::release_to_bucket_cache(size_t n, thread_cache::bucket_cache& b){

}

minimalloc::thread_cache::thread_cache(size_t num_buckets, size_t bucket_cache_capacity){

    for(size_t i = 0; i < num_buckets; i++) {
        this->bucket_caches.push_back(bucket_cache(bucket_cache_capacity, i));
    }

}


// BUCKET_CACHE

minimalloc::thread_cache::bucket_cache::bucket_cache(size_t capacity, int index) {
    this->size = 0;
    this->capacity = (capacity > MAX_CACHE_CAPACITY) ? MAX_CACHE_CAPACITY : capacity;
    this->index = index;

    for (size_t i = 0; i < capacity; i++) {
        this->stack[i] = nullptr;
    }
}




// BUCKET

minimalloc::bucket::bucket(uint8_t* const pbuffer, uint8_t* const pbufferend, const size_t block_size){

    // initialize linked list
    this->pBuffer = pbuffer;
    uint8_t* address = pbuffer;

    node head_node;
    head_node.offset = 0;
    node* curr = (node*)address;
    *curr = head_node;
    this->head = curr;
    address += block_size;
    
    while (address + block_size <= pBufferEnd) {
        node next_node;
        next_node.offset = curr->offset + block_size;
        curr->next = (node*)address;
        *curr->next = next_node;
        curr = curr->next;
        address += block_size;
    }

    node last_node;
    last_node.offset = node::Invalid;
    last_node = *((node*)pbufferend);
    curr->next = ((node*)pbufferend);
};

void* minimalloc::bucket::allocate(){

    if (this->head->offset == node::Invalid) {
        return nullptr;
    }

    uint8_t* p = (uint8_t*)this->head;
    this->head = this->head->next;

    return p;

};

void minimalloc::bucket::free(uint8_t* const pointer){

    node new_node;
    new_node.offset = pointer - this->pBuffer;
    new_node.next = this->head;
    this->head = (node*)pointer;
    *this->head = new_node;
}
    
