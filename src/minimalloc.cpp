#include "minimalloc.h"

// MINIMALLOC

// minimalloc constructor
minimalloc::minimalloc(std::vector<size_t> block_sizes, uint64_t num_blocks){


    // initialize characteristics
    this->bucket_count= block_sizes.size();
    this->max_block = *std::max_element(block_sizes.begin(), block_sizes.end());
    this->min_block = *std::min_element(block_sizes.begin(), block_sizes.end());
    this->block_sizes = block_sizes;
    this->num_blocks = num_blocks;

    // calculate total pool size
    size_t total_pool_size = 0;
    for (size_t i =0 ; i < bucket_count; i++){
        total_pool_size += block_sizes[i] * num_blocks;
    }
    this->memory_pool_size = total_pool_size;

    //initialize global pool
    this->p_global_pool = new uint8_t[this->memory_pool_size];
    if (this->p_global_pool == nullptr){
        std::cout << "Pool initialization failed\n";
        return;
    }


    // create global pool bucket for each block size
    uint8_t* buffer_index = this->p_global_pool;

    for (size_t i = 0; i < this->bucket_count; i++) {
        this->block_indices.insert({block_sizes[i], i});
        size_t bucket_size = this->block_sizes[i] * this->num_blocks;
        this->pool_buckets.push_back(bucket(buffer_index, buffer_index + bucket_size, block_sizes[i]));
        buffer_index += bucket_size;
    }

    // just one thread cache for now
    for (size_t i = 0; i < 1; i++){
        this->thread_caches.push_back(thread_cache(this->bucket_count, MAX_CACHE_CAPACITY));
    }

    //warmup the thread caches
    thread_cache&tc = this->thread_caches[0];
    cache_warmup(tc);
}


// destructor
minimalloc::~minimalloc(){
    free(this->p_global_pool);
    std::cout << "deleted\n";
}

// allocate
void* minimalloc::allocate(size_t bytes){

    // check current thread
    // check get associated thread cache

    size_t bucket_index = get_bucket(bytes);
    thread_cache& tc = this->thread_caches[0];

    thread_cache::bucket_cache& bc = tc.bucket_caches[bucket_index];

    // if current bucket is empty, allocate more
    if (bc.size == 0) {
        release_to_bucket_cache(CACHE_WARMUP_SIZE, bc);
    }

    // get pointer from bucket
    return bc.alloc_from_bucket_cache();
}


// deallocate
void minimalloc::deallocate(void* pointer){

    // check current thread
    // get associaetd thread cache
    size_t bucket_index = get_bucket(pointer);
    thread_cache& tc = this->thread_caches[0];

    thread_cache::bucket_cache& bc = tc.bucket_caches[bucket_index];

    // if bucket is full, empty it
    if (bc.size == bc.capacity) {
        return_from_bucket_cache(bc.capacity/3, bc);
    }
    // return pointer to the bucket cache
    return bc.free_to_bucket_cache(pointer);
}


// warmup the cache
void minimalloc::cache_warmup(thread_cache& tc){

    for(size_t i = 0; i < this->bucket_count; i++){

        thread_cache::bucket_cache&bc = tc.bucket_caches[i];
        release_to_bucket_cache(CACHE_WARMUP_SIZE, bc);
    }
}


// identify correct bucket based on size you want to allocate
// used in allocation
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
    return this->block_indices.at(chosen_bucket);
}

// identify correct bucket to return pointer to
// use in deallocation
int minimalloc::get_bucket(void* pointer) {

    size_t offset = (uint8_t*)pointer - this->p_global_pool;
    return offset/this->memory_pool_size;

}


// return pointers from bucket cache to global pool bucket
void minimalloc::return_from_bucket_cache(size_t n, thread_cache::bucket_cache& bc){
    if (bc.size < n) {
        return;
    }

    // correct bucket
    bucket& b = this->pool_buckets[bc.index];

    for (size_t i = 0; i < n; i++){
        void* p = bc.stack[bc.size-1];
        b.free((uint8_t*)p);
        bc.size--;
    }
}

