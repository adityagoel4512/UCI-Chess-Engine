add_compile_options(-mpopcnt -Werror)

add_executable(AdiChess main.cpp board.cpp moveUtils.cpp piece.cpp move.cpp moveGenerator.cpp)