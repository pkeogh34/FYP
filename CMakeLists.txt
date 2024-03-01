cmake_minimum_required(VERSION 3.23)
project(FYP)

set(CMAKE_CXX_STANDARD 23)

include_directories(BRNN_parallel3.1)
include_directories(BRNN_parallel3.1/testing_BRNN)
include_directories(DNN_n-to-n_1layer)

add_executable(pack
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
        DNN_n-to-n_1layer/Train_inc.cxx)

add_executable(FYP main.cpp JsonParse/JsonParse.h JsonParse/JsonParse.cpp DataExploration/DataExploration.h DataExploration/DataExploration.cpp Protein/Protein.cpp Protein/Protein.h)