// release pointers from global pool to bucket_cache
void minimalloc::release_to_bucket_cache(size_t n, thread_cache::bucket_cache& bc){

    bucket& b = this->pool_buckets[bc.index];

    for (size_t i = 0; i < n; i++){
       bc.stack[bc.size] = b.allocate(); 
       bc.size++;
    }
}


// THREAD_CACHE

// thread_cache constructor
minimalloc::thread_cache::thread_cache(size_t num_buckets, size_t bucket_cache_capacity){

    for(size_t i = 0; i < num_buckets; i++) {
        this->bucket_caches.push_back(bucket_cache(bucket_cache_capacity, i));
    }

}


// BUCKET_CACHE

//constructor
minimalloc::thread_cache::bucket_cache::bucket_cache(size_t capacity, int index) {
    this->size = 0;
    this->capacity = (capacity > MAX_CACHE_CAPACITY) ? MAX_CACHE_CAPACITY : capacity;
    this->index = index;

    for (size_t i = 0; i < capacity; i++) {
        this->stack[i] = nullptr;
    }
}


// return pointer to bucket cache
// used during deallocation
void minimalloc::thread_cache::bucket_cache::free_to_bucket_cache(void* pointer){
}

// get pointer from bucket cache
// used during allocation
void* minimalloc::thread_cache::bucket_cache::alloc_from_bucket_cache(){

    void* p = this->stack[this->size-1];
    this->size --;
    return p;
}





// BUCKET

// constructor
minimalloc::bucket::bucket(uint8_t* pbuffer, uint8_t* pbufferend, size_t block_size){

    // initialize linked list
    this->pBuffer = pbuffer;
    uint8_t* address = pbuffer;

    node head_node;
    head_node.offset = 0;
    node* curr = (node*)address;
    *curr = head_node;
    this->head = curr;
    address += block_size;
    
    while (address + block_size < pbufferend) {
        node next_node;
        next_node.offset = curr->offset + block_size;
        curr->next = (node*)address;
        *curr->next = next_node;
        curr = curr->next;
        address += block_size;
    }

    node last_node;
    last_node.offset = node::Invalid;
    *(node*)address = last_node;
};

// allocate from global pool bucket
void* minimalloc::bucket::allocate(){

    if (this->head->offset == node::Invalid) {
        return nullptr;
    }

    uint8_t* p = (uint8_t*)this->head;
    this->head = this->head->next;

    return p;

};

// free from global pool bucket
void minimalloc::bucket::free(uint8_t* const pointer){

    node new_node;
    new_node.offset = pointer - this->pBuffer;
    new_node.next = this->head;
    this->head = (node*)pointer;
    *this->head = new_node;
}
    


// get state of allocator
void minimalloc::get_stats(){

    std::cout << "----------------------------------------------------------------\n";
    std::cout << "BLOCK SIZES\n";
    for (size_t i = 0; i < this->bucket_count; i++){
        std::cout << this->block_sizes[i] << " ";
    }
    std::cout << "\n";
    std::cout << "----------------------------------------------------------------\n";

    std::cout << "Num Thread_Caches " << this->thread_caches.size() << "\n";

    for(size_t i = 0; i < this->thread_caches.size(); i++) {
        std::cout << "THREAD " << i << ": \n";

        for (size_t j = 0; j < this->bucket_count; j++){
            std::cout << "bucket cache " << j << " (block: " << this->block_sizes[j] << ") size: " << thread_caches[i].bucket_caches[j].size << " capacity: " << thread_caches[i].bucket_caches[j].capacity << "\n";
        }
    }

    std::cout << "\n";
    std::cout << "----------------------------------------------------------------\n";
    std::cout << "Num Buckets " << this->pool_buckets.size() << "\n";

    for(size_t i = 0; i < this->pool_buckets.size(); i++){
        std::cout << "BUCKET " << i << "\n";

    }
    std::cout << "----------------------------------------------------------------\n";

}
