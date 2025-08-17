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
    
//    juce::MemoryInputStream jsonStream (BinaryData::ts_mini_json, BinaryData::ts_mini_jsonSize, false);
//    auto jsonInput = nlohmann::json::parse (jsonStream.readEntireStreamAsString().toStdString());
//    neuralNet[0].parseJson (jsonInput);
//    neuralNet[1].parseJson (jsonInput);
    
    model_pointer = std::make_unique<const char*> (TS9_data);
    modelSize_pointer = std::make_unique<const int> (TS9_dataSize);
    
    juce::MemoryInputStream jsonStream (*model_pointer, *modelSize_pointer, false);
    auto jsonInput = nlohmann::json::parse (jsonStream.readEntireStreamAsString().toStdString());
    neuralNet[0].parseJson (jsonInput);
    neuralNet[1].parseJson (jsonInput);
    
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
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    std::cout << "preparetoplay called" <<std::endl;
    
    juce::MemoryInputStream jsonStream (*model_pointer, *modelSize_pointer, false);
    auto jsonInput = nlohmann::json::parse (jsonStream.readEntireStreamAsString().toStdString());
    neuralNet[0].parseJson (jsonInput);
    neuralNet[1].parseJson (jsonInput);
    
    neuralNet[0].reset();
    neuralNet[1].reset();
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
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
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
    juce::ScopedNoDenormals noDenormals;
    auto d {apvts.getRawParameterValue("DRIVE")};
    auto drive = d->load();
    
    auto v {apvts.getRawParameterValue("VOLUME")};
    auto volume = v->load();
    
    auto ts {apvts.getRawParameterValue("TS9")};
    auto TS9_b = ts->load();
    if(TS9_b){
        model_pointer = std::make_unique<const char*> (TS9_data);
        modelSize_pointer = std::make_unique<const int> (TS9_dataSize);
    }
    else{
        model_pointer = std::make_unique<const char*> (Mini_data);
        modelSize_pointer = std::make_unique<const int> (Mini_dataSize);
    }
    if(prev_TS9_b != TS9_b){
        prepareToPlay(getSampleRate(), buffer.getNumSamples());
    }
    prev_TS9_b = TS9_b;
    
    
    for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
    {
        auto* x = buffer.getWritePointer (ch);
        for (int n = 0; n < buffer.getNumSamples(); ++n)
        {
            float input[] = { x[n], drive }; //needs to be a pointer to a float (i.e. a single array?)
            
            x[n] = (neuralNet[ch].forward(input)) * volume;

        }
    }
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
    params.push_back(std::make_unique<juce::AudioParameterFloat> (juce::ParameterID("TONE", 3), "tone", 0.0f, 1.0f, 1.0f));
    params.push_back(std::make_unique<juce::AudioParameterBool> (juce::ParameterID("TS9", 4), "ts9", true));
    params.push_back(std::make_unique<juce::AudioParameterBool> (juce::ParameterID("MINI", 5), "mini", false));
    return {params.begin(), params.end()};
}
