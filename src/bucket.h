# pragma once

#include <stdint.h>
#include <stddef.h>

class bucket {

    private:
        const uint8_t* pBuffer;
        const uint8_t* pBufferEnd;

        struct node{
            size_t offset{0};
            static constexpr uint64_t Invalid = UINT64_MAX;
            node* next{nullptr};
        };
        node* head;

    public:
        bucket(uint8_t* const pBuffer, uint8_t* const pBufferEnd, const size_t block_size);
        void* allocate();
        void free(uint8_t* const pointer);
};
