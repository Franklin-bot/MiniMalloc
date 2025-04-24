#pragma once

#include <vector>
#include <thread>

#include <stdint.h>
#include <stddef.h>

const std::vector<size_t> DEFAULT_BLOCK_SIZES{16, 32, 64, 128, 256, 512, 1024, 2048};
constexpr static size_t DEFAULT_BUCKET_COUNT = 8;
constexpr static size_t MAX_CACHE_CAPACITY = 16; 
constexpr static size_t CACHE_WARMUP_SIZE = MAX_CACHE_CAPACITY/2;
constexpr static uint64_t DEFAULT_GLOBAL_POOL_SIZE = 1<<20;

using byte = unsigned char;
using thread_id_t = std::thread::id;
