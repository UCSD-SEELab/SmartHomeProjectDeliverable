# this is a test

import paho.mqtt.client as mqtt
import time
import json
import pickle
import numpy as np
import matplotlib.pyplot as plt
from sklearn.cluster import KMeans
import sys
plt.ion()

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

def degree2cartisian(theta1,theta2,x1=6,y1=19,x2=11,y2=1):
    offset1 = 45
    offset2 = 0
    tmp1 = np.tan((theta1+offset1)/180*np.pi)
    tmp2 = np.tan((theta2+offset2)/180*np.pi)
    x = (y1-y2+tmp1*x1-tmp2*x2)/(tmp1-tmp2)
    y = y1-tmp1*(x-x1)
    return [x,y]


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

    # plot
    xrange = [0,13]
    yrange = [0,21]
    f, ax = plt.subplots()
    ax.axis([-5, 13, -5, 21])
    ax.set_xlabel('x')
    ax.set_ylabel('y')
    ax.set_aspect('equal')
    ax.plot(6,19,'r^') # pir1
    ax.plot(11,1,'r^') # pir2
    ax.plot([0,0], [8,21],'k')  # wall
    ax.plot([0,3,3,0,0],[12,12,18,18,12],'purple') # sofa
    ax.plot([6,8.5,8.5,6,6], [11,11,15,15,11],'g')  # table
    ax.plot([10,11,12,11,10], [16,15,16,17,16],'y')  # coach
    ax.plot([10,11,12,11,10], [12,11,12,13,12],'y')  # coach

    handler = ax.plot(0, 0, 'bo')
    f.canvas.flush_events()
    plt.pause(0.05)

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

            pos = degree2cartisian(angle1, angle2)
            #if xrange[0]<=pos[0]<=xrange[1] and yrange[0]<=pos[1]<=yrange[1]:
            handler[0].set_xdata(pos[0])
            handler[0].set_ydata(pos[1])
            f.canvas.flush_events()
            plt.pause(0.05)

            payload_angle = json.dumps({"angle1": angle1, "angle2": angle2, "posx": pos[0], "posy": pos[1]})
            client.publish(topic3, payload_angle, qos=0, retain=False)

            print "bookshelf:"+str(angle1)+" deg; "+"drawer:"+str(angle2)+" deg; "+"pos:"+str(pos)

        time.sleep(1)
        client.loop_stop()
    plt.ioff()
    plt.show()
