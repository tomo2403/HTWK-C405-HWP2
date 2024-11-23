cmake_minimum_required(VERSION 3.25)
project(b15f)

set(CMAKE_CXX_STANDARD 20)

add_executable(b15f main.cpp
        ../lib/implementation/Encoder.cpp
        ../lib/implementation/Codec.cpp
        ../lib/implementation/Decoder.cpp
        ../lib/implementation/CRC.cpp
        ../lib/implementation/IoManager.cpp
        ../lib/implementation/IoManagerB15F.cpp)

target_link_libraries(b15f PRIVATE b15fdrv)