cmake_minimum_required(VERSION 3.15)
project(compiler)

set(CMAKE_CXX_STANDARD 14)

add_executable(compiler main.cpp import/SourceFile.cpp import/SourceFile.h scanner/scanner.h scanner/scanner.cpp import/SourceFileManager.cpp import/SourceFileManager.h)

add_custom_target(
        generate
        COMMAND cd ${CMAKE_CURRENT_SOURCE_DIR} && /bin/bash generate.sh
)
add_dependencies(compiler generate)