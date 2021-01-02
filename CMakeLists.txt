cmake_minimum_required(VERSION 3.10)

project(AdiChess)
include(GoogleTest)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_subdirectory(src) 

enable_testing()

add_subdirectory(test)


