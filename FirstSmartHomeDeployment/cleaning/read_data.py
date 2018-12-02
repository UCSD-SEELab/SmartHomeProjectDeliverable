'''
BSD 3-Clause License

Copyright (c) 2018, The Regents of the University of California.
All rights reserved.

Created by Yunhui Guo
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
sys.path.append('../')

from preliminaries.preliminaries import *

def toDateTime(s):
    dt = parser.parse(s)
    return dt

# **** Processing procedure for raw data ****
class RawDataDigester(object):
    def __init__(self, data_path, label_path):
        with open(data_path) as fh:
            lines = fh.readlines()

        with open(label_path) as fh:
            self.labels = fh.readlines()

        self.data = defaultdict(list)

        curr_timestamp = None
        for line in lines:
            line = eval(line.strip())
            topic = line['Topic']
            if topic == 'watch':
                curr_timestamp = line['TimeStamp']

            if curr_timestamp is not None:
                break

        for line in lines:
            try:
                line = eval(line.strip())
                topic = line['Topic']

                message = line['Payload']

                if topic == 'watch':
                    curr_timestamp = line['TimeStamp']
                    message = {"timestamp": curr_timestamp, "message": message}
                else:
                    message = {"timestamp": curr_timestamp, "message": message}

                self.data[topic].append(message)
            except SyntaxError:
                continue

    def get_watch_data(self):
        return self.data['watch']

    def get_pir_data(self):
        return self.data['pir/raw/1'], self.data['pir/raw/2']

    def get_plugs_data(self):
        return self.data['plug1'], self.data['plug2'], self.data['plug3']

    def get_ble_data(self):
        return self.data['rssi1'], self.data['rssi2'], self.data['rssi3']

    def get_smartthings_data(self):
        return self.data['smartthings']

    def get_bulb_data(self):
        return self.data["bulb"], self.data["kitchen_bulb"]

    def get_pressuremat_data(self):
        return self.data['PressureMat/raw']

    def list_topics(self):
        return self.data.keys()

    def get_labels(self):

        for line in self.labels:
            message = line.strip().split(" ", 2)
            curr_timestamp = message[0] + " " + message[1]
            print message
            message = {"timestamp": curr_timestamp, "message": message[2]}
            self.data["labels"].append(message)

        return self.data["labels"]

if __name__ == '__main__':
    data_file = "../data/subject1_data/MQTT_Messages.txt"
    labels_file = "../data/subject1_data/labels.txt"

    p = RawDataDigester(data_file, labels_file)

    print p.list_topics()

