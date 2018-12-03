// NeuralNetwork.h

#ifndef _NEURALNETWORK_h
#define _NEURALNETWORK_h

#include "shape.h"
//#define N_IN 20
//#define N_OUT 2
//#define BATCH_LEN 1
//#define N_1 64

#define MSG_MAX_LEN 1000
#include <iostream>
#include <sstream>
#include <string>
using namespace std;

class NeuralNetwork {
public:
    // nodes for neural networks
  float nn_in[BATCH_LEN][N_IN], nn_out[BATCH_LEN][N_OUT]; // input and output nodes
  float nn_l1[BATCH_LEN][N_1]; //, nn_l2[BATCH_LEN][N_2]; // middle layer nodes in nn
  //float nn_bias_in[BATCH_LEN][N_IN], nn_bias_out[BATCH_LEN][N_OUT]; // in out bias
  //float nn_w1[N_IN][N_1], nn_w2[N_1][N_OUT]; // weights between layers

  NeuralNetwork();
  string Loop(uint32_t timestamp, int count, float args[]);

private:
  void ReLU(float *x, int row, int col) ;
  void matrix_multiply(float *m1, int row_m1, int col_m1,
	    float *m2, int row_m2, int col_m2,
	    float *res, int row_res, int col_res);
  void matrix_sum(float *m, int row_m, int col_m,
	    float *v, int col_v);
  void nn(float *in, int in_row, int in_col,
	    float *out, int out_row, int out_col);
  string out_text(uint32_t timestamp, float *out, int out_row, int out_col);
};

#endif
