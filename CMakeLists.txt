CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

project(puyo-puyo CXX)

SET(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -O3 -Wall -Wextra")

add_subdirectory(httpi)

target_include_directories(httpi PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(puyo
    src/main.cpp
    src/grid.h
    )

target_link_libraries(puyo httpi)
