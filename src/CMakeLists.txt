add_compile_options(-mpopcnt)

add_executable(AdiChess main.cpp board.cpp moveUtils.cpp move.cpp moveGenerator.cpp)