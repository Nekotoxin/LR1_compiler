cmake_minimum_required(VERSION 3.10)
project(compiler)

set(CMAKE_CXX_STANDARD 14)

include_directories(lexer, parser, common)

aux_source_directory(lexer LEXER_SRC)
aux_source_directory(parser PARSER_SRC)
aux_source_directory(common COMMON_SRC)

add_executable(tcc main.cpp ${LEXER_SRC} ${PARSER_SRC} ${COMMON_SRC})
