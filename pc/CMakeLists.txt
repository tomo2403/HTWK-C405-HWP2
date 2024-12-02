add_executable(pc main.cpp
        Serial.cpp
        Serial.hpp
        SerialCommunication.cpp)

target_link_libraries(pc PRIVATE lib)