/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
Two_inputAudioProcessorEditor::Two_inputAudioProcessorEditor (Two_inputAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (800, 400);
    
    //Gain slider info
    driveSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    driveSlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, true, 50, 25);
    addAndMakeVisible(driveSlider);
    driveSliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (audioProcessor.apvts, "DRIVE", driveSlider);
    
    driveLabel.setText("Drive", juce::dontSendNotification);
    driveLabel.attachToComponent(&driveSlider, false);
    driveLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(driveLabel);
    
    
    //Vol Slider info
    volSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    volSlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, true, 50, 25);
    addAndMakeVisible(volSlider);
    volSliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (audioProcessor.apvts, "VOLUME", volSlider);
    
    volLabel.setText("Volume", juce::dontSendNotification);
    volLabel.attachToComponent(&volSlider, false);
    volLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(volLabel);

    //to make the plugin resizable --> kinda wonky on Reaper?
    setResizable(true, true);
    getConstrainer()->setFixedAspectRatio(getWidth()/getHeight());
    
    
    //Outlines
//    outline_one.setFill(juce::Colours::blue);
//    outline_one.setStrokeFill(juce::Colours::pink);
//    outline_one.setStrokeThickness(5.f);
//    addAndMakeVisible(outline_one);
    
}

Two_inputAudioProcessorEditor::~Two_inputAudioProcessorEditor()
{
}

//==============================================================================
void Two_inputAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (juce::Colours::black);


}

void Two_inputAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    auto sliderWidth = getWidth() * 0.5;
    auto sliderHeight = getHeight() * 0.8;
    auto heightOffset = getHeight()/2 * 0.2;
    
    
    driveSlider.setBounds(getWidth()/2, heightOffset, sliderWidth, sliderHeight);
    volSlider.setBounds(getWidth()/2 - sliderWidth, heightOffset, sliderWidth, sliderHeight);
    
//    outline_one.setBounds(getWidth()/2, getHeight()/2, 100, 200);
//    outline_one.setRectangle (outline_one.getLocalBounds().toFloat().reduced (10.5f));

}
