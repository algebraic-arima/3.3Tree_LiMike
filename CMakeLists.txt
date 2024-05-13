cmake_minimum_required(VERSION 3.22)
project(BPTree)

set(CMAKE_CXX_STANDARD 17)
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -fsanitize=leak -fsanitize=undefined")
set(CMAKE_CXX_FLAGS_RELEASE "-Ofast")
#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O2")

include_directories(.)

add_executable(code
        BNode.h
        BlockRiver.h
        BPtree.h
        DataNode.h
        error.h
        utility.h
        main.cpp
        Buffer.h)
