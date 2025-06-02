cmake_minimum_required(VERSION 3.28)
project(Ticket_System)

set(CMAKE_CXX_STANDARD 17)

# 添加头文件搜索路径
include_directories(
        ${CMAKE_SOURCE_DIR}/src/vector
        ${CMAKE_SOURCE_DIR}/src
)

# 创建名为storage的静态库
add_library(core STATIC
        src/storage.cpp
        src/order_manager.cpp
        src/user_manager.cpp
        src/train_manager.cpp
)
# 为storage库设置头文件搜索路径
target_include_directories(core PUBLIC
        ${CMAKE_SOURCE_DIR}/src
)

# 创建可执行文件（名称改为code）
add_executable(code
        src/main.cpp
)
# 将storage库链接到可执行文件
target_link_libraries(code PRIVATE storage)