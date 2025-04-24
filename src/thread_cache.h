#pragma once

#include <vector>
#include <stack>

#include <stdint.h>

#include "internal/config.h"


namespace minimalloc {
class thread_cache{
public:

    struct bucket_cache{

        byte* alloc_from_bucket_cache();
        void free_to_bucket_cache(void* pointer);

        size_t index_;
        std::stack<byte*> stk{};

        bucket_cache(size_t index) : index_(index) {}
    };

    thread_cache(size_t num_buckets, size_t bucket_cache_capacity);  
    std::vector<thread_cache::bucket_cache>& get_bucket_caches() { return bucket_cache_list_; };

private:
    std::vector<thread_cache::bucket_cache> bucket_cache_list_;

};

};
