/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "CustomLookAndFeel.h"
//==============================================================================
/**
*/
class DistortionAudioProcessorEditor  : public juce::AudioProcessorEditor,public juce::Slider::Listener,public juce::ComboBox::Listener, public juce::Button::Listener
{
public:
    DistortionAudioProcessorEditor (DistortionAudioProcessor&);
    ~DistortionAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
    void sliderValueChanged(juce::Slider* slider) override;
    void comboBoxChanged(juce::ComboBox* comboBox) override;
    void buttonClicked(juce::Button* button) override;
    float randomGenerator(float min, float max);
private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    DistortionAudioProcessor& audioProcessor;
    CustomLookAndFeel customLookAndFeel;
    
    juce::Slider lowdriveslider;
    juce::Slider middriveslider;
    juce::Slider highdriveslider;
    
    juce::Slider mixslider;
    juce::Slider outputslider;
    juce::ComboBox typeBoxlow;
    juce::ComboBox typeBoxmid;
    juce::ComboBox typeBoxhigh;
    
    juce::Slider lowcutslider;
    juce::Slider peakcenterslider;
    juce::Slider highcutslider;
    
    juce::Label lowdrivelabel;
    juce::Label middrivelabel;
    juce::Label highdrivelabel;
    juce::Label mixlabel;
    juce::Label outputlabel;
    juce::Label lowcutlabel;
    juce::Label peaklabel;
    juce::Label highcutlabel;
    
    juce::TextButton randomButton {"Random"};
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DistortionAudioProcessorEditor)
};
