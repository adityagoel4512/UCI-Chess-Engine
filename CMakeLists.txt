cmake_minimum_required(VERSION 3.10)

project(AdiChess)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

file(GLOB source_files CONFIGURE_DEPENDS ${PROJECT_SOURCE_DIR}/src/*.cpp)

add_subdirectory(src) 

enable_testing()

add_subdirectory(tests)


