/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
Two_inputAudioProcessorEditor::Two_inputAudioProcessorEditor (Two_inputAudioProcessor& p)
: AudioProcessorEditor (&p), audioProcessor (p), driveLabel("DRIVE"), volLabel("LEVEL"), toneLabel("TONE"), buttonLabel("MODEL"), title("NEURAL SCREAMER")
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
    addAndMakeVisible(TS9_model);
    addAndMakeVisible(buttonLabel);
    TS9_button_attachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment> (audioProcessor.apvts, "TS9", TS9_model);

    addAndMakeVisible(Mini_model);
    Mini_button_attachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment> (audioProcessor.apvts, "MINI", Mini_model);


    //title
    addAndMakeVisible(title);
    
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
    juce::Image background = juce::ImageCache::getFromMemory (BinaryData::green3_jpg, BinaryData::green3_jpgSize);
    g.drawImageAt (background.rescaled(getWidth(), getHeight()), 0, 0);
    
    //Setting silver borders
    silver_borders(g);
}



void Two_inputAudioProcessorEditor::resized()
{
    auto sliderWidth = getWidth() * 0.2;
    auto sliderHeight = getHeight() * 0.4;
    auto heightOffset = getHeight() * 0.42;


    //update knobs
    driveSlider.setBounds(getWidth()/2 - sliderWidth, heightOffset, sliderWidth, sliderHeight);
    toneSlider.setBounds(getWidth()/2, heightOffset, sliderWidth, sliderHeight);
    volSlider.setBounds(getWidth()/2  + sliderWidth * 1.22, heightOffset, sliderWidth, sliderHeight);
    
    
    //update buttons
    auto buttonW = sliderWidth*0.8;
    auto buttonH = sliderHeight*0.3;
    
    TS9_model.setBounds(getWidth()*0.075, heightOffset + getHeight()*0.04, buttonW, buttonH);
    Mini_model.setBounds(getWidth()*0.075, heightOffset+(sliderHeight*0.3)*1.2 + getHeight()*0.04, buttonW, buttonH);
    
    
    //update knob titles
    auto labelW = getWidth() * 0.15;
    auto labelH = getHeight() * 0.15;
    float labelY = getHeight() * 0.29;
    
    driveLabel.setBounds(driveSlider.getBounds().getCentreX() - labelW/2,
                         labelY,
                         labelW, labelH);
    toneLabel.setBounds(toneSlider.getBounds().getCentreX() - labelW/2,
                        labelY,
                         labelW, labelH);
    volLabel.setBounds(volSlider.getBounds().getCentreX() - labelW/2,
                       labelY,
                         labelW, labelH);
    buttonLabel.setBounds(TS9_model.getBounds().getCentreX() - labelW/2,
                          labelY,
                         labelW, labelH);
    
    
    //update  fonts
    auto fontSize {getHeight() * 0.11};
    driveLabel.setFont(juce::FontOptions(fontSize));
    toneLabel.setFont(juce::FontOptions(fontSize));
    volLabel.setFont(juce::FontOptions(fontSize));
    buttonLabel.setFont(juce::FontOptions(fontSize));
    
    
 
    //Title
    title.setBounds(getWidth()/2 - labelW*(2.5/2), getHeight() * 0.019, labelW*2.54, labelH*1.5);
    title.setColour(juce::Label::textColourId,
                    juce::Colours::white);
    title.setFont(juce::FontOptions(fontSize*1.1));
    
}


void Two_inputAudioProcessorEditor::silver_borders (juce::Graphics& g)
{
    auto sliderHeight = (getHeight() * 0.4) * 1.5;
    auto heightOffset = driveSlider.getBounds().getY() - (driveSlider.getBounds().getHeight() * 0.45);
    auto cornerSize {10};
    auto thickness2 {8};
    auto thickness1 {5};

    
    //small borders
    g.setColour(juce::Colours::white);
    g.drawRoundedRectangle(driveSlider.getBounds().getCentreX() - driveSlider.getBounds().getWidth()/2,
                           heightOffset,
                           driveSlider.getBounds().getWidth() * 2,
                           sliderHeight, cornerSize, thickness1);
    g.drawRoundedRectangle(TS9_model.getBounds().getCentreX() - TS9_model.getBounds().getWidth()*0.62,
                           heightOffset,
                           driveSlider.getBounds().getWidth(),
                           sliderHeight, cornerSize, thickness1);
    g.drawRoundedRectangle(volSlider.getBounds().getCentreX() - driveSlider.getBounds().getWidth()/2,
                           heightOffset,
                           volSlider.getBounds().getWidth(),
                           sliderHeight, cornerSize, thickness1);
    
    
    //big border
    juce::Path border;

    float w = getWidth();
    float h = getHeight();
    float WLmult = 0.013f;
    float WRmult = 1-WLmult;
    float HTmult = 0.12f;
    float HBmult = 1-0.08f;//HTmult;
    
    border.startNewSubPath({w*0.3f, h*HTmult});
    border.lineTo(w*WLmult, h*HTmult);
    border.lineTo(w*WLmult, h*HBmult);
    border.lineTo(w*WRmult, h*HBmult);
    border.lineTo(w*WRmult, h*HTmult);
    border.lineTo(w*0.7f, h*HTmult);

    juce::ColourGradient grad (juce::Colours::silver, {float(getWidth())/2.f, float(getHeight()) + 200}, juce::Colours::white, {float(getWidth())/2.f, 0}, false);
    g.setGradientFill(grad);
    juce::PathStrokeType stroke (thickness2, juce::PathStrokeType::curved, juce::PathStrokeType::rounded);
    g.strokePath(border, stroke);
    
}
