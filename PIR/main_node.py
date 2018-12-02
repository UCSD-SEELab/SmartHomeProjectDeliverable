# this is a test

from datetime import datetime
import time
import paho.mqtt.client as mqtt
import json


if __name__ == "__main__":
    name = "raspberry_pi"
    topic = "tAxiY7W4P58QH5Oq/living_room/pir"
    broker_address = "iot.eclipse.org"
    client = mqtt.Client(name)
    client.connect(broker_address, 1883, 60)
    while True:
        payload = json.dumps(str(datetime.now()))
        client.publish(topic, payload, qos=0, retain=False)
        print("publishing "+payload)
        time.sleep(1)