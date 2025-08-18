/*
  ==============================================================================

    Components.cpp
    Created: 15 Aug 2025 6:11:46pm
    Author:  Thomas Garvey

  ==============================================================================
*/

#include "Components.h"

CustomLNF::CustomLNF()
{
    auto newFont = juce::Typeface::createSystemTypefaceFor (BinaryData::Schluber_ttf, BinaryData::Schluber_ttfSize);
    setDefaultSansSerifTypeface (newFont);
}




void  CustomLNF::drawRotarySlider (juce::Graphics &g, int x, int y, int width, int height, float sliderPos, float rotaryStartAngle, float rotaryEndAngle, juce::Slider &slider)
{
    auto bounds = juce::Rectangle<int> (x, y, width, height).toFloat().reduced (10);
    auto radius = juce::jmin (bounds.getWidth(), bounds.getHeight()) / 2.2f;
    auto toAngle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);
    auto lineW = juce::jmin (8.0f, radius * 0.5f); //basically the stroke
    auto arcRadius = radius - lineW * 0.5f;


    //path to hold circle shape
    juce::Path knobPath;
    
    //add circle
    knobPath.addEllipse(bounds.getCentreX()-arcRadius, bounds.getCentreY()-arcRadius, arcRadius*2, arcRadius*2);

    //draw drop shadow
    juce::DropShadow shadow (juce::Colours::black, 15, {15, 15});
    shadow.drawForPath(g, knobPath);
    
    //fill the circle
    g.setColour (juce::Colours::white);
    g.fillPath(knobPath);

    //add a stroke to it
    g.setColour (juce::Colours::silver);
    g.strokePath (knobPath, juce::PathStrokeType (5.0f));
    
    
    //Level dots via trig
    g.setColour (juce::Colours::white);
    for(int i = 0; i <11; ++i)
    {
        const float t  = juce::jmap (float(i), 0.0f, 10.f, rotaryStartAngle, rotaryEndAngle);
        float x  = bounds.getCentreX() - (arcRadius+10) * std::sin (t) - 0.5;
        float y  = bounds.getCentreY() - (arcRadius+10) * std::cos (t) - 0.5;
        g.drawEllipse(x, y, 1, 1, 2);
    }
    
    
    
//    //On part
//    if (slider.isEnabled())
//    {
//        juce::Path valueArc;
//        valueArc.addCentredArc (bounds.getCentreX(),
//                                bounds.getCentreY(),
//                                arcRadius,
//                                arcRadius,
//                                0.0f,
//                                rotaryStartAngle,
//                                toAngle,
//                                true);
//
//        g.setColour (juce::Colour(10, 132, 227));
//        g.strokePath (valueArc, juce::PathStrokeType (lineW, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
//    }

    
    //handles the line
    juce::Path tick;
    juce::Point<float> thumbPoint (bounds.getCentreX() + arcRadius * std::cos (toAngle - juce::MathConstants<float>::halfPi),
                             bounds.getCentreY() + arcRadius * std::sin (toAngle - juce::MathConstants<float>::halfPi));
    juce::Point<float> start (bounds.getCentreX(), bounds.getCentreY());
    

    tick.startNewSubPath(start);
    tick.lineTo(thumbPoint);
    juce::PathStrokeType stroke (5, juce::PathStrokeType::JointStyle::curved, juce::PathStrokeType::EndCapStyle::rounded);
    
    
    g.setColour (juce::Colours::black);
    g.strokePath(tick, stroke);
    
    
    auto fontSize = (float)slider.getHeight() * 0.05f;
    g.setFont(fontSize);
}





juce::Label* CustomLNF::createSliderTextBox (juce::Slider& slider)
{
    juce::Label* l = juce::LookAndFeel_V4::createSliderTextBox (slider);
    l->setColour (juce::Label::ColourIds::outlineColourId, juce::Colours::transparentBlack);
    l->setJustificationType (juce::Justification::centred);
    l->setAlpha(0.75);
    
    auto fontSize = (float)slider.getParentHeight() * 0.015f;
    l->setFont(juce::FontOptions(fontSize));
   
    return l;
   
}






