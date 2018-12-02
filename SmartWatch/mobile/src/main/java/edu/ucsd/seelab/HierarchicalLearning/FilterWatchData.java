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

import org.apache.commons.math3.complex.Complex;
import org.apache.commons.math3.linear.Array2DRowRealMatrix;
import org.apache.commons.math3.stat.correlation.PearsonsCorrelation;
import org.apache.commons.math3.transform.DftNormalization;
import org.apache.commons.math3.transform.FastFourierTransformer;
import org.apache.commons.math3.transform.TransformType;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.LinkedList;
import java.util.List;
import java.util.stream.DoubleStream;
import java.util.stream.Stream;

import static edu.ucsd.seelab.HierarchicalLearning.Distribution.watch_corr_xy_accel_mean;
import static edu.ucsd.seelab.HierarchicalLearning.Distribution.watch_corr_xy_accel_variance;
import static edu.ucsd.seelab.HierarchicalLearning.Distribution.watch_corr_xy_gyro_mean;
import static edu.ucsd.seelab.HierarchicalLearning.Distribution.watch_corr_xy_gyro_variance;
import static edu.ucsd.seelab.HierarchicalLearning.Distribution.watch_corr_xz_accel_mean;
import static edu.ucsd.seelab.HierarchicalLearning.Distribution.watch_corr_xz_accel_variance;
import static edu.ucsd.seelab.HierarchicalLearning.Distribution.watch_corr_xz_gyro_mean;
import static edu.ucsd.seelab.HierarchicalLearning.Distribution.watch_corr_xz_gyro_variance;
import static edu.ucsd.seelab.HierarchicalLearning.Distribution.watch_corr_yz_accel_mean;
import static edu.ucsd.seelab.HierarchicalLearning.Distribution.watch_corr_yz_accel_variance;
import static edu.ucsd.seelab.HierarchicalLearning.Distribution.watch_corr_yz_gyro_mean;
import static edu.ucsd.seelab.HierarchicalLearning.Distribution.watch_corr_yz_gyro_variance;
import static edu.ucsd.seelab.HierarchicalLearning.Distribution.watch_energy_accel_mean;
import static edu.ucsd.seelab.HierarchicalLearning.Distribution.watch_energy_accel_variance;
import static edu.ucsd.seelab.HierarchicalLearning.Distribution.watch_energy_gyro_mean;
import static edu.ucsd.seelab.HierarchicalLearning.Distribution.watch_energy_gyro_variance;
import static edu.ucsd.seelab.HierarchicalLearning.Distribution.watch_mean_x_accel_mean;
import static edu.ucsd.seelab.HierarchicalLearning.Distribution.watch_mean_x_accel_variance;
import static edu.ucsd.seelab.HierarchicalLearning.Distribution.watch_mean_x_gyro_mean;
import static edu.ucsd.seelab.HierarchicalLearning.Distribution.watch_mean_x_gyro_variance;
import static edu.ucsd.seelab.HierarchicalLearning.Distribution.watch_mean_y_accel_mean;
import static edu.ucsd.seelab.HierarchicalLearning.Distribution.watch_mean_y_accel_variance;
import static edu.ucsd.seelab.HierarchicalLearning.Distribution.watch_mean_y_gyro_mean;
import static edu.ucsd.seelab.HierarchicalLearning.Distribution.watch_mean_y_gyro_variance;
import static edu.ucsd.seelab.HierarchicalLearning.Distribution.watch_mean_z_accel_mean;
import static edu.ucsd.seelab.HierarchicalLearning.Distribution.watch_mean_z_accel_variance;
import static edu.ucsd.seelab.HierarchicalLearning.Distribution.watch_mean_z_gyro_mean;
import static edu.ucsd.seelab.HierarchicalLearning.Distribution.watch_mean_z_gyro_variance;
import static edu.ucsd.seelab.HierarchicalLearning.Distribution.watch_var_x_accel_mean;
import static edu.ucsd.seelab.HierarchicalLearning.Distribution.watch_var_x_accel_variance;
import static edu.ucsd.seelab.HierarchicalLearning.Distribution.watch_var_x_gyro_mean;
import static edu.ucsd.seelab.HierarchicalLearning.Distribution.watch_var_x_gyro_variance;
import static edu.ucsd.seelab.HierarchicalLearning.Distribution.watch_var_y_accel_mean;
import static edu.ucsd.seelab.HierarchicalLearning.Distribution.watch_var_y_accel_variance;
import static edu.ucsd.seelab.HierarchicalLearning.Distribution.watch_var_y_gyro_mean;
import static edu.ucsd.seelab.HierarchicalLearning.Distribution.watch_var_y_gyro_variance;
import static edu.ucsd.seelab.HierarchicalLearning.Distribution.watch_var_z_accel_mean;
import static edu.ucsd.seelab.HierarchicalLearning.Distribution.watch_var_z_accel_variance;
import static edu.ucsd.seelab.HierarchicalLearning.Distribution.watch_var_z_gyro_mean;
import static edu.ucsd.seelab.HierarchicalLearning.Distribution.watch_var_z_gyro_variance;

public class FilterWatchData {
    class ArrayHistory {
        private final int samplesHistory;
        private LinkedList<double[]> history = new LinkedList<>();
        ArrayHistory(int samples_history) {
            this.samplesHistory = samples_history;
        }

