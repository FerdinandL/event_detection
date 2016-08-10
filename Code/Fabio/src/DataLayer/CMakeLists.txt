cmake_minimum_required(VERSION 2.8)
project(datalayer)

find_package(Qt4 REQUIRED)

SET(CMAKE_CXX_COMPILER "/usr/local/bin/g++-4.9")

set(CMAKE_CXX_FLAGS "-std=c++11 -m64 -O3")

include_directories(.)
include(${QT_USE_FILE})


set(srcs
    storage_test.cpp
    DataStore.hpp
    DataStore.cpp
    arena.cpp
    arena.hpp
    mmap.cpp
    mmap.hpp    
)
    
add_executable(storage ${srcs})
target_link_libraries(storage ${QT_LIBRARIES})
