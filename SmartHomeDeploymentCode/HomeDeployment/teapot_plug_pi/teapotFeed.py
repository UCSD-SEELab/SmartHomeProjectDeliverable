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
import time
from hs100 import core
import numpy as np

from deployModels.HLdeployer.communicate import *

with open("./config.json", 'r') as confFile:
    conf = json.load(confFile)["server"]

com = communicate(conf)
plug = core.Core('192.168.10.85', 9999, False)
plugBuf = [0.0] * 3
counter = 0

while True:
    plugReading = plug.request('meter')['emeter']['get_realtime']
    plugBuf[counter] = plugReading['current']
    counter = (counter + 1) % 3
    feat1 = (np.mean(plugBuf) - 0.488167) / 1.723526
    feat2 = (np.var(plugBuf) - 0.220158) / 1.82480 
    result = com.compute([[feat1, feat2]])[0][0]
    com.sendData(conf["outputTopic"], '{"data": ' + str([[result[0], result[1]]]) +  '}')
    #print com.predictFromWeights([1,2])
    time.sleep(5)
