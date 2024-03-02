cmake_minimum_required(VERSION 3.23)
project(FYP)

set(CMAKE_C_COMPILER "/opt/homebrew/Cellar/llvm/17.0.6_1/bin/clang")
set(CMAKE_CXX_COMPILER "/opt/homebrew/Cellar/llvm/17.0.6_1/bin/clang++")

set(OpenMP_C "${CMAKE_C_COMPILER}")
set(OpenMP_C_FLAGS "-Xpreprocessor -fopenmp -I/opt/homebrew/opt/llvm/include")
set(OpenMP_C_LIB_NAMES "libomp" "libgomp" "libiomp5")
set(OpenMP_CXX "${CMAKE_CXX_COMPILER}")
set(OpenMP_CXX_FLAGS "-Xpreprocessor -fopenmp -I/opt/homebrew/opt/llvm/include")
set(OpenMP_CXX_LIB_NAMES "libomp" "libgomp" "libiomp5")
set(OpenMP_libomp_LIBRARY "/opt/homebrew/opt/llvm/lib/libomp.dylib")
set(OpenMP_libgomp_LIBRARY "/opt/homebrew/opt/llvm/lib/libgomp.dylib")
set(OpenMP_libiomp5_LIBRARY "/opt/homebrew/opt/llvm/lib/libiomp5.dylib")

find_package(OpenMP REQUIRED)

include_directories(BRNN_parallel3.1)
include_directories(BRNN_parallel3.1/testing_BRNN)
include_directories(DNN_n-to-n_1layer)

add_executable(model
        BRNN_parallel3.1/testing_BRNN/BRNN.cxx
        BRNN_parallel3.1/testing_BRNN/BRNN.h
        BRNN_parallel3.1/testing_BRNN/BRNNr.cxx
        BRNN_parallel3.1/testing_BRNN/BRNNr.h
        BRNN_parallel3.1/testing_BRNN/Layer.cxx
        BRNN_parallel3.1/testing_BRNN/Layer.h
        BRNN_parallel3.1/testing_BRNN/MModel.cxx
        BRNN_parallel3.1/testing_BRNN/MModel.h
        BRNN_parallel3.1/testing_BRNN/Model.cxx
        BRNN_parallel3.1/testing_BRNN/Model.h
        BRNN_parallel3.1/testing_BRNN/NN.cxx
        BRNN_parallel3.1/testing_BRNN/NN.h
        BRNN_parallel3.1/testing_BRNN/NNr.cxx
        BRNN_parallel3.1/testing_BRNN/NNr.h
        BRNN_parallel3.1/testing_BRNN/NNt.cxx
        BRNN_parallel3.1/testing_BRNN/NNt.h
        BRNN_parallel3.1/testing_BRNN/Predict.cxx
        BRNN_parallel3.1/testing_BRNN/Predict_thresholds.cxx
        BRNN_parallel3.1/testing_BRNN/Sequence.h
        BRNN_parallel3.1/testing_BRNN/Train.cxx
        BRNN_parallel3.1/BRNN.cxx
        BRNN_parallel3.1/BRNN.h
        BRNN_parallel3.1/Layer.cxx
        BRNN_parallel3.1/Layer.h
        BRNN_parallel3.1/Model.cxx
        BRNN_parallel3.1/Model.h
        BRNN_parallel3.1/NN.cxx
        BRNN_parallel3.1/NN.h
        BRNN_parallel3.1/NNt.cxx
        BRNN_parallel3.1/NNt.h
        BRNN_parallel3.1/Sequence.h
        BRNN_parallel3.1/Train.cxx
        DNN_n-to-n_1layer/DNN1.cxx
        DNN_n-to-n_1layer/DNN1.h
        DNN_n-to-n_1layer/Layer.cxx
        DNN_n-to-n_1layer/Layer.h
        DNN_n-to-n_1layer/Model.cxx
        DNN_n-to-n_1layer/Model.h
        DNN_n-to-n_1layer/NN.cxx
        DNN_n-to-n_1layer/NN.h
        DNN_n-to-n_1layer/NN1.cxx
        DNN_n-to-n_1layer/NN1.h
        DNN_n-to-n_1layer/NNt.cxx
        DNN_n-to-n_1layer/NNt.h
        DNN_n-to-n_1layer/NNt1.cxx
        DNN_n-to-n_1layer/NNt1.h
        DNN_n-to-n_1layer/Sequence.h
        DNN_n-to-n_1layer/Train.cxx
        DNN_n-to-n_1layer/Train_inc.cxx
        )

add_executable(FYP main.cpp
        JsonParse/JsonParse.h
        JsonParse/JsonParse.cpp
        DataExploration/DataExploration.h
        DataExploration/DataExploration.cpp
        Protein/Protein.cpp
        Protein/Protein.h
        ChangeFormat.cpp
        ChangeFormat.h)

if(OpenMP_CXX_FOUND OR OpenMP_C_FOUND)
    target_link_libraries(model PUBLIC OpenMP::OpenMP_CXX OpenMP::OpenMP_C)
endif()