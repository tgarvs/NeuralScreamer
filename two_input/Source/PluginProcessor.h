/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#define RTNEURAL_USE_EIGEN 1
#define RTNEURAL_DEFAULT_ALIGNMENT 16
#include <JuceHeader.h>
#include "RTNeural.h"

//==============================================================================
/**
*/
class Two_inputAudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    Two_inputAudioProcessor();
    ~Two_inputAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;
    

    juce::AudioProcessorValueTreeState apvts;
    juce::AudioProcessorValueTreeState::ParameterLayout createParams();

private:
    //==============================================================================
//    RTNeural::ModelT<float, 2, 64,
//        RTNeural::Conv1DT<float, 2, 36, 12, 1>,
//        RTNeural::Conv1DT<float, 36, 36, 12, 1>,
//        RTNeural::LSTMLayerT<float, 36, 96>,
//        RTNeural::DenseT<float, 96, 64>
//    > neuralNet[2];
    
    RTNeural::ModelT<float, 2, 1,
    // LSTM<inSz,hiddenSz>
    RTNeural::LSTMLayerT<float, 2, 64>,
    // Dense<inSz,outSz>
    RTNeural::DenseT<float, 64, 1>
    > neuralNet[2];
    




    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Two_inputAudioProcessor)
};
