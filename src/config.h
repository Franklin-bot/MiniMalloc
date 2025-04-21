#pragma once

#include <stdint.h>
#include <stddef.h>

constexpr static size_t DEFAULT_BLOCK_SIZES[8] = {16, 32, 64, 128, 256, 512, 1024, 2048};
constexpr static size_t DEFAULT_BUCKET_COUNT = 8;
constexpr static size_t MAX_CACHE_CAPACITY = 16; 
constexpr static size_t CACHE_WARMUP_SIZE = MAX_CACHE_CAPACITY/2;
