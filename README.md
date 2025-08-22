
# Neural Screamer
This is a Github repository corresponding with the Master's Thesis, [Black & White: A Comparative Analysis of Virtual Analog Modeling for Real Time Audio](https://github.com/tgarvs/NeuralScreamer/blob/main/Paper.pdf). 

This repository contains an emulation of an Ibanez TS9 Tube Screamer pedal made via Deep Learning. The purpose of this thesis is to compare emulation efficacy and real time capabilities of Neural Network virtual analog modeling against a component based technique -- the DK-Method.



![Screenshot of Plugin](https://github.com/tgarvs/NeuralScreamer/blob/main/neuralScreamerUI.png "Neural Screamer UI")



## Downloads

[Download for mac (10.14 or newer)](https://github.com/tgarvs/NeuralScreamer/releases/download/v.2.0.2/NeuralScreamerInstaller.pkg)
[Download for Windows](https://github.com/tgarvs/NeuralScreamer/releases/download/v.2.0.2/NeuralScreamerInstaller.pkg)



## Installation From Source
1. Download Source and the jucer project
2. Download [RTNeural](https://github.com/jatinchowdhury18/RTNeural)
3. Set header paths to the RTNeural .h file  and (optional) XSIMD library in jucer project globally and all exporters
4. Update build scheme to build in your daw of choice
5. Build and enjoy!



## Included Files
    - Python: Includes the dataset preprocessing and Keras model scripts
    - Audio: All audio data in various states and formats
    - Model Export: Exported weights/biases/architectures ready to use with RTNeural
    - Two input: Source and jucer project for the plugin



## How it's Made
Virtual analog modeling is the general process of turning hardware into software. 'White box' is the general term for modeling techniques where the system is known. These generally involve making a unique system of equations to represent the components of a circuit and how they influence in input signal. 'Black Box' techniques are the opposite, where the system is unknown. In recent years techniques involving Deep Learning has grown in popularity.

Many audio systems are straightforward to model, but can become increasingly difficult when nonlinear components (diodes, vacuum tubes, triodes, etc) are included. Guitar distortion effects rely heavily on these components, and so have become a source of study for many researchers in the field. Neural networks have shown to model nonlinearities well, while being agnostic to the approximations and difficulties common to many white box techniques.

### Circuit Modeling
For this thesis, the white box emulation is a derivation of Eric Tarr's [Tubescreamer clone](https://github.com/erictarrbelmont/TSPedal/tree/CompletedPlugin). He uses the Discrete-Kirchoff (DK) Method, which is an extension of Modified Nodal Analysis. This method was chosen because it avoids various domain transfers by substituting discrete component equations directly during Nodal Analysis.



### Deep Learning
The Neural Network architecture for this thesis was established by Alec Wright in this [paper](https://www.mdpi.com/2076-3417/10/3/766). One secondary objective of this thesis was to determine if successful plugin companies like Neural DSP, who use this same architecture, are actually creating better products or if they are using Neural Networks as both a marketing ploy and a way to make worse products, but quicker. 

This thesis found that Wright's architecture did provide a more baseline faithful emulation through subjective listening and playing tests.

Lastly, the model weights were exported and ran as a real time inferencing machine within a JUCE framework using Jatin Chowdhury's [RTNeural](https://github.com/jatinchowdhury18/RTNeural).
 


## Future Work
The following updates need to be included for this plugin:

        - Smooth knobs
        - Safeguard no output when silent in TS9

