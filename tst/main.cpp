#include "../src/minimalloc.h"
#include <vector>
#include <stddef.h>
#include <stdint.h>


int main() {

    std::vector<size_t> memory_block_sizes = {16, 32, 64, 128, 256, 512, 1024, 2048};
    uint64_t memory_pool_size = 1<<8;

    minimalloc m = minimalloc(memory_block_sizes, 100);

    void* p = (void*)m.allocate(7);
    std::cout << "pointer = " << p << "\n";
    p = (void*)m.allocate(7);
    std::cout << "pointer = " << p << "\n";
    p = (void*)m.allocate(7);
    std::cout << "pointer = " << p << "\n";
    p = (void*)m.allocate(7);
    std::cout << "pointer = " << p << "\n";
    p = (void*)m.allocate(7);
    std::cout << "pointer = " << p << "\n";
    p = (void*)m.allocate(7);
    std::cout << "pointer = " << p << "\n";
    p = (void*)m.allocate(7);
    std::cout << "pointer = " << p << "\n";
    p = (void*)m.allocate(7);
    std::cout << "pointer = " << p << "\n";
    m.get_stats();
    void* p2 = (void*)m.allocate(7);
    std::cout << "pointer = " << p2 << "\n";
    m.get_stats();


}
