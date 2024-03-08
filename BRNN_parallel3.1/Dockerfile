# Use an official base image with C++ compiler
FROM gcc:latest

# Copy the BRNN model files into the container
COPY Train.cxx Options.txt ./
COPY ../train.data ./

# Compile BRNN model
RUN g++ -fopenmp -o TrainBRNN Train.cxx

# Command to run when the container starts
CMD ["./TrainBRNN"]
