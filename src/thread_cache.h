#include <stddef.h>
#include <stdint.h>
#include <vector>

class thread_cache{

    private:
        struct bucket_cache;
        std::vector<thread_cache::bucket_cache> bucket_caches;

    public:
        thread_cache(size_t num_buckets, size_t bucket_cache_capacity);  
        void* alloc_from_thread_cache(int i_bucket);
        void free_from_thread_cache(void* pointer);
        void request_from_global_pool(size_t num, size_t block_size);
        void return_to_global_pool(size_t num, size_t block_size);

};

struct thread_cache::bucket_cache {

    bucket_cache(size_t capacity);
    constexpr static size_t MAX_CACHE_CAPACITY = 16; 
    size_t size;
    size_t capacity;
    void* stack[MAX_CACHE_CAPACITY];
};
