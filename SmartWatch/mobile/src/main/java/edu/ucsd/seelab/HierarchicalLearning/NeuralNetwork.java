/*
    BSD 3-Clause License

    Copyright (c) 2018, The Regents of the University of California.
    All rights reserved.

    Created by Massimiliano Menarini
    University of California, San Diego
    Department of Computer Science and Engineering

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this
    list of conditions and the following disclaimer.

    * Redistributions in binary form must reproduce the above copyright notice,
    this list of conditions and the following disclaimer in the documentation
    and/or other materials provided with the distribution.

    * Neither the name of the copyright holder nor the names of its
    contributors may be used to endorse or promote products derived from
    this software without specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
    AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
    DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
    FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
    DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
    SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
    CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
    OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
    OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

package edu.ucsd.seelab.HierarchicalLearning;

import org.apache.commons.math3.linear.Array2DRowRealMatrix;
import org.apache.commons.math3.linear.DefaultRealMatrixChangingVisitor;
import org.apache.commons.math3.linear.RealMatrix;
import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

public class NeuralNetwork {
    /*
     * ReLU activation. Read continuously!
     */
    private static void ReLU(double[][] x, int row, int col) {
        for (int i = 0; i < row; ++i) {
            for (int j = 0; j < col; ++j)
			    if(x[i][j] < 0) // do nothing or make it zero
			        x[i][j] = 0;
        }
    }

    /*
     * matrix_sum: m+=v
     */
    private static void matrix_sum(RealMatrix m, double[] v) {
        if (m.getColumnDimension() != v.length)
            return;
        m.walkInOptimizedOrder(new DefaultRealMatrixChangingVisitor() {
            @Override
            public double visit(int row, int column, double value) {
                return value+v[column];
            }
        });
    }

    /*
     * nn: in -> nn -> out
     * "in" is feature_num * batch_num, "out" is classnum*batch_num
     */
    private static double[][] nn(double[] in) {
        if (in.length != ShapeAndWeights.N_IN)
            return null;
        RealMatrix in_m = new Array2DRowRealMatrix(in).transpose();
        RealMatrix nn_w1 = new Array2DRowRealMatrix(ShapeAndWeights.nn_w1);
        RealMatrix nn_w2 = new Array2DRowRealMatrix(ShapeAndWeights.nn_w2);

        RealMatrix nn_l1 = in_m.multiply(nn_w1);
        matrix_sum(nn_l1, ShapeAndWeights.nn_bias_in[0]);
        ReLU(nn_l1.getData(), nn_l1.getRowDimension(), nn_l1.getColumnDimension());

        RealMatrix out_m = nn_l1.multiply(nn_w2);
        matrix_sum(out_m, ShapeAndWeights.nn_bias_out[0]);
        ReLU(out_m.getData(), out_m.getRowDimension(), out_m.getColumnDimension());

        if (out_m.getColumnDimension()!= ShapeAndWeights.N_OUT)
            return null;
        return out_m.getData();
    }

    /*
     * pack: pack out to batch_data.
     * out_col should equal to N_OUT as we read continuously!
     */
    private static String out_text(long timestamp, double[][] out, int out_row, int out_col) throws JSONException {
        JSONObject js = new JSONObject();
        js.put("timestamp", timestamp);
        JSONArray js_arr = new JSONArray(out);
        js.put("data",js_arr);
        return js.toString();
    }


    /*
     * Count must match the number of features times the batch length
     * the varialbes are listed as floats one batch after the other
     */
    public static String Loop(long timestamp, int count, double args[]) {
        if (count != ShapeAndWeights.N_IN * ShapeAndWeights.BATCH_LEN) {
            return "Error input size is wrong";
        }
        // compute nn for this input matrix
        double[][] out = nn(args);
        // pack the result into send batch
        try {
            return out_text(timestamp, out, ShapeAndWeights.BATCH_LEN, ShapeAndWeights.N_OUT);
        } catch (JSONException e) {
            e.printStackTrace();
            return e.getMessage();
        }
    }
}
