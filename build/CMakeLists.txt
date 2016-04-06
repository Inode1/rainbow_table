cmake_minimum_required (VERSION 2.8)

project (rainbow_table)
file (GLOB HEADER 
	../*.h)
file (GLOB SOURCE
	../*.cpp)
set(CMAKE_CXX_COMPILER "g++-5")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -pthread")
message(STATUS "${CMAKE_CXX_COMPILER}")
add_executable(rainbow_table ${SOURCE} ${HEADER})
