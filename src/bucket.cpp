#include "bucket.h"

// constructor
bucket::bucket(uint8_t* pbuffer, uint8_t* pbufferend, size_t block_size){

    // initialize linked list
    pBuffer_ = pbuffer;
    uint8_t* address = pbuffer;

    node head_node;
    head_node.offset_ = 0;
    node* curr = (node*)address;
    *curr = head_node;
    head_ = curr;
    address += block_size;
    size_ = 1;
    
    while (address + block_size < pbufferend) {
        node next_node;
        next_node.offset_ = curr->offset_ + block_size;
        curr->next_ = (node*)address;
        *curr->next_ = next_node;
        curr = curr->next_;
        address += block_size;
        size_++;
    }

    node last_node;
    last_node.offset_ = node::INVALID;
    *(node*)address = last_node;
    size_++;
};

// allocate from global pool bucket
void* bucket::allocate(){

    if (head_->offset_ == node::INVALID) {
        return nullptr;
    }

    uint8_t* p = (uint8_t*)head_;
    head_ = head_->next_;
    size_--;

    return p;

};

// free from global pool bucket
void bucket::free(uint8_t* const pointer){

    node new_node;
    new_node.offset_ = pointer - pBuffer_;
    new_node.next_ = head_;
    head_ = (node*)pointer;
    *head_ = new_node;
    size_++;

}
