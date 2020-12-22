cmake_minimum_required(VERSION 3.10)

project(AdiChess)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_COMPILER "g++")
add_compile_options(-mpopcnt)
add_executable(AdiChess main.cpp board.cpp moveGeneration.cpp piece.cpp)