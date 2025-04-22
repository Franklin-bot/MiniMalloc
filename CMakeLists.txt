cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)

project(minimalloc VERSION 1.0)


add_library(MINIMALLOC_lib OBJECT

    src/minimalloc.cpp
    src/thread_cache.cpp
    src/bucket.cpp

)

add_executable(TEST_exe test/tests.cpp)
target_link_libraries(TEST_exe PRIVATE $<TARGET_OBJECTS:MINIMALLOC_lib>)
