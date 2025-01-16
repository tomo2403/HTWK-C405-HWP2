cmake_minimum_required(VERSION 3.25.1)
project(V6)
set(CMAKE_CXX_STANDARD 23)

add_executable(V6 main.cpp)

target_link_libraries(V6 PRIVATE b15fdrv)