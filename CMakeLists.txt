cmake_minimum_required(VERSION 3.10)
project(Ticket_System)

# 设置C++17标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 启用所有警告（推荐）
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")

# 包含目录配置（绝对路径）
include_directories(
        ${CMAKE_SOURCE_DIR}/src
        ${CMAKE_SOURCE_DIR}/src/map
        ${CMAKE_SOURCE_DIR}/src/priority_queue
        ${CMAKE_SOURCE_DIR}/src/vector
)

# 源文件分组（清晰分类）
set(CORE_SOURCES
        src/order_manager.cpp
        src/storage.cpp
        src/time_sys.cpp
        src/train_manager.cpp
        src/user_manager.cpp
)

set(HEADERS
        src/order_manager.hpp
        src/time_sys.hpp
        src/train_manager.hpp
        src/user_manager.hpp
)

set(UTILITIES
        src/map/exceptions.hpp
        src/map/map.hpp
        src/map/utility.hpp
        src/priority_queue/exceptions.hpp
        src/priority_queue/priority_queue.hpp
        src/priority_queue/utility.hpp
        src/vector/exceptions.hpp
        src/vector/utility.hpp
        src/vector/vector.hpp
)

# 生成可执行文件（指定main.cpp为入口）
add_executable(code
        src/main.cpp
        ${CORE_SOURCES}
        ${HEADERS}
        ${UTILITIES}
)