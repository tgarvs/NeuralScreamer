/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
Two_inputAudioProcessor::Two_inputAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ), apvts(*this, nullptr, "Parameters", createParams())

#endif
{
    //Load model 1
    juce::MemoryInputStream jsonStream1 (BinaryData::ts_nine_json, BinaryData::ts_nine_jsonSize, false);
    auto jsonInput1 = nlohmann::json::parse (jsonStream1.readEntireStreamAsString().toStdString());
    neuralNet9[0].parseJson (jsonInput1);
    neuralNet9[1].parseJson (jsonInput1);
    

    //Load model 2
    juce::MemoryInputStream jsonStream2 (BinaryData::ts_mini_json, BinaryData::ts_mini_jsonSize, false);
    auto jsonInput2 = nlohmann::json::parse (jsonStream2.readEntireStreamAsString().toStdString());
    neuralNetMini[0].parseJson (jsonInput2);
    neuralNetMini[1].parseJson (jsonInput2);
    
    
    //Set intial model for pointer
//    model_ptr = neuralNet9;
    
}

Two_inputAudioProcessor::~Two_inputAudioProcessor()
{
}

//==============================================================================
const juce::String Two_inputAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool Two_inputAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool Two_inputAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool Two_inputAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double Two_inputAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int Two_inputAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int Two_inputAudioProcessor::getCurrentProgram()
{
    return 0;
}

void Two_inputAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String Two_inputAudioProcessor::getProgramName (int index)
{
    return {};
}

void Two_inputAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void Two_inputAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
//    juce::FloatVectorOperations::disableDenormalisedNumberSupport();

    neuralNet9[0].reset();
    neuralNet9[1].reset();
    
    neuralNetMini[0].reset();
    neuralNetMini[1].reset();
    
    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = getMainBusNumInputChannels();

    filter.reset();
    filter.prepare(spec);
    filter.setType(filtertype);
}



void Two_inputAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool Two_inputAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void Two_inputAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    static double worstMs = 0;
    const auto t0 = juce::Time::getHighResolutionTicks();
    
    
    juce::ScopedNoDenormals noDenormals;
    
 
    //Read drive knob
    auto d {apvts.getRawParameterValue("DRIVE")};
    auto drive = d->load();
    
    //Read volume knob
    auto v {apvts.getRawParameterValue("VOLUME")};
    auto volume = v->load();
    
    auto c = apvts.getRawParameterValue("TONE")->load();
    filter.setCutoffFrequency(c);
    
    //Check to see which button is on and update model if changes
    auto ts {apvts.getRawParameterValue("TS9")};
    auto TS9_b = ts->load();
    
    auto& nets = TS9_b ? neuralNet9 : neuralNetMini;
   
    
    //process samples
    for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
    {
        auto* x = buffer.getWritePointer (ch);
        for (int n = 0; n < buffer.getNumSamples(); ++n)
        {
            float input[] = { x[n], drive }; //needs to be a pointer to a float (i.e. a single array?)
            auto y = nets[ch].forward(input);
            x[n] = y * (volume * 0.9);
        }
    }
    

    const auto t1 = juce::Time::getHighResolutionTicks();
    const auto ms = juce::Time::highResolutionTicksToSeconds(t1 - t0) * 1000.0;
    if (ms > worstMs) { worstMs = ms; DBG("processBlock ms worst=" << worstMs); }
    
    
    //lowpass filtering
    auto block = juce::dsp::AudioBlock<float> {buffer};
    auto context = juce::dsp::ProcessContextReplacing<float>(block);
    filter.process(context);
    
}




//==============================================================================
bool Two_inputAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* Two_inputAudioProcessor::createEditor()
{
    return new Two_inputAudioProcessorEditor (*this);
}

//==============================================================================
void Two_inputAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void Two_inputAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new Two_inputAudioProcessor();
}


juce::AudioProcessorValueTreeState::ParameterLayout Two_inputAudioProcessor::createParams(){


    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;
    params.push_back(std::make_unique<juce::AudioParameterFloat> (juce::ParameterID("DRIVE", 1), "drive", 0.0f, 1.0f, 0.5f));
    params.push_back(std::make_unique<juce::AudioParameterFloat> (juce::ParameterID("VOLUME", 2), "volume", 0.0f, 1.5f, 1.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat> (juce::ParameterID("TONE", 3), "tone", juce::NormalisableRange<float>(20.0f, 20000.0f, 1.0f, 0.4f), 20000.0f));
    params.push_back(std::make_unique<juce::AudioParameterBool> (juce::ParameterID("TS9", 4), "ts9", true));
    params.push_back(std::make_unique<juce::AudioParameterBool> (juce::ParameterID("MINI", 5), "mini", false));
    return {params.begin(), params.end()};
}




void Two_inputAudioProcessor::reset()
{
    filter.reset();
}


