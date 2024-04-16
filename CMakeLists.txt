cmake_minimum_required(VERSION 3.22)
project(BPTree)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)

add_executable(BPTree
        BlockRiver.h
        BPtree.h
        error.h
        main.cpp)
