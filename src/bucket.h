#pragma once

#include <stddef.h>
#include <stdint.h>

#include "internal/config.h"

namespace minimalloc {
class bucket {
private:

    class node {
    private:

        size_t offset_{0};
        node* next_{nullptr};
        friend class bucket;

    public:

        static constexpr uint64_t INVALID  = UINT64_MAX;
    };
    

    byte* pBuffer_;
    byte* pBufferEnd_;
    node* head_;
    size_t size_;

public:
    bucket(byte* pBuffer, byte* pBufferEnd, size_t block_size);
    void* allocate();
    void free(uint8_t* pointer);

    size_t get_size() const { return size_; }
    void set_size(size_t size) {size_ = size; }
};

};
