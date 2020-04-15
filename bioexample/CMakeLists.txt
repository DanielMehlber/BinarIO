#examples
cmake_minimum_required(VERSION 3.0)

project(bioexamples VERSION 1.0)

message("bioexample is being built")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${BIO_EXAMPLE_OUT})

add_executable(bioexamples main.cpp)

target_include_directories(bioexamples PUBLIC ${BIO_INCLUDE_DIR})
target_link_libraries(bioexamples PUBLIC binarIO)