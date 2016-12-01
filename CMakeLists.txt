CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

project(puyo-puyo CXX)

SET(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -O3 -Wall -Wextra")

add_subdirectory(httpi)

target_include_directories(httpi PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

set(puyo_src
    src/grid.cpp
    src/grid.h
    src/game.h
    src/game.cpp
    src/versus.h
    )

add_executable(puyo
    ${puyo_src}
    src/main.cpp
    )


add_executable(puyo_test
    src/grid.cpp
    src/grid.h
    tests/tests.cpp
    )

target_link_libraries(puyo httpi)
target_link_libraries(puyo_test httpi)
