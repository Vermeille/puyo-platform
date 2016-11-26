CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

project(puyo-puyo CXX)

SET(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -O3 -Wall -Wextra")
set(CUDA_NVCC_FLAGS ${CUDA_NVCC_FLAGS} -O3 --std=c++11)

add_executable(puyo
    src/main.cpp
    src/grid.h
    )
