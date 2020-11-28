cmake_minimum_required(VERSION 3.17)
set(CMAKE_CXX_STANDARD 14)

set(BUILD_ARCH "-m64")
set(TARGET_64 ON)

# Import LLVM
find_package(LLVM REQUIRED CONFIG)

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

include_directories(${LLVM_INCLUDE_DIRS})
include_directories("/usr/local/lib/cmake/llvm")
add_definitions(${LLVM_DEFINITIONS})

project(llvm_tutorial)
add_executable(llvm_tutorial llvm_tutorial.cpp)

llvm_map_components_to_libnames(
        Analysis
        Core
        ExecutionEngine
        InstCombine
        Object OrcJIT
        RuntimeDyld
        ScalarOpts
        Support
        native
)

# Link against LLVM libraries
target_link_libraries(llvm_tutorial ${llvm_libs})


########################################################################################################################

#project(compiler)
#add_executable(compiler compiler/main.cpp compiler/import/SourceFile.cpp compiler/import/SourceFile.h compiler/scanner/scanner.h compiler/scanner/scanner.cpp compiler/import/SourceFileManager.cpp compiler/import/SourceFileManager.h)
#add_custom_target(
#        generate
#        COMMAND cd ${CMAKE_CURRENT_SOURCE_DIR} && /bin/bash generate.sh
#)
#add_dependencies(compiler generate)