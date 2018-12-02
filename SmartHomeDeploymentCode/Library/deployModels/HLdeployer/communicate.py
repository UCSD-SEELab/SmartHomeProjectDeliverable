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

import sys
from collections import OrderedDict
import paho.mqtt.client as paho
import json
import time
import subprocess

try:
    __import__('tensorflow')
except ImportError:
    class loadModel():
        pass
else:
    from loadModel import *

class communicate(loadModel):
    def __init__(self, conf, callbacks = None, useTensor = True):
        self.conf = conf
        self.dataDict = OrderedDict()
        self.topics = self.conf['topics']
        self.outputTopic = conf['outputTopic']
        self.mqttClient = paho.Client(conf["clientID"])

        if "username" in self.conf.keys():
            self.mqttClient.username_pw_set(conf["username"], conf["pwd"])
        
        self.mqttClient.connect(self.conf["host"],
                self.conf["port"],
                self.conf["keepalive"])

        if callbacks:
            assert len(self.topics) == len(callbacks), "each topic should have a callback"

            for topic,func in zip(self.topics, callbacks):
                self.dataDict[topic] = [0.0] * conf["numInputs"]
                self.mqttClient.message_callback_add(topic, func)
                self.mqttClient.subscribe(topic)

        if useTensor:
            if 'tensorflow' in sys.modules:
                loadModel.__init__(self, conf)
            else:
                print("error Tensorflow not installed")
                exit()
        else:
            self.modelDir = conf["modelDir"].strip(".")
            self.outputVal = [0] * conf["outputLength"]
            subprocess.call(["make", "-C", conf["modelDir"]])
                
    def sendData(self, topic, msg):
        self.mqttClient.publish(topic, msg)

    def sendStoredData(self, topic):
        self.mqttClient.publish(topic, self.dataDict[topic])

    def startListening(self):
        self.mqttClient.loop_start()

    def stopListening(self):
        self.mqttClient.loop_stop()

    def predictionLoop(self):
        while True:
            feature_vec = []
            for key, vals in self.dataDict.items():
                feature_vec = feature_vec + vals

            result = self.compute([feature_vec])[0]
            print (self.compute([feature_vec]))
            self.sendData(self.outputTopic, "{data:" + str(result) +"}") 
            time.sleep(3)

    def predictFromWeights(self, inputFeat):
        feature_vec = [str(val) for val in inputFeat]
        callArr = ["." + self.modelDir + "compute"] + feature_vec
        process = subprocess.Popen(callArr, stdout = subprocess.PIPE)
        result, err = process.communicate()
        print (result)
        self.sendData(self.outputTopic, "{data:" + str(result) +"}") 
        return json.loads(result)

if __name__ == '__main__':
    with open("../test/config.json", 'r') as confFile:
        conf = json.load(confFile)["local"]

    def blah(mosq, obj, msg):
        datum = json.loads(str(msg.payload))
        print (datum)
        test.dataDict["crk"] = datum["data"][0]
        print(test.dataDict["crk"])

    test = communicate(conf, [blah])
    test.startListening()
    #test.sendData("crk", str("[5.0, 9.0]"))
    #import time
    #time.sleep(5)
    #print("Exiting")
    #test.stopListening()
    test.predictionLoop()

