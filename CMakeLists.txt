cmake_minimum_required(VERSION 3.6)
project(proj1)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES aws.cpp serverA.cpp serverB.cpp serverC.cpp client.cpp)
add_executable(proj1 ${SOURCE_FILES})