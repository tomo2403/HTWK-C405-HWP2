add_executable(pc main.cpp
        ComInterface.cpp
        ComInterface.hpp)

target_link_libraries(pc PRIVATE lib)