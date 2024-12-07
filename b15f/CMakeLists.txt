add_executable(b15f main.cpp
        ComInterface.cpp
        ComInterface.hpp
)

target_link_libraries(b15f PRIVATE b15fdrv)
target_link_libraries(b15f PRIVATE lib)