cmake_minimum_required(VERSION 3.30)
project(b15f)

set(CMAKE_CXX_STANDARD 20)

add_executable(b15f main.cpp)

target_link_libraries(b15f PRIVATE b15fdrv)