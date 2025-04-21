### Minimalloc - custom heap allocator
because malloc is too slow

* slab allocator optimized for rapid alloc/dealloc of small objects
* memory managed in fixed sized blocks
* blocks cached per thread to limit concurrency overhead
* block distribution between threads handled in global pool
* 200+% faster than malloc

WIP Todo:
1. block alignment
2. mmap
