# BLE Location for SmartHome Project
This application collects data from BLE beacons in our experimental SmartHome and communicate with the MQTT broker to send the information about the raw RSSI values recieved from the beacons an the result of applying feature extraction and a NN with wights computed by the training phase (see SmartHomePublic).

The application is in Java and can be open, compiled, and side loaded on modern Android phones using Android Studio.
After a new training cycle the wigths must be updated and the application recompiled and redeployed.
To do this simply replace the file ShapeAndWeights.java with the one found after running the NeuralNetExportEmbedded.py script in NeuralNetExportEmbedded\out\ble_location. See instructions in NeuralNetExportEmbedded for information on how to generate the file.
If you use different beacons you also need to sepcify their MAC addresses in the filters in MainActivity.java lines 226-253 and 334-339.
All communication configurations (server address, user, and password) are in the file SamsungProjectConfig.java

## Usage
The UI has 2 buttons
1. Scan start the scanning process and look for beacons. The default behavior is to apply the NN and
send the resulting data via MQTT on the channel location/nnjson.
2. Send Raw sends the raw data to topic crk. This data is used to train the models.

## Warning
The application will not work if the phone goes to sleep. Please diable sleep and leave the screen on to collect data.
