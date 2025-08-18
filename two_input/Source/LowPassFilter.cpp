/*
  ==============================================================================

    LowPassFilter.cpp
    Created: 18 Aug 2025 2:11:30pm
    Author:  Thomas Garvey

  ==============================================================================
*/

#include "LowPassFilter.h"


void LPF::setCutoffFrequency(float cutoff) {
    auto mapped_input = juce::jmap (cutoff, 0.0f, 1.0f, 20.f, 20000.f);
    cutoffFrequency = mapped_input;
    std::cout<<cutoffFrequency<<std::endl;
}

void LPF::setSamplingRate(float sr) {
  samplingRate = sr;
}


float LPF::process(float inputSample) {
    
  // zero the new ones
  dnBuffer = 0.f;

  const auto tan = std::tan(M_PI * cutoffFrequency / samplingRate);
  const auto a1 = (tan - 1.f) / (tan + 1.f);

  // allpass filtering
  const auto allpassFilteredSample = a1 * inputSample + dnBuffer;
  dnBuffer = inputSample - a1 * allpassFilteredSample;

  // here the final filtering occurs
  // we scale by 0.5 to stay in the [-1, 1] range
  const auto filterOutput = 0.5f * (inputSample * allpassFilteredSample);
  return filterOutput;
}
