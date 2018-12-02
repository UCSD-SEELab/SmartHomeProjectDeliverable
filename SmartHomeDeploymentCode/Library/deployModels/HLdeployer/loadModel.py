'''
BSD 3-Clause License

Copyright (c) 2018, The Regents of the University of California.
All rights reserved.

Created by Rishikanth Chandrasekaran
SEE Lab
University of California, San Diego

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

import tensorflow as tf
from tensorflow.python.framework import tensor_util
import numpy as np

class loadModel():
    def __init__(self, conf):
        assert "sensor" in conf.keys(), "Sensor name needs to be given"
        assert "modelDir" in conf.keys(), "Unable to find Model directory"

        self.sensorName = conf["sensor"]
        self.modelDir = conf["modelDir"]
        with open(conf["modelDir"] + "variable_list.txt") as fp:
            self.variableList = eval(fp.read())
        self.frozenModel = self.modelDir + conf["sensor"] + "_frozen.pb"
        self.sess = tf.Session()
        self.graph_def = tf.GraphDef()
        self.loadGraph()

    def loadGraph(self):
        with tf.gfile.GFile(self.frozenModel, "r") as f:
            self.graph_def.ParseFromString(f.read())

    def load_frozen_model(self, sensor_input):
        if type(sensor_input) is not np.ndarray:
            sensor_input = np.array(sensor_input)

        new_input = tf.placeholder(tf.float32, [None, sensor_input.shape[1]], self.sensorName)
        keep_prob = tf.placeholder(tf.float32)
    
        variable_name = self.sensorName + "_output"
        for idx, var in enumerate(self.variableList):
            if variable_name in var:
                variable_name = var
                break
    
        output = tf.import_graph_def(
            self.graph_def,
            input_map={"input/" + self.sensorName+":0": new_input, "input/keepprob:0": keep_prob},
            return_elements = [variable_name+":0"]
        )
            
        results = self.sess.run(output, feed_dict={new_input: sensor_input, keep_prob : 1.0})

        return results

    def compute(self, inputData):
        return self.load_frozen_model(inputData)


    
if __name__=="__main__":
    import json
    with open("../test/config.json") as fp:
        conf = json.load(fp)

    model = loadModel(conf)
    res = model.compute([[2.0, 3.0]])
    print(res)
    
