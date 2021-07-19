
project(TestDriver)

cmake_minimum_required(VERSION 3.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "-O0")
set(CMAKE_BUILD_TYPE Debug)

# create perf test executable
add_executable(bmperf
               bmperf.cpp)

add_executable(concrete_example
               concrete_example.cpp)

add_executable(simple_example
               simple_example.cpp)

add_executable(min_viable_example
               min_viable_example.cpp)
