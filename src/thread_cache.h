#pragma once

#include <vector>

#include <stdint.h>

#include "config.h"


class thread_cache{
public:

    class bucket_cache{
    public:

        bucket_cache(size_t capacity, int index);
        void* alloc_from_bucket_cache();
        void free_to_bucket_cache(void* pointer);

        size_t get_size() const { return size_; }
        void set_size(size_t size) { size_ = size; }
        size_t get_capacity() const { return capacity_; }
        size_t get_index() const { return index_; }
        void set_index(size_t index) { index_ = index; }

        std::array<void*, MAX_CACHE_CAPACITY>& get_stack() { return stack; }

    private:
        size_t size_;
        size_t capacity_;
        int index_;

        std::array<void*, MAX_CACHE_CAPACITY> stack{};
    };

    thread_cache(size_t num_buckets, size_t bucket_cache_capacity);  
    std::vector<thread_cache::bucket_cache>& get_bucket_caches() { return bucket_cache_list_; };

private:
    std::vector<thread_cache::bucket_cache> bucket_cache_list_;

};
