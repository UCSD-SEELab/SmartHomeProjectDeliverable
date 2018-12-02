# MQTTLogger
This simple program will connect to the message broker and store all data sent to it in a textfile toghether with a
timestamp identifying when the message was received. The files generated are the raw data files used in the learning process.

This is a windows console application written in C# and .NET. To compile you can simply use VisualStudio community edition 
(it is a simple .NET Core 2.1 program that should work on other platforms as well with Mono but we did not try other OSes; the Main
function is in Program.cs). To generate the executable file you can publish the code selecting selfcontained deployment. It will give 
you the option to compile for windows, linux and mac (see [here](https://docs.microsoft.com/en-us/dotnet/core/deploying/deploy-with-vs?tabs=vs157))

*For convinienge we have packeged compiled version in the repository (MQTTLogger_Win.zip).* 


When you run the MQTTLogger.exe file it will by default log to a file called MQTT_Messages.txt in the current directory. 
You can specify a different desination on the command line. 

```
MQTTLogger.exe
```
Will log to MQTT_Messages.txt in the current directory.

```
MQTTLogger.exe H:\MQTT_LOG\MQTT_Messages.txt
```
Will log to MQTT_Messages.txt in directory MQTT_LOG of drive H.

The resulting log file logs one line per Message and has 3 fields:
1. a timestamp
2. the topic the message was sent to
3. the message payload.

For example a message from the smartwatch sending the result of computing one of the herarchical neural networks locally would look like:

```
Timestamp:#11/30/2018 21:55:53#Topic:#watch/nnjson#Message:#{"timestamp":1543643750,"data":[[2.209544905217436,5.392193963731336]]}
```
We did not use the # character in our messages so it makes it easy to parse the string to its constituens.


The application will also log every message received on the console. To exit the program you can just press any key. 

## Warnings

**Do not kill the aplication** or the terminal it is running on because you may lose some of the messages not yet flushed to the log file.
**The application will owerwirte the file every time** so make sure to save your data before rerunnung this program.
