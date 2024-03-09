# Predicting Protein Disorder Using Evolutionary Scale Modeling

## Overview
This project focuses on the prediction of protein disorder, a crucial aspect of understanding protein functions and interactions in bioinformatics. The unique approach of this study involves using evolutionary scale modeling to enhance prediction accuracy. The project comprises a comparative analysis: initially running models with a modern formatted set of proteins and subsequently applying evolutionary scale modeling for improved predictions.

## Features
- Implementation of machine learning models for protein disorder prediction.
- Use of a modern dataset of proteins for initial predictions.
- Application of evolutionary scale modeling to enhance prediction accuracy.
- Comparative analysis to evaluate the effectiveness of evolutionary scale modeling.

## Getting Started
These instructions will help you set up the project and run it on your local machine for development and testing purposes.

### Prerequisites
- Docker and Docker Compose
- Access to a Unix-like shell environment
- Basic knowledge of command-line operations

### Installation
1. **Clone the Repository**:  
   `git clone https://github.com/pkeogh34/FYP.git`
2. **Navigate to the Project Directory**:  
   `cd FYP`
3. **Build the Docker Containers**:  
   This step compiles the BRNN and DNN models and sets up the necessary environment.  
   `docker-compose up --build`

## Usage
- **Set parameters for the model in the corresponding options file:**

  Each model has a corresponding options file that controls parameters for the model training
   e.g learning rate, num hidden layers etc.
- **Exec into the docker container for the model you wish to train:**
  - `docker exec -it fyp-brnn-1 /bin/bash`
  - `docker exec -it fyp-dnn-1 /bin/bash`
- **Run the command to start the model using the corresponding options file**:  
   - `./TrainBRNN ./Options.txt`
   - `./TrainDNN ./Options_1l.txt`

## Data

## Models

## Results

## Acknowledgements

## Contact Information
- **Student**: Patrick Ross Keogh
- **Supervisor**: Associate Professor Gianluca Pollastri
- **Institution**: University College Dublin
