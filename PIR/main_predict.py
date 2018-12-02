# this is a test

import paho.mqtt.client as mqtt
import time
import json
import pickle
import numpy as np
from sklearn.cluster import KMeans
import sys

def on_message(client, userdata, message):
    global mqtt_topic,mqtt_data,receive_msg
    mqtt_topic = message.topic
    mqtt_data = json.loads(message.payload)
    receive_msg = True

def filter(past_result):
    ans = 0
    weight = []
    for i in range(len(past_result)):
        ans += (i+1)*past_result[i]
        weight.append(i+1)
    ans = ans/np.sum(weight)
    return int(round(ans))


if __name__ == "__main__":

    past_result1 = [4]
    past_result2 = [4]
    cnt=0
    filter_len = 3

    with open("DecisionTree", "rb") as sf1:
        clusterer = pickle.load(sf1)

    with open("decomposition", "rb") as sf2:
        ica = pickle.load(sf2)

    mqtt_data = ""
    receive_msg = False

    name = "laptop"
    topic1 = "pir/raw/1"
    topic2 = "pir/raw/2"
    topic3 = "pir/angular_locations"
    broker_address = "192.168.10.6"
    client = mqtt.Client(name)
    client.username_pw_set("admin","IBMProject$")
    client.on_message = on_message
    client.connect(broker_address, 61613, 60)

    client.subscribe(topic1, qos=0)
    client.subscribe(topic2, qos=0)

    while True:
        client.loop_start()
        if receive_msg:
            receive_msg = False
            raw = map(int,mqtt_data["values"])

            X = ica.transform([raw])
            y = clusterer.predict(X)
            if mqtt_topic==topic1:
                if len(past_result1)==filter_len:
                    past_result1.remove(past_result1[0])
                past_result1.append(int(y[0]))
            else:
                if len(past_result2)==filter_len:
                    past_result2.remove(past_result2[0])
                past_result2.append(int(y[0]))
            # print mqtt_data
            angle1 = filter(past_result1)*11.25
            angle2 = filter(past_result2)*11.25
            print "bookshelf:"+str(angle1)+" deg; "+"drawer:"+str(angle2)+" deg"
            # print mqtt_data["timestamp"]+" predict: "+ str(labels[int(np.median(past_result))])
            cnt += 1
            # print("total:"+str(cnt))
            payload_angle = json.dumps({"unit":"degree","angle1": angle1, "angle2": angle2})
            client.publish(topic3, payload_angle, qos=0, retain=False)
        time.sleep(1)
        client.loop_stop()
