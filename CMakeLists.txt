cmake_minimum_required(VERSION 3.0)

project(movie_manager)

add_executable(movie_manager main/main.c implementations/dados.c implementations/idxPrimario.c implementations/idxSecundario.c)

target_include_directories(movie_manager PUBLIC includes)


