cmake_minimum_required(VERSION 3.23)
project(FYP)

set(CMAKE_C_COMPILER "/opt/homebrew/Cellar/llvm/17.0.6_1/bin/clang")
set(CMAKE_CXX_STANDARD 23)

set(OpenMP_C "${CMAKE_C_COMPILER}")
set(OpenMP_C_FLAGS "-Xpreprocessor -fopenmp -I/opt/homebrew/opt/llvm/include")
set(OpenMP_C_LIB_NAMES "libomp")
set(OpenMP_CXX "${CMAKE_CXX_COMPILER}")
set(OpenMP_CXX_FLAGS "-Xpreprocessor -fopenmp -I/opt/homebrew/opt/llvm/include")
set(OpenMP_CXX_LIB_NAMES "libomp")
set(OpenMP_libomp_LIBRARY "/opt/homebrew/opt/llvm/lib/libomp.dylib")

find_package(OpenMP REQUIRED)

add_executable(FYP main.cpp
        JsonParse/JsonParse.h
        JsonParse/JsonParse.cpp
        DataExploration/DataExploration.h
        DataExploration/DataExploration.cpp
        Protein/Protein.cpp
        Protein/Protein.h
        ChangeFormat.cpp
        ChangeFormat.h)

