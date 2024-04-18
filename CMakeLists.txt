cmake_minimum_required(VERSION 3.22)
project(BPTree)

set(CMAKE_CXX_STANDARD 17)
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -fsanitize=leak")

include_directories(.)

add_executable(code
        BNode.h
        BlockRiver.h
        BPtree.h
        DataNode.h
        error.h
        utility.h
        main.cpp
)
