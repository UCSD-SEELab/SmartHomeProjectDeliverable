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
from datetime import datetime

from deployModels.HLdeployer.communicate import *

cab1_time = datetime.now()
cab2_time = datetime.now()
draw1_time = datetime.now()
draw2_time = datetime.now()
fridge_time = datetime.now()

def on_msg_cab1(mosq, obj, msg):
    global cab1_time
    print("entering")
    if str(msg.payload) == "open":
        print ("open")
        curr = datetime.now()
        diff = (curr - cab1_time).days * 24 * 60
        cab1_time = curr
        if diff <= 1:
            com.dataDict["smartthings/Contact 1/contact"][0] = 1
        else:
            com.dataDict["smartthings/Contact 1/contact"][0] = 0

        if diff <= 5:
            com.dataDict["smartthings/Contact 1/contact"][1] = 1
        else:
            com.dataDict["smartthings/Contact 1/contact"][0] = 0

        if diff <= 10:
            com.dataDict["smartthings/Contact 1/contact"][2] = 1
        else:
            com.dataDict["smartthings/Contact 1/contact"][0] = 0

    print("received cabinet 1", com.dataDict["smartthings/Contact 1/contact"])


def on_msg_cab2(mosq, obj, msg):
    global cab2_time
    if str(msg.payload) == "open":
        print ("open")
        curr = datetime.now()
        diff = (curr - cab2_time).days * 24 * 60
        cab2_time = curr
        if diff <= 1:
            com.dataDict["smartthings/Contact 2/contact"][0] = 1
        else:
            com.dataDict["smartthings/Contact 2/contact"][0] = 0

        if diff <= 5:
            com.dataDict["smartthings/Contact 2/contact"][1] = 1
        else:
            com.dataDict["smartthings/Contact 2/contact"][0] = 0

        if diff <= 10:
            com.dataDict["smartthings/Contact 2/contact"][2] = 1
        else:
            com.dataDict["smartthings/Contact 2/contact"][0] = 0

    print("received cabinet2", com.dataDict["smartthings/Contact 2/contact"])



def on_msg_draw1(mosq, obj, msg):
    global draw1_time
    if str(msg.payload) == "open":
        print ("open")
        curr = datetime.now()
        diff = (curr - draw1_time).days * 24 * 60
        if diff <= 1:
            com.dataDict["smartthings/Drawer 1/contact"][0] = 1
        else:
            com.dataDict["smartthings/Drawer 1/contact"][0] = 0

        if diff <= 5:
            com.dataDict["smartthings/Drawer 1/contact"][1] = 1
        else:
            com.dataDict["smartthings/Drawer 1/contact"][0] = 0
    
        if diff <= 10:
            com.dataDict["smartthings/Drawer 1/contact"][2] = 1
        else:
            com.dataDict["smartthings/Drawer 1/contact"][0] = 0

    print("received Drawer 1", com.dataDict["smartthings/Drawer 1/contact"])

def on_msg_draw2(mosq, obj, msg):
    global draw2_time
    if str(msg.payload) == "open":
        print ("open")
        curr = datetime.now()
        diff = (curr - draw2_time).days * 24 * 60
        if diff <= 1:
            com.dataDict["smartthings/Drawer 2/contact"][0] = 1
        else:
            com.dataDict["smartthings/Drawer 2/contact"][0] = 0
    
        if diff <= 5:
            com.dataDict["smartthings/Drawer 2/contact"][1] = 1
        else:
            com.dataDict["smartthings/Drawer 2/contact"][0] = 0
    
        if diff <= 10:
            com.dataDict["smartthings/Drawer 2/contact"][2] = 1
        else:
            com.dataDict["smartthings/Drawer 2/contact"][0] = 0

    print("received drawer 2", com.dataDict["smartthings/Drawer 2/contact"])

def on_msg_fridge(mosq, obj, msg):
    global fridge_time 
    if str(msg.payload) == "open":
        print ("open")
        curr = datetime.now()
        diff = (curr - fridge_time).days * 24 * 60
        if diff <= 1:
            com.dataDict["smartthings/Fridge/contact"][0] = 1
        else:
            com.dataDict["smartthings/Fridge/contact"][0] = 0
    
        if diff <= 5:
            com.dataDict["smartthings/Fridge/contact"][1] = 1
        else:
            com.dataDict["smartthings/Fridge/contact"][0] = 0
    
        if diff <= 10:
            com.dataDict["smartthings/Fridge/contact"][2] = 1
        else:
            com.dataDict["smartthings/Fridge/contact"][0] = 0

    print("received fridge", com.dataDict["smartthings/Fridge/contact"])



with open("./config.json", 'r') as confFile:
    conf = json.load(confFile)["server"]

com = communicate(conf, [on_msg_cab1, on_msg_cab2, on_msg_draw1, on_msg_draw2, on_msg_fridge])
com.startListening()
com.predictionLoop()


