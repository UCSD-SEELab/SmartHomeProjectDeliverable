# ActivityRemoteSmartHome
This is a simple Android app that can be used to create labels used for training (see SmartHomePublic).
Users can run this app to connect send label messages of adtivities directly to the MQTT broker. 
These labels are used as truth value for the training phase.

The tool is written in Kotlin and can be simply opened with Android studio, compiled and side loaded on recen Android phones.

## Usage
Once the app starts, make sure the MQTT broker addess is correct and hit the Disconnected button to the right of the address.
There are 2 columns of buttons in the UI:
1. the left column for high level activities (such as cooking, etc.). Pressing the button 
once, a message labeling the activity as *in progress* is sent pressing it again a message labeling the activity as *ended* is sent.
Notice that multiple high level activities can be *in progress* at the same time. 
2. the right column send labels about lower level activities (such as standing, sitting, etc.). It works similarly to the left column
but only one of such activities can be active at a given time. If a new activity is selected while another is *in progress* the previous
*in progress* activity is automatically ended.
