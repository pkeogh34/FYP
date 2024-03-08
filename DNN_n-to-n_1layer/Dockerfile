# Use an official base image with C++ compiler
FROM gcc:latest

# Copy the DNN model files into the container
COPY Train.cxx Options.txt ./
COPY ../train.data ./

# Compile DNN model
RUN g++ -fopenmp -o TrainDNN Train.cxx

# Command to run when the container starts
CMD ["./TrainDNN"]

