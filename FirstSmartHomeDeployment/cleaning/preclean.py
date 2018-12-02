'''
BSD 3-Clause License

Copyright (c) 2018, The Regents of the University of California.
All rights reserved.

Created by Anthony Thomas
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
from read_data import *

def toDateTime(s):
    dt = parser.parse(s)
    return dt

def main():
    clean_raw_data("../data/subject2_data/MQTT_Messages.txt", "../data/subject2_data/labels.txt", "subject2")
    clean_raw_data("../data/subject3_data/MQTT_Messages.txt", "../data/subject3_data/labels.txt", "subject3")

def clean_raw_data(path, label_path, subject=""):
    raw_data = RawDataDigester(path, label_path)

    labels = process_labels(
            raw_data
        ).groupby(
            "label", as_index=False
        ).first().set_index("timestamp")
    watch_data = process_watch_data(raw_data)
    tv_plug, teapot_plug = process_plug_data(raw_data)
    pir_data = process_pir_data(raw_data)
    contact_data = process_contact_data(raw_data)    
    misc_smartthings_data = process_misc_smartthings_data(raw_data)

    # create H5 dataset store for subject data
    out_path = "../temp/{}_data.h5".format(subject)
    hdf_opts = {"complib": "blosc", "complevel": 9}
    labels.to_hdf(out_path, "labels", **hdf_opts)
    watch_data.to_hdf(out_path, "watch", **hdf_opts)
    tv_plug.to_hdf(out_path, "tv_plug", **hdf_opts)
    teapot_plug.to_hdf(out_path, "teapot_plug", **hdf_opts)
    pir_data.to_hdf(out_path, "pir1", **hdf_opts)

    for name, data in contact_data.iteritems():
        data.to_hdf(out_path, name, **hdf_opts)

    for name, data in misc_smartthings_data.iteritems():
        data.to_hdf(out_path, name, **hdf_opts)
    
def process_labels(raw_data):
    labels = raw_data.get_labels()
    data = {
        "label": [x["message"] for x in labels], 
        "timestamp": [toDateTime(
            x["timestamp"]) for x in labels]
    }

    return pd.DataFrame(data)


def process_pir_data(raw_data):
    pir = raw_data.get_pir_data()[0]

    vals = [extract_pir_values(x["message"]) for x in pir]
    mat = np.concatenate(vals).reshape(-1,vals[0].size)

    clean_data = pd.DataFrame(mat)
    clean_data.columns = map(lambda x: "pir1_{}".format(x), range(mat.shape[1]))
    clean_data["timestamp"] = [toDateTime(x["timestamp"]) for x in pir]

    return clean_data.set_index("timestamp")

def extract_pir_values(msg):
    return np.array(json.loads(msg)["values"])


def process_watch_data(raw_data, save_stub=""):
    watch = raw_data.get_watch_data()

    data = {"step": [], "heart_rate_bpm": [],
            "accel_X": [], "accel_Y": [], "accel_Z": [],
            "gyro_X": [], "gyro_Y": [], "gyro_Z": [], 
            "timestamp": []}

    for parsed in watch:
        current_ts = parsed['timestamp']    
        current_ts = current_ts.split(".")[0].split("T")
        current_ts = current_ts[0] + " " + current_ts[1]

        parsed = parsed["message"].strip().split(";")
        data["step"].append(float(parsed[1]))
        data["heart_rate_bpm"].append(float(parsed[3]))
        data["accel_X"].append(float(parsed[5]))
        data["accel_Y"].append(float(parsed[6]))
        data["accel_Z"].append(float(parsed[7]))
        data["gyro_X"].append(float(parsed[9]))
        data["gyro_Y"].append(float(parsed[10]))
        data["gyro_Z"].append(float(parsed[11]))
        data["timestamp"].append(toDateTime(current_ts))

    clean_data = pd.DataFrame(data).set_index("timestamp").sort_index()

    return clean_data

def process_plug_data(raw_data):
    # apparently these features are junk
    plug1 = unpack_features(raw_data.get_plugs_data()[0])
    plug2 = unpack_features(raw_data.get_plugs_data()[1])
    #tv_plug = unpack_features(raw_data.get_plugs_data()[3])
    #teapot_plug = unpack_features(raw_data.get_plugs_data()[4])

    return plug1, plug2

def unpack_features(messages, dtypes=None, default_dtype=np.float64):
    if dtypes is None:
        dtypes = {}
    messages_new = []
    for item in messages:
        tmp = {}
        for name, value in item.iteritems():
            if name == 'timestamp':
                value = value.split(".")[0].split("T")
                value = value[0] + " " + value[1]
                tmp['timestamp'] = value
            else:
                value = eval(value)
                tmp['voltage'] = value['voltage']
                tmp['current'] = value['current']
        messages_new.append(tmp)

    messages = messages_new
    data = {x: [] for x in messages[0].keys()}
    for item in messages:
        for name, value in item.iteritems():
            if name == "timestamp":
                data[name].append(toDateTime(value))
            elif name in dtypes:
                data[name].append(dtypes[name](value))
            else:
                data[name].append(default_dtype(value))

    processed = pd.DataFrame(data)
    return processed.set_index("timestamp") if "timestamp" in data else processed


def parse_rssi_message(msg):
    cleaned = msg.replace("{","").replace("}","").split(",")
    data = {}
    for item in cleaned:
        name, value = item.split(": ")
        data[name.replace(" ","")] = float(value)
    return data


def process_contact_data(raw_data):
    contact_sensor_vars = {"smartthings/Basin Left Drawer/contact": "drawer1",
                           "smartthings/Cabinet Drawer/contact": "drawer2",
                           "smartthings/Dishwasher Above Drawer/contact": "drawer3",
                           "smartthings/Bottom Left Drawer/contact": "drawer4",
                           "smartthings/Top Left Drawer/contact": "fridge",
                           "smartthings/Stove Lower Drawer/contact": "drawer5",
                           "smartthings/Top Right Drawer/contact": "drawer6"}

    contact_data = {}
    for data_stream_name, clean_name in contact_sensor_vars.iteritems():
        stream = raw_data.data[data_stream_name]
        varname = "{}_contact".format(clean_name)
        data = {varname: [], "timestamp": []}
        for item in stream:
            value = item['timestamp'].split(".")[0].split("T")
            value = value[0] + " " + value[1]

            data[varname].append(1 if "open" in str(item["message"]) else 0)
            data["timestamp"].append(toDateTime(value))

        contact_data[varname] = pd.DataFrame(data).set_index("timestamp")

    return contact_data


def process_misc_smartthings_data(raw_data):
    return {
        "dining_room_motion": process_active_stream(
            raw_data, "smartthings/Aeotec MultiSensor 6/motion", "dining_room_motion"),
        "living_room_motion": process_active_stream(
            raw_data, "smartthings/Motion Sensor/motion", "living_room_motion"),
    }


def process_active_stream(raw_data, name, varname):
    stream = raw_data.data["smartthings/{}".format(name)]
    data = {
        varname: [x["message"] == "active" for x in stream],
        "timestamp": [process_watch_ts(x["timestamp"]) for x in stream]
    }

    return pd.DataFrame(data)

if __name__=="__main__":
    main()
