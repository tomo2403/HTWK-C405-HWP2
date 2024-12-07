add_executable(b15f main.cpp
B15Receiver.cpp
B15Sender.cpp
Timer.cpp
b15global.cpp)

target_link_libraries(b15f PRIVATE b15fdrv)
target_link_libraries(b15f PRIVATE lib)