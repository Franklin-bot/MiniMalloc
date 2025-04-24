#include "bucket.h"
#include <iostream>

namespace minimalloc {
// constructor
bucket::bucket(byte* pbuffer, byte* pbufferend, size_t block_size) :
    pBuffer_(pbuffer),
    pBufferEnd_(pbufferend),
    head_(nullptr),
    size_(0)
    {

    byte* address = pbuffer;

    node head_node;
    head_node.offset_ = 0;
    node* curr = reinterpret_cast<node*>(address);
    *curr = head_node;
    head_ = curr;
    address += block_size;
    size_ = 1;
    
    while (address + block_size < pbufferend) {
        node next_node;
        next_node.offset_ = curr->offset_ + block_size;
        curr->next_ = reinterpret_cast<node*>(address);
        *curr->next_ = next_node;
        curr = curr->next_;
        address += block_size;
        size_++;
    }
};

byte* bucket::allocate(){

    if (size_ == 0) {
        return nullptr;
    }

    byte* p = head_->offset_ + pBuffer_ + sizeof(node);
    head_ = head_->next_;
    size_--;

    return p;

};

void bucket::deallocate(byte* const pointer){

    node new_node;
    new_node.offset_ = pointer - pBuffer_;
    new_node.next_ = head_;
    head_ = reinterpret_cast<node*>(pointer);
    *head_ = new_node;
    size_++;

}

};