void CustomLNF::drawToggleButton (juce::Graphics& g, juce::ToggleButton& button, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown)
{
    juce::Rectangle<float> newBounds (button.getLocalBounds().toFloat().reduced(1));
    juce::Path tickPath;
    auto fontSize = (float) newBounds.getHeight() * 0.4f;

    drawTickBox (g, button,
                 newBounds.getX(), newBounds.getY(), newBounds.getWidth(), newBounds.getHeight(),
                 button.getToggleState(),
                 button.isEnabled(),
                 shouldDrawButtonAsHighlighted,
                 shouldDrawButtonAsDown);

    g.setColour(juce::Colour((juce::uint8)0, (juce::uint8)212, (juce::uint8)144, (juce::uint8)255));
    g.setFont (fontSize);
    
//    g.drawFittedText (button.getButtonText(),
//                      button.getLocalBounds().withTrimmedLeft (button.getLocalBounds().getWidth()*0.05).withTrimmedTop(button.getLocalBounds().getHeight()*0.1),
//                      juce::Justification::centred, 10);
    
    g.drawFittedText (button.getButtonText(),
                      newBounds.toNearestInt().withTrimmedTop(newBounds.getHeight()*0.1),
                      juce::Justification::centred, 10);


    if (! button.isEnabled()){

    }
    
//    g.setColour(juce::Colours::black);
//    g.drawRect(button.getLocalBounds());
}




void CustomLNF::drawTickBox (juce::Graphics& g, juce::Component& component,
                                  float x, float y, float w, float h,
                                  const bool ticked,
                                  [[maybe_unused]] const bool isEnabled,
                                  [[maybe_unused]] const bool shouldDrawButtonAsHighlighted,
                                  [[maybe_unused]] const bool shouldDrawButtonAsDown)
{

    juce::Rectangle<float> tickBounds (x, y, w, h);
    juce::Path tickPath;
    
    
    if(component.isMouseOver()){
        tickPath.addRoundedRectangle(tickBounds, 10);
        
        juce::DropShadow shadow (juce::Colours::black, 20, {10, 10});
        shadow.drawForPath(g, tickPath);
        
        juce::DropShadow shadow2 (juce::Colours::black, 1, {10, 10});
        shadow.drawForPath(g, tickPath);
        
        g.setColour (juce::Colours::silver.withAlpha( (juce::uint8)155) );
        g.fillPath(tickPath);
        
        g.setColour (juce::Colours::silver);
        g.drawRoundedRectangle(tickBounds, 10, 1);
    }
    
    else{
        tickPath.addRoundedRectangle(tickBounds, 10);

        juce::DropShadow shadow (juce::Colours::black, 20, {10, 10});
        shadow.drawForPath(g, tickPath);
        
        juce::DropShadow shadow2 (juce::Colours::black, 1, {10, 10});
        shadow.drawForPath(g, tickPath);
        
        g.setColour (juce::Colours::white);
        g.fillPath(tickPath);
        
        g.setColour (juce::Colours::silver);
        g.drawRoundedRectangle(tickBounds, 10, 1);
    }
    

    auto radius {component.getLocalBounds().getWidth()*0.1};
    if (ticked)
    {
//        g.setColour (juce::Colours::white);
//        g.setColour(juce::Colour((juce::uint8)0, (juce::uint8)212, (juce::uint8)144, (juce::uint8)255));
//        g.setColour(juce::Colour((juce::uint8)247, (juce::uint8)32, (juce::uint8)39, (juce::uint8)255));
        g.setColour(juce::Colour((juce::uint8)3, (juce::uint8)112, (juce::uint8)179, (juce::uint8)255));
//        g.setColour(juce::Colour((juce::uint8)110, (juce::uint8)183, (juce::uint8)228, (juce::uint8)255));
        
        g.fillEllipse((tickBounds.getCentreX()-radius/2) - tickBounds.getWidth()*0.3, tickBounds.getCentreY()-radius/2, radius, radius);
    }
}




void CustomToggleButton::mouseEnter(const juce::MouseEvent & event)
{
    repaint();
}


void CustomToggleButton::mouseExit(const juce::MouseEvent & event)
{
    repaint();
}

