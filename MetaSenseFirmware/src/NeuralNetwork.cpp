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

#include "NeuralNetwork.h"
#include "weigths.h"


/*
 * ReLU activation. Read continuously!
 */
template<int ROW_M, int COL_M>
void NeuralNetwork::ReLU(float x[ROW_M][COL_M])
{
	for (int i = 0; i < ROW_M; ++i)
		for (int j = 0; j < COL_M; ++j)
			if (x[i][j] < 0) x[i][j] = 0; // do nothing or make it zero
}

/*
 * matrix_sum: m+=v
 */
template<int ROW_M, int COL_M>
void NeuralNetwork::matrix_sum(float m[ROW_M][COL_M], float v[COL_M])
{
	for (int i = 0; i < ROW_M; ++i)
		for (int k = 0; k < COL_M; ++k)
			m[i][k] += v[k];
}

/*
 * matrix_multiply: m1*m2=res.
 */
template<int ROW_M1, int COL_M1, int ROW_M2, int COL_M2, int ROW_M_OUT, int COL_M_OUT>
void NeuralNetwork::matrix_multiply(float firstMatrix[ROW_M1][COL_M1], float secondMatrix[ROW_M2][COL_M2], float outputMatrix[ROW_M_OUT][COL_M_OUT])
{
	int i, j, k;
	// Checking correct parameters are specified
	if (COL_M1 != ROW_M2 || ROW_M1 != ROW_M_OUT || COL_M2 != COL_M_OUT)
		return;
	// Initializing output matrix to 0.
	for (i = 0; i < ROW_M_OUT; ++i)
		for (j = 0; j < COL_M_OUT; ++j)
			outputMatrix[i][j] = 0;

	// Multiplying matrix firstMatrix and secondMatrix and storing in array mult.
	for (i = 0; i < ROW_M1; ++i)
		for (j = 0; j < COL_M2; ++j)
			for (k = 0; k < COL_M1; ++k)
				outputMatrix[i][j] += firstMatrix[i][k] * secondMatrix[k][j];
}

/*
 * nn: in -> nn -> out
 * "in" is feature_num * batch_num, "out" is classnum*batch_num
 */
template<int ROW_M1, int COL_M1, int ROW_M_OUT, int COL_M_OUT>
void NeuralNetwork::nn(float in[ROW_M1][COL_M1], float out[ROW_M_OUT][COL_M_OUT]) {
	// check whether the rows of input and output equal
	if (ROW_M1 != ROW_M_OUT || COL_M1 != N_IN || COL_M_OUT != N_OUT)
		return;
	matrix_multiply<ROW_M1, COL_M1, N_IN, N_1, ROW_M1, N_1>(in,
		nn_w1,  nn_l1);
	matrix_sum<ROW_M1, N_1>(nn_l1, nn_bias_in[0]);
	ReLU<ROW_M1, N_1>(nn_l1);

	matrix_multiply<ROW_M1, N_1, N_1, N_OUT, ROW_M_OUT, COL_M_OUT>(nn_l1, nn_w2, out);
	matrix_sum<ROW_M_OUT, COL_M_OUT>(out, nn_bias_out[0]);
}

/*
 * pack: pack out to batch_data.
 * out_col should equal to N_OUT as we read continuously!
 */
char* NeuralNetwork::json(uint32_t timestamp, float *out, int out_row, int out_col) {
  StaticJsonBuffer<MSG_JSON_BUF_MAX_LEN> jsonBuffer;
	JsonObject& root = jsonBuffer.createObject();
	root["timestamp"] = timestamp;
  JsonArray& array = root.createNestedArray("data");
	for (int i = 0; i < out_row; ++i) {
    JsonArray& innerArray = jsonBuffer.createArray();
		for (int j = 0; j < out_col; ++j)
			innerArray.add(out[i*N_OUT+j]);
		array.add(innerArray);
	}
  root.printTo(buffer,sizeof(buffer));
  return buffer;
}

/*
 * constructor
 * TODO: Load from constants
 */
NeuralNetwork::NeuralNetwork() {
  // long randNum;
	// for (int i = 0; i < N_IN; ++i)
	// 	for (int j = 0; j < N_1; ++j) {
	// 		randNum = random(0, MAX_RAND_NUM);
	// 		nn_w1[i][j] = ((float)randNum) / MAX_RAND_NUM;
	// 	}
	// for (int i = 0; i < N_IN; ++i) {
	// 	randNum = random(0, MAX_RAND_NUM);
	// 	nn_bias_in[0][i] =  ((float)randNum) / MAX_RAND_NUM;
	// }
	//
	// // for (int i = 0; i < N_1; ++i)
	// // 	for (int j = 0; j < N_2; ++j) {
	// // 		randNum = random(0, MAX_RAND_NUM);
	// // 		nn_w2[i][j] = ((float)randNum) / MAX_RAND_NUM;
	// // 	}
	// for (int i = 0; i < N_1; ++i)
	// 	for (int j = 0; j < N_OUT; ++j) {
	// 		randNum = random(0, MAX_RAND_NUM);
	// 		nn_w2[i][j] = ((float)randNum) / MAX_RAND_NUM;
	// 	}
	// for (int i = 0; i < N_OUT; ++i) {
	// 	randNum = random(0, MAX_RAND_NUM);
	// 	nn_bias_out[0][i] =  ((float)randNum) / MAX_RAND_NUM;
	// }

}

/*
* Count must match the number of features times the batch length
* the varialbes are listed as floats one batch after the other
*/
char* NeuralNetwork::Loop(uint32_t timestamp, int count, float args[]) {
  if (count!=N_IN*BATCH_LEN) {
		buffer[0] = 0;
		return buffer;
	}
	int j;
  M_MQTT_TRACE("Looping on params\r\n"); Serial.flush();
	int k=0,z=0;
  for(j=0; j<count; j++){
		M_MQTT_TRACE("Got param[%d]\r\n", j); Serial.flush();
    nn_in[k][z++] = args[j];
		if(z==N_IN){
			z=0; k++;
		}
  }
	M_MQTT_TRACE("Closed param list\r\n");
  // compute nn for this input matrix
	nn<BATCH_LEN, N_IN, BATCH_LEN, N_OUT>(nn_in, nn_out);
	//nn((float *)nn_in, BATCH_LEN, N_IN, (float *)nn_out, BATCH_LEN, N_OUT);
  // pack the result into send batch
	return json(timestamp, (float *)nn_out, BATCH_LEN, N_OUT);
}
