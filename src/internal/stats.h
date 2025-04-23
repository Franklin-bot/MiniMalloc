#include <vector>
#include <iostream>


namespace minimalloc {
struct minimalloc_stats {
    
    std::vector<size_t> block_sizes_;
    std::vector<size_t> thread_cache_bucket_size_;
    std::vector<size_t> thread_cache_bucket_capacity_;
    std::vector<size_t> global_pool_bucket_size_;

    void print() {
        std::cout << "\n############## STATS BABY ######################################\n";
        std::cout << "----------------------------------------------------------------\n";
        std::cout << "BLOCK SIZES\n";
        for (size_t bs : block_sizes_){
            std::cout << bs << " ";

        }
        std::cout << "\n----------------------------------------------------------------\n";
        std::cout << "LOCAL THREAD CACHE\n";
        for (size_t j = 0; j < block_sizes_.size(); j++){
            std::cout << "bucket cache " << j << " (block: " << block_sizes_[j] << ") size: " << thread_cache_bucket_size_[j] << " capacity: " << thread_cache_bucket_capacity_[j] << "\n";
        }
        std::cout << "\n----------------------------------------------------------------\n";
        std::cout << "GLOBAL POOL BUCKETS" << "\n";
        for(size_t i = 0; i < block_sizes_.size(); i++){
            std::cout << "BUCKET " << block_sizes_[i] << " num blocks: " << global_pool_bucket_size_[i] << "\n";

        }
        std::cout << "----------------------------------------------------------------\n";
        std::cout << "################################################################\n\n";
    }

};

};
