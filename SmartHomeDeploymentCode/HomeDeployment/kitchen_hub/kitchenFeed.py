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
sys.path.append('../../Library/')
import json

from deployModels.HLdeployer.communicate import *

def on_msg_metasense(mosq, obj, msg):
    datum = json.loads(str(msg.payload))
    com.dataDict["MetaSense/nnjson"] = datum["data"][0]
    print ("recieved metasense", com.dataDict["MetaSense/nnjson"])

def on_msg_plug(mosq, obj, msg):
    datum = json.loads(str(msg.payload))
    com.dataDict["teapot_plug_inference"] = datum["data"][0]
    print("recieved plug", com.dataDict["teapot_plug_inference"])

def on_msg_mat(mosq, obj, msg):
    datum = json.loads(str(msg.payload))
    com.dataDict["pressuremat_inference"] = datum["data"][0]
    print("recieved mat", com.dataDict["pressuremat_inference"])

with open("./config.json", 'r') as confFile:
    conf = json.load(confFile)["test"]

com = communicate(conf, [on_msg_metasense, on_msg_mat, on_msg_plug])
com.startListening()
com.predictionLoop()


