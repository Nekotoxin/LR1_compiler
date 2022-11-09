cmake_minimum_required(VERSION 3.10)
project(compiler)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_USE_RESPONSE_FILE_FOR_INCLUDES 0)

file(GLOB_RECURSE TCC_SRC "src/*.cpp")

add_executable(tcc src/main.cpp ${TCC_SRC})
target_include_directories(tcc PUBLIC src/lexer src/parser src/common deps)