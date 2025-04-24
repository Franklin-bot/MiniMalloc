#pragma once

#include <stddef.h>
#include <stdint.h>

#include "internal/config.h"

namespace minimalloc {
class bucket {
private:

    struct node {
        size_t offset_{0};
        node* next_{nullptr};
    };
    

    byte* pBuffer_;
    byte* pBufferEnd_;
    node* head_;
    size_t size_;

public:
    bucket(byte* pBuffer, byte* pBufferEnd, size_t block_size);
    byte* allocate();
    void deallocate(byte* pointer);

    size_t get_size() const { return size_; }
    void set_size(size_t size) {size_ = size; }
};

};
