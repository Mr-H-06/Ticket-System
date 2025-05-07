cmake_minimum_required(VERSION 3.28)
project(code)

set(CMAKE_CXX_STANDARD 14)

include_directories(vector/src)

add_executable(code
    src/storage.cpp)

target_include_directories(code PRIVATE ${CMAKE_SOURCE_DIR}/src/vector)