cmake_minimum_required(VERSION 3.25.1)
project(lib)
project(pc)
project(b15f)
project(test)

set(CMAKE_CXX_STANDARD 20)

add_subdirectory(lib)
add_subdirectory(pc)
add_subdirectory(b15f)
add_subdirectory(test)