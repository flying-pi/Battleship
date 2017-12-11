cmake_minimum_required(VERSION 3.6)
project(Battleship)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -pthread")

set(SOURCE_FILES main.cpp)
add_executable(Battleship ${SOURCE_FILES})