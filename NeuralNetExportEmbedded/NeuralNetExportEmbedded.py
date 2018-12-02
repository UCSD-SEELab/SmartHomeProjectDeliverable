'''
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
'''

import re
import os

def readparams(filename):
    with open(filename, 'r') as fp:
        lines = fp.readlines()
        filtered = filter(lambda x:  not re.match(r'^\s*$', x), lines)
        return list(filtered)

def create_arrays_def_CPP(var_name, lines):
    values_str = ','.join(lines)
    var_str = "float {0}[{1}] = {{{2}}};".format(var_name, len(lines), values_str)
    return var_str;

def count_matrix(lines):
    cur_count = -1
    for line in lines:
        count = len(line.split())
        if cur_count==-1:
            cur_count = count
        if cur_count!=count:
            raise ValueError( "all lines in a matrix must have the same length" )
    return len(lines), cur_count

def create_matrix_def_Java(var_name, lines):
    row, cols = count_matrix(lines)
    cols_arr = []
    for line in lines:
        cols_arr.append('{'+','.join(line.split())+'}')
    values_str = ','.join(cols_arr)
    var_str = "public static final double {0}[][] = {{{1}}};".format(var_name, values_str)
    return var_str;

def create_consts_def_Java(name, size):
    return "public static final int {0} = {1};".format(name, size)

def create_matrix_def_CPP(var_name, lines):
    row, cols = count_matrix(lines)
    cols_arr = []
    for line in lines:
        cols_arr.append('{'+','.join(line.split())+'}')
    values_str = ','.join(cols_arr)
    var_str = "float {0}[{1}][{2}] = {{{3}}};".format(var_name, row, cols, values_str)
    return var_str;

def create_consts_def_CPP(name, size):
    return "#define {0} {1}".format(name, size)

def create_distribution_file(path, name, filename):
    file_in = readparams(filename)
    with open_file_ensure_exists_dir(os.path.join(path, re.sub('\.csv$', '', name),'distribution.h'), 'w') as cfp:
        with open_file_ensure_exists_dir(os.path.join(path, re.sub('\.csv$', '', name),'Distribution.java'), 'w') as jfp:
            cfp.write("#ifndef _DISTRIBUTION_h\n")
            cfp.write("#define _DISTRIBUTION_h\n")
            jfp.write("package edu.ucsd.seelab.HierarchicalLearning;\n")
            jfp.write("public class Distribution {\n")
            for line in file_in:
                line_vals = [x.strip() for x in line.split(',')]
                if(line_vals[0]!=""):
                    cfp.write(create_consts_def_CPP(line_vals[0]+'_mean', line_vals[1])+"\n")
                    cfp.write(create_consts_def_CPP(line_vals[0]+'_variance', line_vals[2])+"\n")
                    jfp.write("    "+"public static final double {0} = {1};".format(line_vals[0]+'_mean', line_vals[1])+"\n")
                    jfp.write("    "+"public static final double {0} = {1};".format(line_vals[0]+'_variance', line_vals[2])+"\n")
            cfp.write("#endif\n")
            jfp.write("}\n")

def open_file_ensure_exists_dir(filename , open_mode):
    if not os.path.exists(os.path.dirname(filename)):
        os.makedirs(os.path.dirname(filename))
    return open(filename, open_mode)

def create_header_file(w_filename, s_filename, javaClassName, path, dir):
    bias_in = readparams(os.path.join(path,'bias_0_weight_values.txt'))
    n_1, batch_len = count_matrix(bias_in);
    w_0_weights = readparams(os.path.join(path,'w_0_weight_values.txt'))
    n_in, n_1_tmp = count_matrix(w_0_weights);
    if n_1_tmp!=n_1:
        raise ValueError("all n_1 vals must match" )
    bias_out = readparams(os.path.join(path,'bias_output_weight_values.txt'))
    n_out, batch_len_tmp = count_matrix(bias_out);
    if batch_len_tmp!=batch_len:
        raise ValueError("all batch len vals must match" )
    w_out_weights = readparams(os.path.join(path,'w_output_weight_values.txt'))
    n_1_tmp, n_out_tmp = count_matrix(w_out_weights);
    if n_out_tmp!=n_out:
        raise ValueError("all n_out vals must match" )
    
    with open_file_ensure_exists_dir(os.path.join("out",dir,s_filename), 'w') as sfp:
        sfp.write("#ifndef _SHAPE_h\n")
        sfp.write("#define _SHAPE_h\n")
        sfp.write(create_consts_def_CPP('N_IN', n_in)+"\n")
        sfp.write(create_consts_def_CPP('N_OUT', n_out)+"\n")
        sfp.write(create_consts_def_CPP('BATCH_LEN', batch_len)+"\n")
        sfp.write(create_consts_def_CPP('N_1', n_1)+"\n")
        sfp.write("#endif\n")
            
    with open_file_ensure_exists_dir(os.path.join("out",dir,w_filename), 'w') as wfp:
        wfp.write("#ifndef _WEIGHTS_h\n")
        wfp.write("#define _WEIGHTS_h\n")
        wfp.write(create_matrix_def_CPP('nn_bias_in', [' '.join(bias_in).replace('\n', ' ').replace('\r', '')])+"\n")
        wfp.write(create_matrix_def_CPP('nn_bias_out', [' '.join(bias_out).replace('\n', ' ').replace('\r', '')])+"\n") # [' '.join(bias_in).replace('\n', ' ').replace('\r', '')])+"\n")
        wfp.write(create_matrix_def_CPP('nn_w1', w_0_weights)+"\n")
        wfp.write(create_matrix_def_CPP('nn_w2', w_out_weights)+"\n")
        wfp.write("#endif\n")

    with open_file_ensure_exists_dir(os.path.join("out",dir,javaClassName+".java"), 'w') as jfp:
        jfp.write("package edu.ucsd.seelab.HierarchicalLearning;\n")
        jfp.write("public class {0} {{\n".format(javaClassName))
        jfp.write("    "+create_consts_def_Java('N_IN', n_in)+"\n")
        jfp.write("    "+create_consts_def_Java('N_OUT', n_out)+"\n")
        jfp.write("    "+create_consts_def_Java('BATCH_LEN', batch_len)+"\n")
        jfp.write("    "+create_consts_def_Java('N_1', n_1)+"\n")
        jfp.write("    "+create_matrix_def_Java('nn_bias_in', [' '.join(bias_in).replace('\n', ' ').replace('\r', '')])+"\n")
        jfp.write("    "+create_matrix_def_Java('nn_bias_out', [' '.join(bias_out).replace('\n', ' ').replace('\r', '')])+"\n") # [' '.join(bias_in).replace('\n', ' ').replace('\r', '')])+"\n")
        jfp.write("    "+create_matrix_def_Java('nn_w1', w_0_weights)+"\n")
        jfp.write("    "+create_matrix_def_Java('nn_w2', w_out_weights)+"\n")
        jfp.write("}\n")

for root, dirs, files in os.walk("DataFiles"):
    for dir in dirs:
        create_header_file("weigths.h", "shape.h", "ShapeAndWeights", os.path.join("DataFiles",dir), dir)

for root, dirs, files in os.walk("TrainDistributions"):
    for file in files:
        create_distribution_file(os.path.abspath("out"), file, os.path.join("TrainDistributions",file))