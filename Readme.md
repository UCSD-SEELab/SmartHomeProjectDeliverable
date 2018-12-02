# SmartHome Herarchical Learning Software Packages
Each subdirectory contains differnt parts of the software ditribution needed to replicate our Herarchical Leaarning experimetns.
 
Here is the  list of software packages we are delivering:

1. SmartHomePublic - Code for Training both monolithic and hierarchical models based on the sensor data collected in the SmartHome.
 This folder also contains the datasets we collected in the second home we instrumented.
2. FirstSmartHomeDeployment - This directory contains the datasets for first home we instrumented along with a version of the training code that works on this datasets.
3. MQTTLogger - Logs MQTT Messages and creates the txt file used by SmartHomePublic (this also converts the MQTT raw data messages sent by the sensor to the format used in our training)
4. Docker Apollo MQ – A simple docker build file to automate correct configuration of the MQTT server
5. ActivityRemoteSmartHome – Android app that send the activity labels directly to the MQTT used during the data collection 
6. NeuralNetExportEmbedded – A python script that reads the weights and distribution text files generated by SmartHomePublic and generate C header files and java files to configure automatically the classifiers written in C and Java.
7. NNCompute – The neural network classifier written in C++ to be used with low end embedded systems (like the particle Photon that powers the MetaSense air quality sensor)
8. SmartHomeDeploymentCode – Python implementation of the hierarchical classifiers that runs on the Raspberry PIs.
9. MetaSenseFirmware – A version of the MetaSense firmware that can send raw air quality data directly to MQTT, it also has run the hierarchical NN and send the result to MQTT
10. BLE-Location – Android application to collect data from the BLE beacons, it runs the hierarchical NN classifier and can send both the classier data and the raw data to MQTT
11. SmartWatch – Two app on the watch captures all data and send it to the Android phone app. The android phone forward raw data to MQTT, it also run a hierarchical classifier and send the result to MQTT
12. SmartThings MQTT Interface – This folder contains instructions on how to run a small server script that receive event messages from the Samsung Hub and forward them to MQTT.
13. PIR - Runs on a Raspberry PI connected to custom hardware to implement a PIR sensor that can detect movement on different angular directions


## License
All code produced by UCSD is licensed under BSD 3 clause license. Other code available in the public domain that we used is included in this repository for convenience. Licenses are specified in the different subfolders and in banners in the code files.