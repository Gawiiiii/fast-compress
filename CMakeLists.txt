cmake_minimum_required(VERSION 3.22)
project(FastCompress)
set(CMAKE_CXX_STANDARD 17)

include_directories(util)
find_library(zstd libzstd.so)
find_library(lz4 liblz4.so)
link_libraries(pthread jemalloc ${zstd} ${lz4} numa)
add_compile_options(-march=native -fopt-info-vec-optimized)

add_executable(FastCompress main.cpp)