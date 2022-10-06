cmake_minimum_required(VERSION 3.23)
project(lex)

set(CMAKE_CXX_STANDARD 11)

add_executable(lex main.cpp lex.cpp lex.h pre_process.cpp pre_process.h global_def.h symbol.cpp symbol.h)
