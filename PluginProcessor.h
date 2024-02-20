/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "Mu45FilterCalc/Mu45FilterCalc.h"
#include "StkLite-4.6.2/BiQuad.h"
//==============================================================================
/**
*/
class DistortionAudioProcessor  : public juce::AudioProcessor
                            #if JucePlugin_Enable_ARA
                             , public juce::AudioProcessorARAExtension
                            #endif
{
public:
    //==============================================================================
    DistortionAudioProcessor();
    ~DistortionAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;
    
    
    enum DistortionType{
        ArctanDistortion = 1,
        HardClipping = 2,
        SoftClipping = 3
    };
    void setDistortionTypelow(int type);
    void setDistortionTypemid(int type);
    void setDistortionTypehigh(int type);
    DistortionType getDistortionTypelow();
    DistortionType getDistortionTypemid();
    DistortionType getDistortionTypehigh();

private:
    //==============================================================================
    juce::AudioParameterFloat* lowdrive;
    juce::AudioParameterFloat* middrive;
    juce::AudioParameterFloat* highdrive;
    juce::AudioParameterInt* lowdistortionType;
    juce::AudioParameterInt* middistortionType;
    juce::AudioParameterInt* highdistortionType;
    
    juce::AudioParameterFloat* output;
    juce::AudioParameterFloat* mix;
    
    juce::AudioParameterFloat* lowcutoff;
    juce::AudioParameterFloat* peakcenter;
    juce::AudioParameterFloat* highcutoff;
    float lowdriveLinear;
    float middriveLinear;
    float highdriveLinear;
    
    float outputLinear;
    
    void updatefilter();
    float mFs; // Sampling rate
    stk::BiQuad highpassL,highpassR; //high pass
    stk::BiQuad lowpassL,lowpassR; // low pass
    stk::BiQuad peakFilterL,peakFilterR; // peak
    
    juce::dsp::LinkwitzRileyFilter<float> crossoverLowHighL, crossoverLowHighR;
    juce::dsp::LinkwitzRileyFilter<float> crossoverMidHighL, crossoverMidHighR;

    juce::dsp::LinkwitzRileyFilter<float> midBandLowPassFilterL, midBandLowPassFilterR;
    juce::dsp::LinkwitzRileyFilter<float> midBandHighPassFilterL, midBandHighPassFilterR;

    void calcAlgorithmParams();
    DistortionType currentDistortionTypelow = ArctanDistortion;
    DistortionType currentDistortionTypemid = ArctanDistortion;
    DistortionType currentDistortionTypehigh = ArctanDistortion;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DistortionAudioProcessor)
};
