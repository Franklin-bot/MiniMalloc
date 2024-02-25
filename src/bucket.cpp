#include "bucket.h"

bucket::bucket(uint8_t* const pbuffer, uint8_t* const pbufferend, const size_t block_size){

    // initialize linked list
    this->pBuffer = pbuffer;
    uint8_t* address = pbuffer;

    node head_node;
    head_node.offset = 0;
    node* curr = (node*)address;
    *curr = head_node;
    this->head = curr;
    address += block_size;
    
    while (address + block_size <= pBufferEnd) {
        node next_node;
        next_node.offset = curr->offset + block_size;
        curr->next = (node*)address;
        *curr->next = next_node;
        curr = curr->next;
        address += block_size;
    }

    node last_node;
    last_node.offset = node::Invalid;
    last_node = *((node*)pbufferend);
    curr->next = ((node*)pbufferend);
};

void* bucket::allocate(){

    if (this->head->offset == node::Invalid) {
        return nullptr;
    }

    uint8_t* p = (uint8_t*)this->head;
    this->head = this->head->next;

    return p;

};

void bucket::free(uint8_t* const pointer){

    node new_node;
    new_node.offset = pointer - this->pBuffer;
    new_node.next = this->head;
    this->head = (node*)pointer;
    *this->head = new_node;
}
    
    


