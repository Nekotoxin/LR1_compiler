cmake_minimum_required(VERSION 3.23)
project(parser)

set(CMAKE_CXX_STANDARD 14)

add_executable(parser main.cpp Parser.cpp Parser.h AST.cpp AST.h PreProcess.cpp PreProcess.h utils.h ParsingTable.cpp ParsingTable.h LR1DFA.cpp LR1DFA.h)
