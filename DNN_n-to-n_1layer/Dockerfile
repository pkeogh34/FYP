# Start from a base image with C++ compiler
FROM gcc:latest

# Set the working directory in the container
WORKDIR /app

# Copy the entire DNN_n-to-n_1layer directory contents into the container at /app
COPY DNN_n-to-n_1layer/ ./

# Compile your program
RUN g++ -fopenmp -o TrainDNN Train.cxx NN.cxx NN1.cxx NNt.cxx NNt1.cxx DNN1.cxx Layer.cxx Model.cxx