        void pushArray(float[] val) {
            double[] d_val = new double[val.length];
            for(int i=0; i<val.length; i++)
                d_val[i]=val[i];
            pushArray(d_val);
        }

        void pushArray(double[] val) {
            history.addFirst(val);
            while(history.size()>=samplesHistory)
                history.removeLast();
        }

        double[] getMeanAndVar() {
            if (history.isEmpty()) return new double[0];
            double[] acc = new double[history.getFirst().length*2];
            for (double[] val: history)
                for(int i=0; i<val.length; i++)
                    acc[i]+=val[i];
            for(int i=0; i<acc.length/2; i++)
                acc[i]/=history.size();
            for (double[] val: history)
                for(int i=0; i<val.length; i++)
                    acc[i+val.length]+=Math.pow(val[i]-acc[i],2);
            return acc;
        }
        double[] getCorrelation() {
            Array2DRowRealMatrix m = new Array2DRowRealMatrix(3,samplesHistory);
            int col=0;
            for (double[] v : history) {
                m.setColumn(col++, v);
            }
            PearsonsCorrelation c = new PearsonsCorrelation();
            return new double[]{
                    c.correlation(m.getRow(0),m.getRow(1)),
                    c.correlation(m.getRow(0),m.getRow(2)),
                    c.correlation(m.getRow(1),m.getRow(2))
            };
        }
        double getEnergy() {
            FastFourierTransformer fft = new FastFourierTransformer(DftNormalization.STANDARD);
            Array2DRowRealMatrix m = new Array2DRowRealMatrix(3,samplesHistory);
            int col=0;
            for (double[] v : history) {
                m.setColumn(col++, v);
            }
            Complex[] trans = fft.transform(m.getRow(0), TransformType.FORWARD);
            Stream<Complex> ret = Arrays.stream(trans);
            return ret.mapToDouble(in-> in.abs()).sum();
        }
    }

    private final int samplesHistory;
    private ArrayHistory historyAcc;
    private ArrayHistory historyGyro;

    public FilterWatchData(int samples_history) {
        this.samplesHistory = samples_history;
        historyAcc = new ArrayHistory(samplesHistory);
        historyGyro = new ArrayHistory(samplesHistory);
    }

    private double[] Normalize(double[] in) {
        in[0] = (in[0]-watch_corr_xy_accel_mean)/watch_corr_xy_accel_variance;
        in[1] = (in[1]-watch_corr_xz_accel_mean)/watch_corr_xz_accel_variance;
        in[2] = (in[2]-watch_corr_yz_accel_mean)/watch_corr_yz_accel_variance;
        in[3] = (in[3]-watch_mean_x_accel_mean)/watch_mean_x_accel_variance;
        in[4] = (in[4]-watch_mean_y_accel_mean)/watch_mean_y_accel_variance;
        in[5] = (in[5]-watch_mean_z_accel_mean)/watch_mean_z_accel_variance;
        in[6] = (in[6]-watch_var_x_accel_mean)/watch_var_x_accel_variance;
        in[7] = (in[7]-watch_var_y_accel_mean)/watch_var_y_accel_variance;
        in[8] = (in[8]-watch_var_z_accel_mean)/watch_var_z_accel_variance;
        in[9] = (in[9]-watch_corr_xy_gyro_mean)/watch_corr_xy_gyro_variance;
        in[10] = (in[10]-watch_corr_xz_gyro_mean)/watch_corr_xz_gyro_variance;
        in[11] = (in[11]-watch_corr_yz_gyro_mean)/watch_corr_yz_gyro_variance;
        in[12] = (in[12]-watch_mean_x_gyro_mean)/watch_mean_x_gyro_variance;
        in[13] = (in[13]-watch_mean_y_gyro_mean)/watch_mean_y_gyro_variance;
        in[14] = (in[14]-watch_mean_z_gyro_mean)/watch_mean_z_gyro_variance;
        in[15] = (in[15]-watch_var_x_gyro_mean)/watch_var_x_gyro_variance;
        in[16] = (in[16]-watch_var_y_gyro_mean)/watch_var_y_gyro_variance;
        in[17] = (in[17]-watch_var_z_gyro_mean)/watch_var_z_gyro_variance;
        in[18] = (in[18]-watch_energy_accel_mean)/watch_energy_accel_variance;
        in[19] = (in[19]-watch_energy_gyro_mean)/watch_energy_gyro_variance;
        return in;
    }

    public void pushData(float[] acc, float[] gyro){
        historyAcc.pushArray(acc);
        historyGyro.pushArray(gyro);
    }
    public double[] GenerateNNInput(){
        List<Double> ret = new ArrayList<>(20);
        DoubleStream res = DoubleStream.of(historyAcc.getCorrelation());
        res = DoubleStream.concat(res,DoubleStream.of(historyAcc.getMeanAndVar()));
        res = DoubleStream.concat(res,DoubleStream.of(historyGyro.getCorrelation()));
        res = DoubleStream.concat(res,DoubleStream.of(historyGyro.getMeanAndVar()));
        res = DoubleStream.concat(res,DoubleStream.of(historyAcc.getEnergy()));
        res = DoubleStream.concat(res,DoubleStream.of(historyGyro.getEnergy()));
        return Normalize(res.toArray());
    }
}
