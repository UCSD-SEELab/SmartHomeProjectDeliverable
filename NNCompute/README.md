# NNCompute
C++ code to compute the NN based on weights headers created by NeuralNetExportEmbedded

Before compiling put the correct shape.h and weights.h (see Readme for NeuralNetExportEmbedded) in the main directory.

Compile then run the generated executable:
* It expects on a single line on the standard input the list of float values separated by comma (they number should match the N_IN in the shape.h file).
* It will return on the standard output a new line terminated json string with the matrix of floats (shape [1][N_OUT]) resulting from the given inputs.
  
