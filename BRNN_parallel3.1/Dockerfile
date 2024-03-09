# Start from a base image with C++ compiler
FROM gcc:latest

# Set the working directory in the container
WORKDIR /app

# Copy the entire BRNN_parallel3.1 directory contents into the container at /app
COPY BRNN_parallel3.1/ ./

# Compile your program
RUN g++ -fopenmp -o TrainBRNN Train.cxx NN.cxx NNt.cxx BRNN.cxx Layer.cxx Model.cxx
