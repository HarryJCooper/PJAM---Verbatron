#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
CircularBufferAudioProcessor::CircularBufferAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ), apvts(*this, nullptr, "Parameters", createParameters())
#endif
{
}

CircularBufferAudioProcessor::~CircularBufferAudioProcessor()
{
}

//==============================================================================
const juce::String CircularBufferAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool CircularBufferAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool CircularBufferAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool CircularBufferAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double CircularBufferAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int CircularBufferAudioProcessor::getNumPrograms()
{
    return 1;
}

int CircularBufferAudioProcessor::getCurrentProgram()
{
    return 0;
}

void CircularBufferAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String CircularBufferAudioProcessor::getProgramName (int index)
{
    return {};
}

void CircularBufferAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void CircularBufferAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    auto delayBufferSize = sampleRate * 2.0f;
    delayBuffer.setSize(getTotalNumOutputChannels(), (int)delayBufferSize);
}

void CircularBufferAudioProcessor::releaseResources()
{
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool CircularBufferAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void CircularBufferAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();
    
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());
    
    auto delayTime = apvts.getRawParameterValue("DELAY_TIME");
    float dT = delayTime->load();
    
    auto repeats = apvts.getRawParameterValue("REPEATS");
    int reps = repeats->load();
    // just determiing our buffer size and delay buffer size
    auto bufferSize = buffer.getNumSamples();
    auto delayBufferSize = delayBuffer.getNumSamples();
    
    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        auto* channelData = buffer.getWritePointer (channel);
        // copy input signal to a delay buffer
        for (int numOfReps = 0; numOfReps < reps; ++numOfReps){
            fillBuffer(bufferSize, delayBufferSize, channel, channelData);
            readFromBuffer(buffer, delayBuffer, bufferSize, delayBufferSize, channel, dT);
        }
    }
    
    writePosition += buffer.getNumSamples();
    writePosition %= delayBufferSize;
}

void CircularBufferAudioProcessor::fillBuffer(int bufferSize, int delayBufferSize, int channel, float* channelData)
{
    // Check to see if main buffer copies ot delay buffer without needing to wrap.
    if (delayBufferSize > (bufferSize + writePosition))
    {
        // copy main buffer contents to delay buffer
        delayBuffer.copyFromWithRamp(channel, writePosition, channelData, bufferSize, 0.1f, 0.1f);
    }
    else
    {
        // determine how much space is left at the end of the delay buffer
        auto numSamplesToEnd = delayBufferSize - writePosition;
        // copy that amount of contents to the end...
        delayBuffer.copyFromWithRamp(channel, writePosition, channelData, numSamplesToEnd, 0.1f, 0.1f);
        // calculate how much contents is remaining to copy
        auto numSamplesAtStart = bufferSize - numSamplesToEnd;
        // copy remaining amount to beginning of delay buffer
        delayBuffer.copyFromWithRamp(channel, 0, channelData + numSamplesToEnd, numSamplesAtStart, 0.1f, 0.1f);
    }
}

void CircularBufferAudioProcessor::readFromBuffer(juce::AudioBuffer<float>& buffer, juce::AudioBuffer<float>& delayBuffer, int bufferSize, int delayBufferSize, int channel, float delayTime)
{
    auto readPosition = writePosition - (getSampleRate() * delayTime);
    
    if (readPosition < 0)
        readPosition += delayBufferSize;
    
    
    if (readPosition + bufferSize < delayBufferSize)
    {
        buffer.addFromWithRamp(channel, 0, delayBuffer.getReadPointer(channel, readPosition), bufferSize, 0.7f, 0.7f);
    }
    else
    {
        auto numSamplesToEnd = delayBufferSize - readPosition;
        buffer.addFromWithRamp(channel, 0, delayBuffer.getReadPointer(channel, readPosition), numSamplesToEnd, 0.7f, 0.7f);
        
        auto numSamplesAtStart = bufferSize - numSamplesToEnd;
        buffer.addFromWithRamp(channel, numSamplesToEnd, delayBuffer.getReadPointer(channel, 0), numSamplesAtStart, 0.7f, 0.7f);
    }
}

//==============================================================================
bool CircularBufferAudioProcessor::hasEditor() const
{
    return true;
}

juce::AudioProcessorEditor* CircularBufferAudioProcessor::createEditor()
{
    return new CircularBufferAudioProcessorEditor (*this);
}

//==============================================================================
void CircularBufferAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // TODO
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void CircularBufferAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // TODO
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new CircularBufferAudioProcessor();
}

juce::AudioProcessorValueTreeState::ParameterLayout CircularBufferAudioProcessor::createParameters()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;
    
    params.push_back(std::make_unique<juce::AudioParameterFloat>("DELAY_TIME", "Delay Time", 0.0f, 1.0f, 0.5f));
    params.push_back(std::make_unique<juce::AudioParameterInt>("REPEATS", "Decay Time", 0, 10, 4));
    return {params.begin(), params.end()};
}
