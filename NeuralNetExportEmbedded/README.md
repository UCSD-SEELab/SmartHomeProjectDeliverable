# NeuralNet Export Embedded
This python script will generate header files for C and Java classes intended to configure different embedded applications to use the Neural Net parameters learned by the code in SmartHomePublic.
[For more information on how to generate these parameter see intructions in SmartHomePublic]

## Usage
Copy the parameter files to DataFiles and TrainDistributions:
* DataFiles contains weights information found under SmartHomePublic\output\NeuralNets\HierarchyAwareMLP\saved_models\saved_weights
* TrainDistributions contains the CSV files found under SmartHomePublic\output

For example under windows pwershell you would run the script as follow:
```
cp ..\SmartHomePublic\output\*.csv .\TrainDistributions\
cp -r ..\SmartHomePublic\output\NeuralNets\HierarchyAwareMLP\saved_models\saved_weights\* .\DataFiles\
python NeuralNetExportEmbedded.py
```
The resulting files are created under directory `out`. Each subdirectory containg Java and Header files.
* distribution.h and Distribution.java contain the means and variances of the sensors output features analyzed in the learning phase. 
They are used in some of the heirarchical networks implemntations to normalize the inputs after feature extraction. 
* shape.h, weights.h and ShapeAndWeights.java contain constants specifying the shape of the Neural Network used and the weights discovered in the learning phase.

These files must be copied into different implementations of the herarchical classifiers to update the parameters after the learning phase.