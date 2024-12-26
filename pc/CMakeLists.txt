add_executable(pc main.cpp
        ComInterface.cpp
        ComInterface.hpp)

add_executable(pc2 main.cpp
        ComInterface.cpp
        ComInterface.hpp)

target_link_libraries(pc PRIVATE lib)
target_link_libraries(pc2 PRIVATE lib)

target_compile_definitions(pc PRIVATE SERIAL_PORT="/dev/tty.usbmodem311201")
target_compile_definitions(pc2 PRIVATE SERIAL_PORT="/dev/tty.usbmodem311101")

target_compile_definitions(pc PRIVATE OUTPUT_RO=0)