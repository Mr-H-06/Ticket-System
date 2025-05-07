cmake_minimum_required(VERSION 3.28)
project(src)

set(CMAKE_CXX_STANDARD 14)

include_directories(vector/src)

add_executable(src
    vector/src/exceptions.hpp
    vector/src/utility.hpp
    vector/src/vector.hpp
    storage.cpp)
