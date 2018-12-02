// BSD 3-Clause License
//
// Copyright (c) 2018, The Regents of the University of California.
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice, this
//   list of conditions and the following disclaimer.
//
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
//
// * Neither the name of the copyright holder nor the names of its
//   contributors may be used to endorse or promote products derived from
//   this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
// OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

// NeuralNetwork.h

#ifndef _NEURALNETWORK_h
#define _NEURALNETWORK_h

#include "application.h"
//#include <stdarg.h>
#include "ArduinoJson.h"
#include "logger.h"
#include "shape.h"

#define MSG_JSON_MAX_LEN 200
#define MSG_JSON_BUF_MAX_LEN 800

class NeuralNetwork {
public:
  // nodes for neural networks
  float nn_in[BATCH_LEN][N_IN], nn_out[BATCH_LEN][N_OUT]; // input and output nodes
  float nn_l1[BATCH_LEN][N_1]; // middle layer nodes in nn


  NeuralNetwork();
  char* Loop(uint32_t timestamp, int count, float args[]);

private:
  char buffer[MSG_JSON_MAX_LEN];
  template<int ROW_M, int COL_M>
  void ReLU(float x[ROW_M][COL_M]);
  template<int ROW_M1, int COL_M1, int ROW_M2, int COL_M2, int ROW_M_OUT, int COL_M_OUT>
  void matrix_multiply(float firstMatrix[ROW_M1][COL_M1], float secondMatrix[ROW_M2][COL_M2], float outputMatrix[ROW_M_OUT][COL_M_OUT]);
  template<int ROW_M, int COL_M>
  void matrix_sum(float m[ROW_M][COL_M], float v[COL_M]);
  template<int ROW_M1, int COL_M1, int ROW_M_OUT, int COL_M_OUT>
  void nn(float inputMatrix[ROW_M1][COL_M1], float outputMatrix[ROW_M_OUT][COL_M_OUT]);
  char* json(uint32_t timestamp, float *out, int out_row, int out_col);
};

#endif
