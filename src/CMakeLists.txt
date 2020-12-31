add_compile_options(-mpopcnt -g -rdynamic)

add_executable(AdiChess main.cpp board.cpp moveUtils.cpp move.cpp moveGenerator.cpp)