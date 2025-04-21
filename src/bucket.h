#include <stddef.h>
#include <stdint.h>

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
    

    uint8_t* pBuffer_;
    uint8_t* pBufferEnd_;
    node* head_;
    size_t size_;

public:
    bucket(uint8_t* pBuffer, uint8_t* pBufferEnd, size_t block_size);
    void* allocate();
    void free(uint8_t* pointer);

    size_t get_size() { return size_; }
    void set_size(size_t size) {size_ = size; }
};
