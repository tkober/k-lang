cmake_minimum_required(VERSION 3.15)
project(compiler)

set(CMAKE_CXX_STANDARD 14)

add_executable(compiler main.cpp meta/SourceFile.cpp meta/SourceFile.h)