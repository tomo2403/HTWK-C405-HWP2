add_library(lib STATIC lib.cpp lib.hpp
        header/CodecCommand.hpp
        header/BlockType.hpp
        header/IDecoderObserver.hpp
        header/ThreadSafeQueue.hpp
        implementation/Logger.cpp
        header/Logger.hpp
        implementation/CRC.cpp
        header/CRC.hpp
        implementation/ControlPanel.cpp
        header/ControlPanel.hpp)