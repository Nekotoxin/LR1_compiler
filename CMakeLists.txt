cmake_minimum_required(VERSION 3.10)
project(compiler)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_USE_RESPONSE_FILE_FOR_INCLUDES 0)

file(GLOB_RECURSE TCC_SRC "src/*.cpp")

find_package(LLVM REQUIRED CONFIG)

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

add_definitions(${LLVM_DEFINITIONS})

llvm_map_components_to_libnames(llvm_libs CORE)

#SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DLLVM_DISABLE_ABI_BREAKING_CHECKS_ENFORCING")

add_executable(tcc src/main.cpp ${TCC_SRC})
target_include_directories(tcc PUBLIC src/lexer src/parser src/common deps ${LLVM_INCLUDE_DIRS})
target_link_libraries(tcc ${llvm_libs})