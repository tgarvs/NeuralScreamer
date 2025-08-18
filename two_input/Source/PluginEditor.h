/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "Components.h"

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
    void silver_borders (juce::Graphics& g);
    

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    Two_inputAudioProcessor& audioProcessor;
    
    CustomLNF myCustomLNF;
    
    CustomDial driveSlider;
    CustomLabel driveLabel;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> driveSliderAttachment;
    
    CustomDial volSlider;
    CustomLabel volLabel;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> volSliderAttachment;
    
    CustomDial toneSlider;
    CustomLabel toneLabel;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> toneSliderAttachment;

    
    enum RadioButtonIds {
        model_buttons = 1001
    };
    

    CustomLabel buttonLabel;
    CustomToggleButton TS9_model {model_buttons, "TS9"};
    CustomToggleButton Mini_model {model_buttons, "Mini"};
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> TS9_button_attachment, Mini_button_attachment;
    
    
    CustomLabel title;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Two_inputAudioProcessorEditor)
};
