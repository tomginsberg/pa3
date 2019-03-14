cmake_minimum_required(VERSION 3.6)
project(pa3)
set(CMAKE_CXX_STANDARD 14)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Wall")
set(SOURCE_FILES main.cpp)
add_executable(pa3 ${SOURCE_FILES})

enable_testing()
