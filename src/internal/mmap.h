#include <memory>
#include <iostream>

#include <sys/mman.h>

#include "config.h"

namespace minimalloc {
struct Pool {

    byte* mem = nullptr;
    size_t size = 0;

    Pool(uint64_t size, int prot, int flags) {
        mem = static_cast<byte*>(mmap(nullptr, size, prot, flags, -1, 0));
        if (mem == MAP_FAILED) {
            std::cerr << std::strerror(errno) << "\n";
            throw std::bad_alloc();
        }
    }

    // remove copy constructor and assignment
    Pool(const Pool&) = delete;
    Pool& operator=(const Pool&) = delete;

    // move constructor
    Pool(Pool&& other) noexcept
      : mem(other.mem), size(other.size)
    {
        other.mem  = nullptr;
        other.size = 0;
    }

    // Move assignment
    Pool& operator=(Pool&& other) noexcept {
        if (this != &other) {

            if (mem && mem != MAP_FAILED) {
                munmap(mem, size);
            }

            mem = other.mem;
            size = other.size;

            other.mem = nullptr;
            other.size = 0;
        }
        return *this;
    }
    ~Pool() {
        if (mem != nullptr && mem != MAP_FAILED){
            munmap(mem, size);
        } 
    }
};

};
