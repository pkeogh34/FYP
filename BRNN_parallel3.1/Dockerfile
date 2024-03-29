# Start from a base image with C++ compiler
FROM gcc:latest

# Set the working directory in the container
WORKDIR /app

# Copy the entire BRNN_parallel3.1 directory contents into the container at /app
COPY BRNN_parallel3.1/ ./

# Compile your program using the Makefile
RUN make Train
