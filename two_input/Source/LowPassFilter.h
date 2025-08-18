/*
  ==============================================================================

    LowPassFilter.h
    Created: 18 Aug 2025 2:11:30pm
    Author:  Thomas Garvey

  ==============================================================================
*/

#pragma once
#include <vector>
#include "JuceHeader.h"
#include <math.h>

class LPF
{
public:
    
    void setCutoffFrequency(float cutoffFrequency);
    void setSamplingRate(float samplingRate);
    float process(float inputSample);
    
private:
    float cutoffFrequency;
    float samplingRate;
    float dnBuffer;
};

