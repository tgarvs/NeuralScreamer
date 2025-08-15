/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class Two_inputAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    Two_inputAudioProcessorEditor (Two_inputAudioProcessor&);
    ~Two_inputAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    Two_inputAudioProcessor& audioProcessor;
    
    juce::Slider driveSlider;
    juce::Label driveLabel;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> driveSliderAttachment;
    
    juce::Slider volSlider;
    juce::Label volLabel;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> volSliderAttachment;
    
//    juce::DrawableRectangle outline_one;


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Two_inputAudioProcessorEditor)
};
