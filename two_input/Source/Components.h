/*
  ==============================================================================

    Components.h
    Created: 15 Aug 2025 6:11:46pm
    Author:  Thomas Garvey

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>


class CustomLNF : public juce::LookAndFeel_V4
{
public:
    CustomLNF();
    void  drawRotarySlider (juce::Graphics &g, int x, int y, int width, int height, float sliderPos, float rotaryStartAngle, float rotaryEndAngle, juce::Slider &slider) override;
    juce::Label* createSliderTextBox (juce::Slider& slider) override;
    
    void drawToggleButton (juce::Graphics& g, juce::ToggleButton& button, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override;
    void drawTickBox (juce::Graphics& g, juce::Component& component,
                                      float x, float y, float w, float h,
                                      const bool ticked,
                                      [[maybe_unused]] const bool isEnabled,
                                      [[maybe_unused]] const bool shouldDrawButtonAsHighlighted,
                                      [[maybe_unused]] const bool shouldDrawButtonAsDown) override;
};




class CustomDial : public juce::Slider
{
    
public:
    CustomDial()
    {
        setLookAndFeel(&LNF);
        setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
        setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, true, 50, 25);
    }
    
    ~CustomDial(){
        setLookAndFeel(nullptr);
    }
    
    
    
private:
    CustomLNF LNF;
};




class CustomLabel : public juce::Label
{
public:
    CustomLabel(const std::string& name)
    {
        setLookAndFeel(&LNF);
        setText(name, juce::dontSendNotification);
        setJustificationType(juce::Justification::centred);
        auto fontSize {50}; //getParentHeight() * 0.025};
        setFont(juce::FontOptions(fontSize));
        
    }
    
    ~CustomLabel()
    {
        setLookAndFeel(nullptr);
    }
    
private:
     CustomLNF LNF;
};



class CustomToggleButton : public juce::ToggleButton
{
public:
    CustomToggleButton(const int model_buttons, const std::string& text)
    : juce::ToggleButton(text), shadow(juce::DropShadow(juce::Colours::black, 15, {10, 10}))
    {
        setLookAndFeel(&LNF);
        setClickingTogglesState (true);
        setRadioGroupId (model_buttons);
        setButtonText(text);
        
        shadow.setOwner(this);
    }
    
    ~CustomToggleButton()
    {
        setLookAndFeel(nullptr);
    }
    
    void mouseEnter(const juce::MouseEvent & event) override;
    void mouseExit(const juce::MouseEvent & event) override;
    
private:
    CustomLNF LNF;
    bool mouseOver {false};
    juce::DropShadower shadow;
};
