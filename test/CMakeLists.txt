add_executable(test main.cpp
        Serial.cpp
        Serial.hpp
        SerialCommunication.cpp)

target_link_libraries(test PRIVATE lib)
target_link_libraries(test PRIVATE b15fdrv)