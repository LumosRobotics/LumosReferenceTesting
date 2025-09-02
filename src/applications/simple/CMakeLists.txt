cmake_minimum_required(VERSION 3.14 FATAL_ERROR)

set(CPP_SOURCE_FILES main.cpp)

add_executable(simple ${CPP_SOURCE_FILES})

# target_include_directories(headless_app PRIVATE ${CMAKE_SOURCE_DIR}/external/dvs/src/interfaces/cpp/)
# target_link_libraries(headless_app simulator math)
