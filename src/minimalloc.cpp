#include "minimalloc.h"

// MINIMALLOC

// minimalloc constructor
minimalloc::minimalloc(const std::vector<size_t>& block_sizes, uint64_t memory_pool_size)
        : block_sizes_(block_sizes),
        memory_pool_size_(memory_pool_size),
        bucket_count_(block_sizes.size()), 
        max_block_(*std::max_element(block_sizes.begin(), block_sizes.end())),
        min_block_(*std::min_element(block_sizes.begin(), block_sizes.end())),
        p_global_pool_(std::make_unique<uint8_t[]>(memory_pool_size_)){

    uint8_t* buffer_index = p_global_pool_.get();

    for (size_t i = 0; i < bucket_count_; i++) {
        block_indices_.insert({block_sizes_[i], i});
        pool_buckets_.push_back(bucket(buffer_index, buffer_index + memory_pool_size_, block_sizes[i]));
        buffer_index += memory_pool_size_;
    }

    // just one thread cache for now
    for (size_t i = 0; i < 1; i++){
        thread_caches_.push_back(thread_cache(bucket_count_, MAX_CACHE_CAPACITY));
    }

    //warmup the thread caches
    thread_cache&tc = thread_caches_[0];
    cache_warmup(tc);
}

// allocate
void* minimalloc::allocate(size_t bytes){

    // check current thread
    // check get associated thread cache

    const size_t bucket_index = get_bucket(bytes);
    std::cout << bucket_index << "\n";
    thread_cache& tc = thread_caches_[0];

    thread_cache::bucket_cache& bc = tc.get_bucket_caches()[bucket_index];

    // if current bucket is empty, allocate more
    if (bc.get_size() == 0) {
        release_to_bucket_cache(CACHE_WARMUP_SIZE, bc);
    }

    // get pointer from bucket
    return bc.alloc_from_bucket_cache();
}


// deallocate
void minimalloc::deallocate(void* pointer){

    // check current thread
    // get associaetd thread cache
    const size_t bucket_index = get_bucket(pointer);
    thread_cache& tc = thread_caches_[0];

    thread_cache::bucket_cache& bc = tc.get_bucket_caches()[bucket_index];

    // if bucket is full, empty it
    if (bc.get_size() == bc.get_capacity()) {
        return_from_bucket_cache(CACHE_WARMUP_SIZE, bc);
    }

    // return pointer to the bucket cache
    bc.free_to_bucket_cache(pointer);
}

// warmup the cache
void minimalloc::cache_warmup(thread_cache& tc){

    for (thread_cache::bucket_cache& bc : tc.get_bucket_caches()){
        release_to_bucket_cache(CACHE_WARMUP_SIZE, bc);
    }
}


// identify correct bucket based on size you want to allocate
// used in allocation
size_t minimalloc::get_bucket(size_t n) const {

    if (n > max_block_) {
        return -1;
    }

    if (n <= min_block_) {
        return 0;
    }

    n -= 1;
    n |= n >> 16;
    n |= n >> 8;
    n |= n >> 4;
    n |= n >> 2;
    n |= n >> 1;

    const size_t chosen_bucket = n+1;
    return block_indices_.at(chosen_bucket);
}

// identify correct bucket to return pointer to
// use in deallocation
int minimalloc::get_bucket(void* pointer) const {

    size_t offset = (uint8_t*)pointer - p_global_pool_.get();
    return offset/memory_pool_size_;


}


// return pointers from bucket cache to global pool bucket
void minimalloc::return_from_bucket_cache(size_t n, thread_cache::bucket_cache& bc){
    if (bc.get_size() < n) {
        return;
    }

    // correct bucket
    bucket& b = pool_buckets_[bc.get_index()];

    for (size_t i = 0; i < n; i++){
        void* p = bc.get_stack()[bc.get_size()-1];
        b.free((uint8_t*)p);
        bc.set_size(bc.get_size()-1);
        b.set_size(b.get_size()+1);
    }
}

// release pointers from global pool to bucket_cache
void minimalloc::release_to_bucket_cache(size_t n, thread_cache::bucket_cache& bc){

    bucket& b = pool_buckets_[bc.get_index()];

    for (size_t i = 0; i < n; i++){
       bc.get_stack()[bc.get_size()] = b.allocate(); 
       bc.set_size(bc.get_size()+1);
       b.set_size(b.get_size()-1);
    }
}

// BUCKET


void minimalloc::get_stats(){

    std::cout << "\n";
    std::cout << "############## STATS BABY ######################################\n";
    std::cout << "----------------------------------------------------------------\n";
    std::cout << "BLOCK SIZES\n";

    for (size_t bs : block_sizes_){
        std::cout << bs << " ";

    }
    std::cout << "\n";
    std::cout << "----------------------------------------------------------------\n";

    std::cout << "Num Thread_Caches " << thread_caches_.size() << "\n";

    for(size_t i = 0; i < thread_caches_.size(); i++) {
        std::cout << "THREAD " << i << ": \n";

        for (size_t j = 0; j < bucket_count_; j++){
            std::cout << "bucket cache " << j << " (block: " << block_sizes_[j] << ") size: " << thread_caches_[i].get_bucket_caches()[j].get_size() << " capacity: " << thread_caches_[i].get_bucket_caches()[j].get_capacity() << "\n";
        }
    }

    std::cout << "\n";
    std::cout << "----------------------------------------------------------------\n";
    std::cout << "Num Buckets " << pool_buckets_.size() << "\n";

    for(size_t i = 0; i < pool_buckets_.size(); i++){
        std::cout << "BUCKET " << i << " num blocks: " << pool_buckets_[i].get_size() << "\n";

    }
    std::cout << "----------------------------------------------------------------\n";
    std::cout << "################################################################\n";
    std::cout << std::endl;

}
