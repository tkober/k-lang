cmake_minimum_required(VERSION 3.15)
project(compiler)

set(CMAKE_CXX_STANDARD 14)

add_executable(compiler main.cpp import/SourceFile.cpp import/SourceFile.h scanner/scanner.h scanner/scanner.cpp)