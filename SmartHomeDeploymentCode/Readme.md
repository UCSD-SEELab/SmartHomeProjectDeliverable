# Deploying Hierarchical Models

This repository contains the code for deploying the pretrained hierarchical models on the 
various devices. The repo is divided into 2 folders:
- Library
- HomeDeployment

The Library holds a python module which we wrote for making deployment easier. The 
library contains 2 submodules.

```
/deployModels
	/communicate - Handles the mqtt messaging, makes use of loadModel to predict and 
	send the result to the next layer
	/loadModel - Handles loading the frozen tensorflow modules and produces the output
```
# Installation
# How to use the deployModules module

## Configuration

The first step in using the module is to write a config file which tells the library the
configuration parameters for the mqtt host, port and other relevant information. The 
different fields are given below. Some of the fileds are mandatory while others are 
optional depending on the functionality desired

```
{
	<name of config>: {
		"host": <hostaddress of the broker>,
		"port": <port on which the broker is listening - usually 1883 for mqtt>,
		"keepalive": <keepalive time for mqtt server>,
		"username": <username - login credential for the broker>,
		"pwd": <password for the broker>,
		"topics": <List of topics to which the mqtt client need to subscribe to>,
		"clientID": <clientId of the client - can be assigned manually or autoamted>,
		"sensor": <the sensor name - or the frozen model name>,
		"numInputs": <number of features per sensor/topic - used for initialization>,
		"outputTopic": <the mqtt Topic to which the partial inference should be sent to>,
		"modelDir": <path to the folder where the frozen models are located at> 
	}
}
```

The below table shows which fields are mandatory and their use


Name of Field | Mandatory | Usage
------------- | --------- | -----
host | Yes | Host address
port | Yes | Port on which mqtt host listens on
keepalive | Yes | keepalive value for mqtt server
username | Optional | If the host requires credentials then username needs to be suplied
pwd | Optional | Used if host requires login credentials
topics | Optional | Topics need to be listed, if the mqtt client needs to subscribe for any particular topic. In our architecture, for simplicity, we do not configure point to point communication between the devices in different hierarchy. Instead we use a common message bus on which every device communicate. We partition the hierarchy by configuring each device to subscribe only to those topics that it requires access to.
clientID | Yes | Each node communicating must have a unique clientID
sensor | Yes | This field is used to load the frozen module. The frozen model for each node is of the form <sensor>_frozen.pb. 
numInputs | Yes | This field indicates the number of features for each topic that is subscribed to. This is used for initialization internally.
outputTopic | Yes | This is the topic to which the processed data must be sent to on the broker.
modelDir | Yes | This is the path to the folder in which the frozen models are stored in

## Functions Provided

This section briefly describes the various functionality available in both the classes

### loadModel

The loadModel class is for simply loading the frozen model and performing inference using that model. It takes the input and config file, and loads the corresponding model file as specified by the config. It then performs inference and returns the value.

Example:
```
import json
with open("config.json") as fp:
	conf = json.load(fp)

model = loadModel(conf) #initialize model by passing the config file
#The compute function takes a 2D array as input where each row is a feature vector and returns the output which is the partial inference from the model
result = model.compute([[2.0, 3.0]]) 
print result
# [[[-0.30995443  0.3088935 ]]]
```
### communicate

The communicate class inherits the loadModel class and provides wrappers for mqtt and looping functions to perform inference every 3 seconds (which is our choosen time interval based on data analysis). This feature is used in the 2nd level of hierarchy (data aggregators).

Function name | Arguments | Functionality
------------- | --------- | -------------
sendData | topic, msg string | Sends the data to the specified topic to the broker (given in config file)
startListening | - | Starts the mqtt client loop which listens for data on all topics subscribed to and triggers the respective callbacks when data is recieved
stopListening | - | Stop the mqtt client loop
predictionLoop | - | This function repeatedly performs inference every 3 seconds and sends the inference result to the mqtt broker on the topic specified in configuration file. This function is used in the 2nd level of hierarchy - at the data aggregators as explained later.


## Usage

There are 2 different usages of this library in the code. One, for the node that is directly connected to the sensor and the second is for data aggreators or nodes at levels higher up in the hierarchy. For the case in which the sensor is directly connected to the node, it's very straightforward. You write the code to extract raw data from the sensors and perform feature extraction, then you call the `compute` function from the `loadModel` class and then send the result out using the `sendData` method of the `communicate` class. 

For data aggregators or other nodes above the sensor in hierarchy, the usage is different. Since we aren't implementing a point to point communication, each of the data aggregator needs to subscribe to the topics the sensor nodes are sending data on. They acquire the said output and then run inference on them. In order to do this, the class allows you to implement callback and implements variables inside the class. The communicate class, creates a dictionary of variables, where in every topic it subscribes to, is used as a key in the dictionary to get the corresponding input. 
The user defines callback functions in their python code, which should update the internal variable of the class. The list of callbacks is passed as argument to the constructor of the class during initialization. If no callbacks are passed, it is assumed that the program is operating on the first case of sensor nodes.

An example can be found [here](https://github.com/UCSD-SEELab/smarthomeDeploymentCode/blob/master/HomeDeployment/kitchen_hub/kitchenFeed.py)
In the following piece of code
```
# we define a callback function which is called everytime a data is recieved on the topic subscribed to.
# Here the topic is "MetaSense/nnjson" which is the output stream of the air quality sensor.
# The class creates an internal field in a dictionary, using the topic as the key
# in our callback we update this internal field with the value of metasense partial inference.

def on_msg_metasense(mosq, obj, msg):
    datum = json.loads(str(msg.payload))
    com.dataDict["MetaSense/nnjson"] = datum["data"][0] #Here we update the internal variable with the value
    print ("recieved metasense", com.dataDict["MetaSense/nnjson"])

with open("./config.json", 'r') as confFile:
    conf = json.load(confFile)["test"]

# In this line we pass as input the list of call back functions
com = communicate(conf, [on_msg_metasense, on_msg_mat, on_msg_plug])
com.startListening() #start the mqtt client on a listening loop to look out for messages
com.predictionLoop() #this function automatically performs inference on the last stored value in the internal variable every 3 seconds and sends the result
```
# Code we used for deploying in the home

The HomeDeployment folder contains the code that we used for the different rooms and the plug sensors. Each folder contains code that runs on a Pi.
The Pis connected to the plug meters are Pi Zeros and all the others are Pi Model B 2

# License Information
The code for connecting to the tp link plug was taken from this [repository](https://github.com/j05h/hs100)

