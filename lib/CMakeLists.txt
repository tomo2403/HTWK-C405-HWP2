add_library(lib STATIC lib.cpp
        implementation/CRC.cpp
        implementation/DataStorage.cpp
        implementation/Decoder.cpp
        implementation/Encoder.cpp
        implementation/Logger.cpp
        implementation/NibbleCompressor.cpp
        implementation/DataPacketAssembler.cpp
        implementation/DataPacketDisassembler.cpp
        implementation/ControlPacketAssembler.cpp
        implementation/ControlPacketDisassembler.cpp
        implementation/Timer.cpp
        utilities.cpp
)
