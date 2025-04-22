#include "../src/minimalloc.h"
#include <vector>
#include <stddef.h>
#include <stdint.h>
#include <cassert>

const std::vector<size_t> memory_block_sizes = {16, 32, 64, 128, 256, 512, 1024, 2048};
const uint64_t memory_pool_size = 1<<20;

void test_basic_allocate(){

    std::cout << "test 1: basic allocate\n";
    minimalloc m = minimalloc(memory_block_sizes, memory_pool_size);

    void* p;
    for (int i = 0; i < 8; i++){
        p = m.allocate(8);
    }

    minimalloc_stats ms = m.get_stats();
    assert(ms.thread_cache_bucket_size_[0] == 0);

    std::cout << "test 1 passed!\n";

}


void test_basic_deallocate(){

    std::cout << "test 2: basic deallocate\n";
    minimalloc m = minimalloc(memory_block_sizes, memory_pool_size);

    void* p = m.allocate(7);

    minimalloc_stats ms = m.get_stats();
    assert(ms.thread_cache_bucket_size_[0] == CACHE_WARMUP_SIZE-1);

    m.deallocate(p);
    ms = m.get_stats();
    assert(ms.thread_cache_bucket_size_[0] == CACHE_WARMUP_SIZE);

    std::cout << "test 2 passed!\n";

}

void test_release_to_bucket_cache(){

    std::cout << "test 3: release to bucket cache\n";

    minimalloc m = minimalloc(memory_block_sizes, memory_pool_size);

    void* p;
    for (int i = 0; i < 8; i++){
        p = m.allocate(8);
    }

    minimalloc_stats ms = m.get_stats();
    assert(ms.thread_cache_bucket_size_[0] == 0);

    const size_t global_bucket_blocks = ms.global_pool_bucket_size_[0];
    p = m.allocate(8);
    ms = m.get_stats();
    assert(ms.global_pool_bucket_size_[0] = global_bucket_blocks - CACHE_WARMUP_SIZE*2);
    assert(ms.thread_cache_bucket_size_[0] == CACHE_WARMUP_SIZE-1);

    std::cout << "test 3 passed!\n";
}

void test_return_from_bucket_cache(){

    std::cout << "test 4: release from bucket cache\n";

    minimalloc m = minimalloc(memory_block_sizes, memory_pool_size);

    std::vector<void*> pointers{};
    for (int i = 0; i < 17; i++){
        pointers.push_back(m.allocate(8));
    }

    minimalloc_stats ms = m.get_stats();
    assert(ms.thread_cache_bucket_size_[0] == CACHE_WARMUP_SIZE-1);

    const size_t global_bucket_blocks = ms.global_pool_bucket_size_[0];
    assert(ms.global_pool_bucket_size_[0] = global_bucket_blocks - CACHE_WARMUP_SIZE*3);

    // fill cache
    for (int i = 0; i < 9; i++){
        m.deallocate(pointers.back());
        pointers.pop_back();
    }
    ms = m.get_stats();
    ms.print();
    assert(ms.thread_cache_bucket_size_[0] == MAX_CACHE_CAPACITY);

    // overflow cache
    m.deallocate(pointers.back());
    pointers.pop_back();

    ms = m.get_stats();
    ms.print();
    // return CACHE_WARMUP_SIZE to global pool
    assert(ms.thread_cache_bucket_size_[0] == MAX_CACHE_CAPACITY - CACHE_WARMUP_SIZE + 1);

    std::cout << "test 4 passed!\n";
}


int main() {

    std::thread([](){ test_basic_allocate(); }).join();
    std::thread([](){ test_basic_deallocate(); }).join();
    std::thread([](){ test_release_to_bucket_cache(); }).join();
    std::thread([](){ test_return_from_bucket_cache(); }).join();

    return 0;

}
