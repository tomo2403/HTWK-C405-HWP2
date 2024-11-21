cmake_minimum_required(VERSION 3.29)
project(lib)

set(CMAKE_CXX_STANDARD 20)

add_executable(lib main.cpp
        implementation/Codec.cpp
        implementation/CRC.cpp
        implementation/Encoder.cpp
        implementation/Decoder.cpp)
