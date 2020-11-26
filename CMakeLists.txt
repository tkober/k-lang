cmake_minimum_required(VERSION 3.17)
set(CMAKE_CXX_STANDARD 14)

project(llvm_tutorial)
add_executable(llvm_tutorial llvm_tutorial.cpp)

project(compiler)
add_executable(compiler compiler/main.cpp compiler/import/SourceFile.cpp compiler/import/SourceFile.h compiler/scanner/scanner.h compiler/scanner/scanner.cpp compiler/import/SourceFileManager.cpp compiler/import/SourceFileManager.h)
add_custom_target(
        generate
        COMMAND cd ${CMAKE_CURRENT_SOURCE_DIR} && /bin/bash generate.sh
)
add_dependencies(compiler generate)