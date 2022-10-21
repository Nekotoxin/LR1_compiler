cmake_minimum_required(VERSION 3.10)
project(compiler)

set(CMAKE_CXX_STANDARD 14)

include_directories(lexer, parser, src/common)

aux_source_directory(src/lexer LEXER_SRC)
aux_source_directory(src/parser PARSER_SRC)
aux_source_directory(src/common COMMON_SRC)

add_executable(tcc src/main.cpp ${LEXER_SRC} ${PARSER_SRC} ${COMMON_SRC})
