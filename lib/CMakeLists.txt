cmake_minimum_required(VERSION 3.29)
project(lib)

set(CMAKE_CXX_STANDARD 20)

add_executable(lib main.cpp Codec.cpp CRC.cpp Encoder.cpp Decoder.cpp
        ioManager.cpp
        ioManager.hpp)
