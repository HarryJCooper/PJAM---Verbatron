#include "PluginProcessor.h"
#include "PluginEditor.h"
//==============================================================================
CircularBufferAudioProcessorEditor::CircularBufferAudioProcessorEditor (CircularBufferAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    auto pjImage = juce::ImageCache::getFromMemory(BinaryData::Logo_png, BinaryData::Logo_pngSize);
    if (!pjImage.isNull())
        mImageComponent.setImage(pjImage, juce::RectanglePlacement::fillDestination);
    else
        jassert (!pjImage.isNull());
    
    addAndMakeVisible(mImageComponent);
    setSize (500, 500);
}

void CircularBufferAudioProcessorEditor::setGainSlider()
{
    gainSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    gainSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 50, 20);
    gainSlider.setTextBoxIsEditable(true);
    gainSlider.setBounds(getWidth()/3 + 100, getHeight()/2 + 50, 150, 150);
    gainSlider.setColour(juce::Slider::thumbColourId, juce::Colours::purple);
    gainSlider.setColour(juce::Slider::textBoxTextColourId, juce::Colours::purple);
    gainSlider.setColour(juce::Slider::trackColourId, juce::Colours::peachpuff);
    gainSliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "DELAY_TIME", gainSlider);
    gainLabel.setColour(juce::Label::textColourId, juce::Colours::purple);
    gainLabel.setText("Delay", juce::dontSendNotification);
    gainLabel.setFont (20.0f);
    gainLabel.attachToComponent (&gainSlider, true);
    gainLabel.setBounds(getWidth()/3 + 145, getHeight()/2 + 30, 100, 20);
    addAndMakeVisible(gainSlider);
    
}

void CircularBufferAudioProcessorEditor::setRepeatsSlider()
{
    repeatsSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    repeatsSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 50, 20);
    repeatsSlider.setTextBoxIsEditable(true);
    repeatsSlider.setTitle("Decay");
    repeatsSlider.setHelpText("Decay Help Text");
    repeatsSlider.setBounds(getWidth()/3 - 100, getHeight()/2 + 50, 150, 150);
    repeatsSlider.setColour (juce::Slider::thumbColourId, juce::Colours::purple);
    repeatsSlider.setColour(juce::Slider::textBoxTextColourId, juce::Colours::purple);
    repeatsSlider.setColour(juce::Slider::trackColourId, juce::Colours::peachpuff);
    repeatsSliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "REPEATS", repeatsSlider);
    repeatsLabel.setColour(juce::Label::textColourId, juce::Colours::purple);
    repeatsLabel.setText("Decay", juce::dontSendNotification);
    repeatsLabel.setFont (20.0f);
    repeatsLabel.attachToComponent (&gainSlider, true);
    repeatsLabel.setBounds(getWidth()/3 - 55, getHeight()/2 + 30, 100, 20);
    addAndMakeVisible(repeatsSlider);
}

CircularBufferAudioProcessorEditor::~CircularBufferAudioProcessorEditor()
{
    gainSliderAttachment = nullptr;
}

//==============================================================================
void CircularBufferAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colours::whitesmoke);
    g.setColour (juce::Colours::purple);
    g.setFont (30.0f);
    setGainSlider();
    setRepeatsSlider();
}

void CircularBufferAudioProcessorEditor::resized()
{
    mImageComponent.setBoundsRelative(0.33, 0, 0.33, 0.33);
}
