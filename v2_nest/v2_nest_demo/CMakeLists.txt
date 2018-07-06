cmake_minimum_required (VERSION 2.8.3)
project (Raspberry-Pi)

set (CMAKE_C_COMPILER gcc)
set (CMAKE_CXX_COMPILER g++)
add_definitions (-std=c++11)

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -pthread")

set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set (CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

#Needed to use Qt
set (CMAKE_AUTOMOC ON)

find_package (Qt5Widgets)

include_directories (${CMAKE_SOURCE_DIR}/include)
add_subdirectory (src/)
