cmake_minimum_required(VERSION 3.29)
project(pc)

set(CMAKE_CXX_STANDARD 20)

add_executable(pc main.cpp
        ../lib/implementation/Encoder.cpp
        ../lib/implementation/Codec.cpp
        ../lib/implementation/Decoder.cpp
        ../lib/implementation/CRC.cpp
        ../lib/implementation/IoManager.cpp)
