#MQTT Bridge
To forward all Zigbee and ZWave sensors data to our MQTT server we use https://github.com/stjohnjohnson/smartthings-mqtt-bridge

The information on how to set this up is available on the github repositoy above. We connect all sensors to SmartThings and configure them via a SmartThings SmartApp to send the data to a web server process running on the same local home server where we are running the MQTT broker. This MQTTBridge web server receives events via HTTP form the SmaarThings Hub and forward them the the MQTT broker.

Form the broker messages are reveived via MQTT by a raspberry pi that runs the hierachical classifier and send the result to the upper level of the classifier herarchy.

In summary the sequence of messages is:
```
       Zigbee              Internet                   Internet             HTTP              MQTT              MQTT
sensor ------> Samsung Hub -------> SmartThings Cloud -------->Samsung Hub -----> MQTTBridge ----->MQTT Broker------> PI Classifier
```
Of course this architecture is just for testing the capability another option would be to add a Zigbee radio directly on the PI and run a single hop from the senor to the PI Classifier.
An even better solution would be to modify the HUB to be able to run herarchical classifiers locally.
