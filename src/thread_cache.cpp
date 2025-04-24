#include "thread_cache.h"

namespace minimalloc {

thread_cache::thread_cache(size_t num_buckets, size_t bucket_cache_capacity){

    for(size_t i = 0; i < num_buckets; i++) {
        bucket_cache_list_.push_back(bucket_cache(i));
    }

}

// return pointer to bucket cache
// used during deallocation
void thread_cache::bucket_cache::free_to_bucket_cache(void* pointer){
    stk.push(static_cast<byte*>(pointer));
}

// get pointer from bucket cache
// used during allocation
byte* thread_cache::bucket_cache::alloc_from_bucket_cache(){

    byte* p = stk.top();
    stk.pop();
    return p;
}

};
