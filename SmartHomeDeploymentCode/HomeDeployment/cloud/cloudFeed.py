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

def on_msg_kitchen(mosq, obj, msg):
    datum = json.loads(str(msg.payload))
    com.dataDict["kitchen_inference"] = datum["data"][0]
    print ("recieved kitchen", com.dataDict["MetaSense_inference"])

def on_msg_livingroom(mosq, obj, msg):
    datum = json.loads(str(msg.payload))
    com.dataDict["livingroom_inference"] = datum["data"][0]
    print("recieved livingroom", com.dataDict["livingroom_inference"])

def on_msg_smartthings(mosq, obj, msg):
    datum = json.loads(str(msg.payload))
    com.dataDict["smartthings_inference"] = datum["data"][0]
    print("recieved mat", com.dataDict["smartthings_inference"])

def on_msg_watch(mosq, obj, msg):
    datum = json.loads(str(msg.payload))
    com.dataDict["watch/nnjson"] = datum["data"][0]
    print("recieved watch", com.dataDict["watch/nnjson"])

def on_msg_localization(mosq, obj, msg):
    datum = json.loads(str(msg.payload))
    com.dataDict["localization_inference"] = datum["data"][0]
    print("recieved localization", com.dataDict["localization_inference"])

with open("./config.json", 'r') as confFile:
    conf = json.load(confFile)["server"]

com = communicate(conf, [on_msg_kitchen, on_msg_livingroom, on_msg_smartthings, on_msg_watch, on_msg_localization])
com.startListening()
com.predictionLoop()


