cmake_minimum_required(VERSION 3.29)
project(pc)

set(CMAKE_CXX_STANDARD 20)

add_executable(pc main.cpp
        ../lib/Encoder.cpp
        ../lib/Codec.cpp
        ../lib/Decoder.cpp
        ../lib/CRC.cpp
        ../lib/Observer.cpp
        ../lib/ioManager.cpp
        ../lib/CommandObserver.cpp)
