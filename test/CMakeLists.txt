add_executable(test main.cpp ComInterface.cpp)

target_link_libraries(test PRIVATE lib)
target_link_libraries(test PRIVATE b15fdrv)