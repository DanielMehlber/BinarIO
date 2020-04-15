###############################################
# Variables:
#	- BIO_LIB_OUT - Where to place .lib file
#	- BIO_DLL_OUT - Where to place .dll file
#	- BIO_EXAMPLE_OUT - Where to place example
###############################################

cmake_minimum_required(VERSION 3.0)

project(binarIO VERSION 1.0)

option(BIO_BUILD_EXAMPLES "Build the examples" ON)

set(BIO_INCLUDE_DIR ${CMAKE_CURRENT_LIST_DIR})

if( NOT DEFINED BIO_LIB_OUT)
	set(BIO_LIBS_OUT ${CMAKE_CURRENT_BINARY_DIR}/lib)
endif()

if( NOT DEFINED BIO_DLL_OUT)
	set(BIO_DLL_OUT ${CMAKE_CURRENT_BINARY_DIR}/bin)
endif()

if( NOT DEFINED BIO_EXAMPLE_OUT)
	set(BIO_EXAMPLE_OUT ${CMAKE_CURRENT_BINARY_DIR}/bin)
endif()


set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${BIO_LIBS_OUT})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${BIO_DLL_OUT})


if(NOT DEFINED BUILD_SHARED_LIBS)
	set(BUILD_SHARED_LIBS ON)
endif()

add_library(binarIO binario.cpp)

include_directories(myexcept)

if(BIO_BUILD_EXAMPLES)
	add_subdirectory(bioexample)
endif()

