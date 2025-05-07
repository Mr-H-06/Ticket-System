cmake_minimum_required(VERSION 3.28)
project(TicketSystem)

set(CMAKE_CXX_STANDARD 14)

include_directories(vector/src)

add_executable(ticket_system
    src/storage.cpp)

target_include_directories(ticket_system PRIVATE ${CMAKE_SOURCE_DIR}/src/vector)