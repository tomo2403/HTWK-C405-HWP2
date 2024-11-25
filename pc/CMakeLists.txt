add_executable(pc main.cpp
        Serial.cpp
        Serial.hpp
        DecoderObserver.cpp
        DecoderObserver.hpp)

target_link_libraries(pc PRIVATE lib)