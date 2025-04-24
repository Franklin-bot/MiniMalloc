#include "minimalloc.h"
#include "internal/config.h"

// MINIMALLOC
//
namespace minimalloc {

thread_local std::unique_ptr<thread_cache> local_thread_cache_;

// minimalloc default constructor
minimalloc::minimalloc() : minimalloc(DEFAULT_BLOCK_SIZES, DEFAULT_GLOBAL_POOL_SIZE) {} 

// minimalloc constructor
minimalloc::minimalloc(const std::vector<size_t>& block_sizes, uint64_t memory_pool_size)
        : block_sizes_(block_sizes),
        global_pool_size_(memory_pool_size),
        global_pool_ (Pool(memory_pool_size * block_sizes.size(), PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS))
{
    byte* buffer_index = global_pool_.mem;
    for (size_t i = 0; i < block_sizes_.size(); i++) {
        byte* start = buffer_index;
        byte* end   = buffer_index + global_pool_size_;
        block_indices_[block_sizes_[i]] = i;
        global_pool_buckets_.emplace_back(start, end, block_sizes_[i]);
        buffer_index += global_pool_size_;
    }

}

// allocate
void* minimalloc::allocate(size_t bytes){

    // check current thread
    // check get associated thread cache

    const size_t bucket_index = get_bucket(bytes);

    if (!local_thread_cache_){
        local_thread_cache_ = std::make_unique<thread_cache>(block_sizes_.size(), MAX_CACHE_CAPACITY);
        cache_warmup(local_thread_cache_.get());
    }

    thread_cache::bucket_cache& bc = local_thread_cache_->get_bucket_caches()[bucket_index];

    // if current bucket is empty, allocate more
    if (bc.stk.empty()) {
        release_to_bucket_cache(bc);
    }

    // get pointer from bucket
    return static_cast<void*>(bc.alloc_from_bucket_cache());
}


// deallocate
void minimalloc::deallocate(void* pointer){

    // check current thread
    // get associaetd thread cache
    const size_t bucket_index = get_bucket(pointer);

    const thread_id_t curr_thread_id = std::this_thread::get_id();

    thread_cache::bucket_cache& bc = local_thread_cache_->get_bucket_caches()[bucket_index];

    // if bucket is full, empty it
    if (bc.stk.size() == MAX_CACHE_CAPACITY) {
        return_from_bucket_cache(bc);
    }

    // return pointer to the bucket cache
    bc.free_to_bucket_cache(pointer);
}

// warmup the cache
void minimalloc::cache_warmup(thread_cache* tc){

    for (thread_cache::bucket_cache& bc : tc->get_bucket_caches()){
        release_to_bucket_cache(bc);
    }
}

// identify correct bucket based on size you want to allocate
// used in allocation
size_t minimalloc::get_bucket(size_t n) const {

    if (n > *std::max_element(block_sizes_.begin(), block_sizes_.end())) {
        return -1;
    }

    if (n <= *std::min_element(block_sizes_.begin(), block_sizes_.end())) {
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

    const size_t offset = static_cast<byte*>(pointer) - global_pool_.mem;
    return offset/global_pool_size_;

}


// return pointers from bucket cache to global pool bucket
void minimalloc::return_from_bucket_cache(thread_cache::bucket_cache& bc){

    // correct bucket
    bucket& b = global_pool_buckets_[bc.index_];

    for (size_t i = 0; i < CACHE_WARMUP_SIZE; i++){
        byte* p = bc.stk.top();
        bc.stk.pop();
        b.deallocate(p);
    }
}

// release pointers from global pool to bucket_cache
void minimalloc::release_to_bucket_cache(thread_cache::bucket_cache& bc){

    bucket& b = global_pool_buckets_[bc.index_];

    for (size_t i = 0; i < CACHE_WARMUP_SIZE; i++){
       bc.stk.push(b.allocate()); 
    }
}

minimalloc_stats minimalloc::get_stats(){

    if (!local_thread_cache_){
        local_thread_cache_ = std::make_unique<thread_cache>(block_sizes_.size(), MAX_CACHE_CAPACITY);
        cache_warmup(local_thread_cache_.get());
    }

    minimalloc_stats ms;
    ms.block_sizes_ = block_sizes_;

    for (int i = 0; i < block_sizes_.size(); i++){
        ms.thread_cache_bucket_size_.push_back(local_thread_cache_->get_bucket_caches()[i].stk.size());
        ms.thread_cache_bucket_capacity_.push_back(MAX_CACHE_CAPACITY);
    }


    for (const bucket& b : global_pool_buckets_){
        ms.global_pool_bucket_size_.push_back(b.get_size());
    }
    return ms;
}

};
