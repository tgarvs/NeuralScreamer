
# Neural Screamer
This is a Github repository corresponding with the Master's Thesis, "Black & White: A Comparative Analysis of Virtual Analog Modeling for Real Time Audio." 
This repository contains an emulation of an Ibanez TS9 Tubescreamer pedal made via Deep Learning. The purpose of this thesis is to compare emulation efficacy and real time capabilities of Neural Network virtual analog modeling against a component based technique -- the DK-Method.


## Included Files
    - Python: Includes the dataset preprocessing and Keras model scripts
    - Audio: All audio data in various states and formats
    - Model Export: Exported weights/biases/architectures ready to use with RTNeural
    - Plugin: JUCE plugin ready for build


## How it is Made
Virtual analog modeling is the general process of turning hardware into software. 'White box' is the general term for modeling techniques where the system is known. These generally involve making a unique system of equations to represent the components of a circuit and how they influence in input signal. 'Black Box' techniques are the opposite, where the system is unknown. In recent years techniques involving Deep Learning has grown in popularity. 
Many audio systems are straightforward to model, but can become increasingly difficult when nonlinear components (diodes, vacuum tubes, triodes, etc) are included. Guitar distortion effects rely heavily on these components, and so have become a source of study for many researchers in the field. Neural networks have shown to model nonlinearities well, while being agnostic to the approximations and difficulties common to many white box techniques.


### Circuit Modeling
For this thesis, the white box emulation is a derivation of Eric Tarr's Tubescreamer clone. He uses the Discrete-Kirchoff (DK) Method, which is an extension of Modified Nodal Analysis. This method was chosen because it avoids various domain transfers by substituting discrete component equations directly during Nodal Analysis.


### Deep Learning
The Neural Network architecture for this thesis was established by Alec Wright in, []. One secondary objective of this thesis was to determine if successful plugin companies like Neural DSP, who use this same architecture, are actually creating better products or if they are using Neural Networks as both a marketing ploy and a way to make worse products, but quicker. 
This thesis found that Wright's architecture did provide a more baseline faithful emulation through subjective listening and playing tests.
Lastly, the model weights were exported and ran as a real time inferencing machine within a JUCE framework using Jatin Chowdhury's RTNeural.


## Installation  From Source


## Future Work
The following updates need to be included for this plugin:

        - Including a tone knob to match the TS9
        - Smooth knobs
        - Updating UI to be more pleasing, unique, & professional
        - Safeguard no output when silent
        - Allow dropdown menu to allow for user to select from various TS models (i.e. TS mini)
        - Prep for release
