# this is a test

import paho.mqtt.client as mqtt
import time
import json
from simplecrypt import encrypt, decrypt
import pickle
import numpy as np
from sklearn.cluster import KMeans
import sys

def on_message(client, userdata, message):
    fh = open("cycle2sampling01bt.txt","a")
    fh.write(message.payload+"\n")
    fh.close()
    global mqtt_data,receive_msg
    mqtt_data = json.loads(message.payload)
    receive_msg = True

if __name__ == "__main__":
    if len(sys.argv)>1:
        if sys.argv[1]=="training":
            mqtt_data = ""
            receive_msg = False

            name = "laptop"
            topic = "tAxiY7W4P58QH5Oq/living_room/pir"
            broker_address = "iot.eclipse.org"
            client = mqtt.Client(name)
            client.on_message = on_message
            client.connect(broker_address, 1883, 60)

            client.subscribe(topic,qos=0)

            while True:
                client.loop_start()
                if receive_msg:
                    receive_msg = False
                    print(str(mqtt_data))
                time.sleep(1)
                client.loop_stop()

        elif sys.argv[1]=="testing":
            with open("kmeans", "rb") as sf:
                kmeans = pickle.load(sf)

            with open("GaussianMixture", "rb") as sf2:
                clusterer = pickle.load(sf2)

            with open("PCA", "rb") as sf3:
                pca = pickle.load(sf3)

            mqtt_data = ""
            receive_msg = False

            name = "laptop"
            topic = "tAxiY7W4P58QH5Oq/living_room/pir"
            broker_address = "iot.eclipse.org"
            client = mqtt.Client(name)
            client.on_message = on_message
            client.connect(broker_address, 1883, 60)

            client.subscribe(topic, qos=0)

            while True:
                client.loop_start()
                if receive_msg:
                    receive_msg = False
                    raw = map(int,mqtt_data["values"])

                    # X = [[raw[i]+raw[i+5]+raw[i+10]+raw[i+15]+raw[i+20]+raw[i+25] for i in range(5)]]
                    X = pca.transform([raw])
                    y = clusterer.predict(X)
                    print mqtt_data["timestamp"]+" predict: "+str(y)
                time.sleep(1)
                client.loop_stop()
        else:
            print "Usage: python main_laptop.py <training/testing> "
    else:
        print "Usage: python main_laptop.py <training/testing> "
