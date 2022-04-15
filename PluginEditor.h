#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================

class CircularBufferAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    CircularBufferAudioProcessorEditor (CircularBufferAudioProcessor&);
    ~CircularBufferAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    CircularBufferAudioProcessor& audioProcessor;
    void setGainSlider();
    void setRepeatsSlider();
    juce::ImageComponent mImageComponent;
    juce::Slider gainSlider;
    juce::Label gainLabel;
    juce::Slider repeatsSlider;
    juce::Label repeatsLabel;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> gainSliderAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> repeatsSliderAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CircularBufferAudioProcessorEditor)
};
