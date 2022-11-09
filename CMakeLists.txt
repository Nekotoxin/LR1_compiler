cmake_minimum_required(VERSION 3.10)
project(compiler)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_USE_RESPONSE_FILE_FOR_INCLUDES 0)

include_directories(lexer, parser, src/common, deps)

aux_source_directory(src/lexer LEXER_SRC)
aux_source_directory(src/parser PARSER_SRC)
aux_source_directory(src/common COMMON_SRC)
aux_source_directory(deps DEPS_SRC)

add_executable(tcc src/main.cpp ${LEXER_SRC} ${PARSER_SRC} ${COMMON_SRC} ${DEPS_SRC})
