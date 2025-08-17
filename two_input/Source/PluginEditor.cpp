/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
Two_inputAudioProcessorEditor::Two_inputAudioProcessorEditor (Two_inputAudioProcessor& p)
: AudioProcessorEditor (&p), audioProcessor (p), driveLabel("DRIVE"), volLabel("LEVEL"), toneLabel("TONE"), buttonLabel("MODEL")
{
    setSize (800, 400);
    juce::LookAndFeel::setDefaultLookAndFeel(&myCustomLNF);
    
    
    //Gain slider info
    addAndMakeVisible(driveSlider);
    addAndMakeVisible(driveLabel);
    driveSliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (audioProcessor.apvts, "DRIVE", driveSlider);

    //Vol Slider info
    addAndMakeVisible(volSlider);
    addAndMakeVisible(volLabel);
    volSliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (audioProcessor.apvts, "VOLUME", volSlider);
    
    //Tone Slider info
    addAndMakeVisible(toneSlider);
    addAndMakeVisible(toneLabel);
    toneSliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (audioProcessor.apvts, "TONE", toneSlider);

    
    //Model Selector buttons
    TS9_model.setClickingTogglesState (true);
    Mini_model.setClickingTogglesState (true);

    TS9_model.setRadioGroupId (model_buttons);
    Mini_model.setRadioGroupId (model_buttons);
    
    addAndMakeVisible(TS9_model);
    addAndMakeVisible(buttonLabel);
    
    TS9_button_attachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment> (audioProcessor.apvts, "TS9", TS9_model);

    addAndMakeVisible(Mini_model);
    Mini_button_attachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment> (audioProcessor.apvts, "MINI", Mini_model);


    
    //to make the plugin resizable --> kinda wonky on Reaper?
    setResizable(true, true);
    getConstrainer()->setFixedAspectRatio(getWidth()/getHeight());
    
}

Two_inputAudioProcessorEditor::~Two_inputAudioProcessorEditor()
{
    juce::LookAndFeel::setDefaultLookAndFeel(nullptr);
}

//==============================================================================
void Two_inputAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
//    g.fillAll (juce::Colours::black);
    juce::Image background = juce::ImageCache::getFromMemory (BinaryData::green_jpeg, BinaryData::green_jpegSize);
    g.drawImageAt (background.rescaled(getWidth(), getHeight()), 0, 0);
    
    
    //Setting silver borders
    silver_borders(g);
    
    
}




void Two_inputAudioProcessorEditor::resized()
{
    auto sliderWidth = getWidth() * 0.2;
    auto sliderHeight = getHeight() * 0.4;
    auto heightOffset = getHeight()/2 - (sliderHeight/3.5);


    //update knobs
    driveSlider.setBounds(getWidth()/2 - sliderWidth, heightOffset, sliderWidth, sliderHeight);
    toneSlider.setBounds(getWidth()/2, heightOffset, sliderWidth, sliderHeight);
    volSlider.setBounds(getWidth()/2  + sliderWidth * 1.22, heightOffset, sliderWidth, sliderHeight);
    
    
    //update buttons
    TS9_model.setBounds(getWidth()*0.075, heightOffset + getHeight()*0.04, sliderWidth*0.8, sliderHeight*0.3);
    Mini_model.setBounds(getWidth()*0.075, heightOffset+(sliderHeight*0.3)*1.2 + getHeight()*0.04, sliderWidth*0.8, sliderHeight*0.3);
    
    
    //update knob titles
    auto labelW = getWidth() * 0.15;
    auto labelH = getHeight() * 0.15;
    
    driveLabel.setBounds(driveSlider.getBounds().getCentreX() - labelW/2,
                         getHeight() * 0.27,
                         labelW, labelH);
//
    toneLabel.setBounds(toneSlider.getBounds().getCentreX() - labelW/2,
                         getHeight() * 0.27,
                         labelW, labelH);
//
    volLabel.setBounds(volSlider.getBounds().getCentreX() - labelW/2,
                         getHeight() * 0.27,
                         labelW, labelH);
    
    buttonLabel.setBounds(TS9_model.getBounds().getCentreX() - labelW/2,
                         getHeight() * 0.27,
                         labelW, labelH);


    
    //update title fonts
    auto fontSize {getHeight() * 0.11};
    driveLabel.setFont(juce::FontOptions(fontSize));
    toneLabel.setFont(juce::FontOptions(fontSize));
    volLabel.setFont(juce::FontOptions(fontSize));
    buttonLabel.setFont(juce::FontOptions(fontSize));
    

}


void Two_inputAudioProcessorEditor::silver_borders (juce::Graphics& g)
{
    auto sliderWidth = getWidth() * 0.2;
    auto sliderHeight = (getHeight() * 0.4) * 1.5;
    auto heightOffset = (getHeight()/2 - (sliderHeight/3.5)) * 0.6;
    auto cornerSize {10};
    auto thickness2 {5};
    auto thickness1 {3};
    auto thickness3 {1};
    


    
    //small borders


//    g.setColour(juce::Colours::silver);
    g.setColour(juce::Colour((juce::uint8)0, (juce::uint8)212, (juce::uint8)144, (juce::uint8)255));
    g.drawRoundedRectangle(getWidth()/2 - sliderWidth, heightOffset, sliderWidth*2, sliderHeight, cornerSize, thickness1);
    g.drawRoundedRectangle(getWidth()/2 + getWidth()*0.445 - sliderWidth, heightOffset, sliderWidth, sliderHeight, cornerSize, thickness1);
    g.drawRoundedRectangle(getWidth()/2 - getWidth()*0.445, heightOffset, sliderWidth, sliderHeight, cornerSize, thickness1);
    
    g.setColour(juce::Colours::silver);
    g.drawRoundedRectangle(getWidth()/2 - sliderWidth, heightOffset, sliderWidth*2, sliderHeight, cornerSize, thickness3);
    g.drawRoundedRectangle(getWidth()/2 + getWidth()*0.445 - sliderWidth, heightOffset, sliderWidth, sliderHeight, cornerSize, thickness3);
    g.drawRoundedRectangle(getWidth()/2 - getWidth()*0.445, heightOffset, sliderWidth, sliderHeight, cornerSize, thickness3);


    
    
    
    //big border
    
    //point by point
    juce::Path border;
    juce::Rectangle<float> outerBorder (getLocalBounds().reduced(10, 40).toFloat());
    border.startNewSubPath({outerBorder.getX() + float(getWidth())*0.1f, outerBorder.getY()});
    border.lineTo(outerBorder.getTopLeft());
    border.lineTo(outerBorder.getBottomLeft());
    border.lineTo(outerBorder.getBottomRight());
    border.lineTo(outerBorder.getTopRight());
    border.lineTo(outerBorder.getX() + getWidth()*0.4f, outerBorder.getY());
    
//    g.setColour(juce::Colour((juce::uint8)0, (juce::uint8)212, (juce::uint8)144, (juce::uint8)255));

    
    g.setColour(juce::Colours::white);
    juce::PathStrokeType stroke (thickness2, juce::PathStrokeType::curved, juce::PathStrokeType::rounded);
    g.strokePath(border, stroke);
    


}
