#include "../src/minimalloc.h"
#include <vector>
#include <stddef.h>
#include <stdint.h>


int main() {

    std::vector<size_t> memory_block_sizes = {16, 32, 64, 128, 256, 512, 1024, 2048};
    uint64_t memory_pool_size = 1<<20;

    minimalloc m = minimalloc(memory_block_sizes, memory_pool_size);

    void* p = (void*)m.allocate(7);
    void* p1 = (void*)m.allocate(7);
    void* p2 = (void*)m.allocate(7);
    void* p3 = (void*)m.allocate(7);
    void* p4 = (void*)m.allocate(7);
    void* p5 = (void*)m.allocate(7);
    void* p6 = (void*)m.allocate(7);
    void* p7 = (void*)m.allocate(7);
    m.get_stats();

    m.deallocate(p);
    m.deallocate(p1);
    m.deallocate(p2);
    m.deallocate(p3);
    m.deallocate(p4);
    m.deallocate(p5);
    m.deallocate(p6);
    m.deallocate(p7);
    m.deallocate(p);
    m.deallocate(p1);
    m.deallocate(p2);
    m.deallocate(p3);
    m.deallocate(p4);
    m.deallocate(p5);
    m.deallocate(p6);
    m.deallocate(p7);
    m.get_stats();
    m.deallocate(p2);
    m.get_stats();


}
