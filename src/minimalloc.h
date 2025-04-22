# pragma once

#include <vector>
#include <algorithm>
#include <iostream>
#include <unordered_map>
#include <thread>

#include <stddef.h>
#include <stdint.h>

#include "thread_cache.h"
#include "bucket.h"
#include "config.h"


struct minimalloc_stats {
    
    std::vector<size_t> block_sizes_;
    std::vector<size_t> thread_cache_bucket_size_;
    std::vector<size_t> thread_cache_bucket_capacity_;
    std::vector<size_t> global_pool_bucket_size_;


    void print() {

        std::cout << "\n";
        std::cout << "############## STATS BABY ######################################\n";
        std::cout << "----------------------------------------------------------------\n";
        std::cout << "BLOCK SIZES\n";
        for (size_t bs : block_sizes_){
            std::cout << bs << " ";

        }
        std::cout << "\n";
        std::cout << "----------------------------------------------------------------\n";
        std::cout << "LOCAL THREAD CACHE\n";
        for (size_t j = 0; j < block_sizes_.size(); j++){
            std::cout << "bucket cache " << j << " (block: " << block_sizes_[j] << ") size: " << thread_cache_bucket_size_[j] << " capacity: " << thread_cache_bucket_capacity_[j] << "\n";
        }
        std::cout << "\n";
        std::cout << "----------------------------------------------------------------\n";
        std::cout << "GLOBAL POOL BUCKETS" << "\n";
        for(size_t i = 0; i < block_sizes_.size(); i++){
            std::cout << "BUCKET " << block_sizes_[i] << " num blocks: " << global_pool_bucket_size_[i] << "\n";

        }
        std::cout << "----------------------------------------------------------------\n";
        std::cout << "################################################################\n";
        std::cout << std::endl;

    }

};


class minimalloc {

    private:

        size_t max_block_;
        size_t min_block_;
        size_t bucket_count_;
        uint64_t num_blocks_;
        uint64_t memory_pool_size_;

        std::unique_ptr<uint8_t[]> p_global_pool_;
        std::vector<size_t>block_sizes_;
        std::vector<bucket> pool_buckets_;
        std::unordered_map<size_t, size_t> block_indices_;

        void cache_warmup(thread_cache* tc);
        size_t get_bucket(size_t n) const;
        int get_bucket(void* pointer) const;
        void return_from_bucket_cache(size_t n, thread_cache::bucket_cache& b);
        void release_to_bucket_cache(size_t n, thread_cache::bucket_cache& b);

    public:

        minimalloc(const std::vector<size_t>& block_sizes, const uint64_t memory_pool_size); 

        void* allocate(size_t bytes);
        void deallocate(void* pointer);
        minimalloc_stats get_stats();
};






