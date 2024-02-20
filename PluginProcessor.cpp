/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
DistortionAudioProcessor::DistortionAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
    addParameter(lowdrive = new juce::AudioParameterFloat("low drive (db)", // parameterID,
            "low Drive", // parameterName,
            -36.0f, // minValue,
            36.0f, // maxValue,
            0.0f)); // defaultValue
    addParameter(middrive = new juce::AudioParameterFloat("mid drive (db)", // parameterID,
            "mid Drive", // parameterName,
            -36.0f, // minValue,
            36.0f, // maxValue,
            0.0f)); // defaultValue
    addParameter(highdrive = new juce::AudioParameterFloat("high drive (db)", // parameterID,
            "high Drive", // parameterName,
            -36.0f, // minValue,
            36.0f, // maxValue,
            0.0f)); // defaultValue
    addParameter(output = new juce::AudioParameterFloat("output (db)", // parameterID,
            "Output", // parameterName,
            -24.0f, // minValue,
            6.0f, // maxValue,
            0.0f)); // defaultValue
    addParameter(mix = new juce::AudioParameterFloat("mix percentage (%)", // parameterID,
            "Mix", // parameterName,
            0.0f, // minValue,
            100.0f, // maxValue,
            100.0f)); // defaultValue
    addParameter(lowdistortionType = new juce::AudioParameterInt("low Distortion type", // parameterID,
            "low Type", // parameterName,
            1, // minValue,
            3, // maxValue,
            1)); // defaultValue
    addParameter(middistortionType = new juce::AudioParameterInt("mid Distortion type", // parameterID,
            "mid Type", // parameterName,
            1, // minValue,
            3, // maxValue,
            1)); // defaultValue
    addParameter(highdistortionType = new juce::AudioParameterInt("high Distortion type", // parameterID,
            "high Type", // parameterName,
            1, // minValue,
            3, // maxValue,
            1)); // defaultValue
    addParameter(lowcutoff = new juce::AudioParameterFloat("lowcutoff (HZ)", // parameterID,
            "low cutoff", // parameterName,
            20.0f, // minValue,
            1000.0f, // maxValue,
            20.0f)); // defaultValue
    addParameter(highcutoff = new juce::AudioParameterFloat("highcutoff (HZ)", // parameterID,
            "high cutoff", // parameterName,
            1000.0f, // minValue,
            20000.0f, // maxValue,
            20000.0f)); // defaultValue
    addParameter(peakcenter = new juce::AudioParameterFloat("peakcenter (HZ)", // parameterID,
            "peak center", // parameterName,
            100.0f, // minValue,
            10000.0f, // maxValue,
            2000.0f)); // defaultValue
}

DistortionAudioProcessor::~DistortionAudioProcessor()
{
}

//==============================================================================
const juce::String DistortionAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool DistortionAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool DistortionAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool DistortionAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double DistortionAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int DistortionAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int DistortionAudioProcessor::getCurrentProgram()
{
    return 0;
}

void DistortionAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String DistortionAudioProcessor::getProgramName (int index)
{
    return {};
}

void DistortionAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void DistortionAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    mFs = sampleRate;
    // Set the crossover frequencies
    auto lowMidCrossoverFrequency = 200.0f; // Example value
    auto midHighCrossoverFrequency = 2000.0f; // Example value

    // Prepare the DSP module
    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = 1; // Prepare each filter for mono processing

    // Prepare and set up the low/mid crossover filters
    crossoverLowHighL.prepare(spec);
    crossoverLowHighR.prepare(spec);
    crossoverLowHighL.setType(juce::dsp::LinkwitzRileyFilter<float>::Type::lowpass);
    crossoverLowHighR.setType(juce::dsp::LinkwitzRileyFilter<float>::Type::lowpass);
    crossoverLowHighL.setCutoffFrequency(lowMidCrossoverFrequency);
    crossoverLowHighR.setCutoffFrequency(lowMidCrossoverFrequency);

    // Prepare and set up the mid/high crossover filters
    crossoverMidHighL.prepare(spec);
    crossoverMidHighR.prepare(spec);
    crossoverMidHighL.setType(juce::dsp::LinkwitzRileyFilter<float>::Type::highpass);
    crossoverMidHighR.setType(juce::dsp::LinkwitzRileyFilter<float>::Type::highpass);
    crossoverMidHighL.setCutoffFrequency(midHighCrossoverFrequency);
    crossoverMidHighR.setCutoffFrequency(midHighCrossoverFrequency);

    //band pass filters
    midBandLowPassFilterL.prepare(spec);
    midBandLowPassFilterR.prepare(spec);
    midBandHighPassFilterL.prepare(spec);
    midBandHighPassFilterR.prepare(spec);
    midBandLowPassFilterL.setType(juce::dsp::LinkwitzRileyFilter<float>::Type::lowpass);
    midBandLowPassFilterR.setType(juce::dsp::LinkwitzRileyFilter<float>::Type::lowpass);
    midBandHighPassFilterL.setType(juce::dsp::LinkwitzRileyFilter<float>::Type::highpass);
    midBandHighPassFilterR.setType(juce::dsp::LinkwitzRileyFilter<float>::Type::highpass);
    midBandLowPassFilterL.setCutoffFrequency(midHighCrossoverFrequency);
    midBandLowPassFilterR.setCutoffFrequency(midHighCrossoverFrequency);
    midBandHighPassFilterL.setCutoffFrequency(lowMidCrossoverFrequency);
    midBandHighPassFilterR.setCutoffFrequency(lowMidCrossoverFrequency);

}

void DistortionAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool DistortionAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
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
//------------------------get and set low band distortion type-----------------------------
DistortionAudioProcessor::DistortionType DistortionAudioProcessor::getDistortionTypelow()
{
    return currentDistortionTypelow;
}
void DistortionAudioProcessor::setDistortionTypelow(int type){
    if (type >= ArctanDistortion && type <= SoftClipping)
    {
        currentDistortionTypelow = static_cast<DistortionType>(type);
    }
}
//------------------------get and set mid band distortion type-----------------------------
DistortionAudioProcessor::DistortionType DistortionAudioProcessor::getDistortionTypemid()
{
    return currentDistortionTypemid;
}
void DistortionAudioProcessor::setDistortionTypemid(int type){
    if (type >= ArctanDistortion && type <= SoftClipping)
    {
        currentDistortionTypemid = static_cast<DistortionType>(type);
    }
}
//-----------------------get and set high band distortion type------------------------------
DistortionAudioProcessor::DistortionType DistortionAudioProcessor::getDistortionTypehigh()
{
    return currentDistortionTypehigh;
}

void DistortionAudioProcessor::setDistortionTypehigh(int type){
    if (type >= ArctanDistortion && type <= SoftClipping)
    {
        currentDistortionTypehigh = static_cast<DistortionType>(type);
    }
}
//-------------------------------convert db to linear-------------------------------------------


void DistortionAudioProcessor::updatefilter(){
    float hpcutoff = lowcutoff ->get();  //high pass filter
    float lpcutoff = highcutoff->get(); //low pass filter
    float peakf = peakcenter->get();
    
    float hpcoeffs[5];
    Mu45FilterCalc::calcCoeffsHPF(hpcoeffs, hpcutoff, 3, mFs);
    float lpcoeffs[5];
    Mu45FilterCalc::calcCoeffsLPF(lpcoeffs, lpcutoff, 3, mFs);
    float peakcoeffs[5];
    Mu45FilterCalc::calcCoeffsPeak(peakcoeffs, peakf, 6, 8, mFs);
    
    highpassL.setCoefficients(hpcoeffs[0], hpcoeffs[1], hpcoeffs[2], hpcoeffs[3], hpcoeffs[4]);
    highpassR.setCoefficients(hpcoeffs[0], hpcoeffs[1], hpcoeffs[2], hpcoeffs[3], hpcoeffs[4]);
    
    lowpassL.setCoefficients(lpcoeffs[0], lpcoeffs[1], lpcoeffs[2], lpcoeffs[3], lpcoeffs[4]);
    lowpassR.setCoefficients(lpcoeffs[0], lpcoeffs[1], lpcoeffs[2], lpcoeffs[3], lpcoeffs[4]);
    
    peakFilterL.setCoefficients(peakcoeffs[0],peakcoeffs[1],peakcoeffs[2],peakcoeffs[3],peakcoeffs[4]);
    peakFilterR.setCoefficients(peakcoeffs[0],peakcoeffs[1],peakcoeffs[2],peakcoeffs[3],peakcoeffs[4]);
    
}
void DistortionAudioProcessor::calcAlgorithmParams()
{
    //low drive
    float lowdriveDb = lowdrive->get();
    if (lowdriveDb <= -60.0){
        lowdriveLinear = 0.0;
    }else{
        lowdriveLinear = powf(10.0, lowdriveDb/20.0);
    }
    //mid drive
    float middriveDb = middrive->get();
    if (middriveDb <= -60.0){
        middriveLinear = 0.0;
    }else{
        middriveLinear = powf(10.0, middriveDb/20.0);
    }
    //high drive
    float highdriveDb = highdrive->get();
    if (highdriveDb <= -60.0){
        highdriveLinear = 0.0;
    }else{
        highdriveLinear = powf(10.0, highdriveDb/20.0);
    }
    //output
    float outputDb = output->get();
    if (outputDb <= -60.0){
        outputLinear = 0.0;
    }else{
        outputLinear = powf(10.0, outputDb/20.0);
    }
}
//distortion algorithm 
std::pair<float, float> applyDistortion(float channelDataL, float channelDataR, float driveLinear, DistortionAudioProcessor::DistortionType currentDistortionType) {
    float wetsignalL, wetsignalR;
    if(currentDistortionType == DistortionAudioProcessor::ArctanDistortion){ //arctan
        wetsignalL = std::atan(driveLinear * channelDataL);
        wetsignalR = std::atan(driveLinear * channelDataR);
    }else if (currentDistortionType == DistortionAudioProcessor::HardClipping){  // hard clipping
        float tempL = driveLinear * channelDataL;
        float tempR = driveLinear * channelDataR;
        if(tempL > 1.0f){
            wetsignalL = 1.0f;
        }else if (tempL <-1.0f){
            wetsignalL = -1.0f;
        }else{
            wetsignalL = tempL;
        }
        if(tempR > 1.0f){
            wetsignalR = 1.0f;
        }else if (tempR <-1.0f){
            wetsignalR = -1.0f;
        }else{
            wetsignalR = tempR;
        }
    }else{
        float tempL = driveLinear * channelDataL;
        float tempR = driveLinear * channelDataR;
        if(tempL > 1.0f){
            wetsignalL = 1.0f;
        }else if (tempL <-1.0f){
            wetsignalL = -1.0f;
        }else{
            wetsignalL = tempL - (1.0f/3.0f) * std::pow(tempL, 3);
        }
        if(tempR > 1.0f){
            wetsignalR = 1.0f;
        }else if (tempR <-1.0f){
            wetsignalR = -1.0f;
        }else{
            wetsignalR = tempR - (1.0f/3.0f) * std::pow(tempR, 3);
        }
    }
    return {wetsignalL, wetsignalR};
}
    
void DistortionAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();


    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());
    //convert drive and output to linear
    calcAlgorithmParams();
    updatefilter();
    float wetmix = 1.0 * mix->get()/100.0;
    float drymix = 1.0 - 1.0 * mix->get()/100.0;
    //int type = distortionType -> get();
    float* channelDataL = buffer.getWritePointer (0);
    float* channelDataR = buffer.getWritePointer (1);
    float drysignalL,drysignalR,wetsignalL,wetsignalR;
    for (int i = 0; i < buffer.getNumSamples(); ++i)
    {
        drysignalL = channelDataL[i];
        drysignalR = channelDataR[i];
        // add tone filters
        drysignalL = highpassL.tick(drysignalL);
        drysignalR = highpassR.tick(drysignalR);
        drysignalL = peakFilterL.tick(drysignalL);
        drysignalR = peakFilterR.tick(drysignalR);
        //low-band
        float lowBandL = crossoverLowHighL.processSample(0, drysignalL);
        float lowBandR = crossoverLowHighR.processSample(1, drysignalR);
        //high-band
        float highBandL = crossoverMidHighL.processSample(0, drysignalL);
        float highBandR = crossoverMidHighR.processSample(1, drysignalR);
        //mid-band
        
        float midBandL = midBandHighPassFilterL.processSample(0, drysignalL);
        midBandL = midBandLowPassFilterL.processSample(0, midBandL);
        float midBandR = midBandHighPassFilterR.processSample(1, drysignalR);
        midBandR = midBandLowPassFilterR.processSample(1, midBandR);
         
        /*
        float midBandL = drysignalL - lowBandL - highBandL;
        float midBandR = drysignalR - lowBandR - highBandR;
        */
        //apply distortion
        auto [LowWetSignalL, LowWetSignalR] = applyDistortion(lowBandL, lowBandR, lowdriveLinear, currentDistortionTypelow);
        auto [MidWetSignalL, MidWetSignalR] = applyDistortion(midBandL, midBandR, middriveLinear, currentDistortionTypemid);
        auto [HighWetSignalL, HighWetSignalR] = applyDistortion(highBandL, highBandR, highdriveLinear, currentDistortionTypehigh);
        // adding all bands
        wetsignalL = LowWetSignalL+MidWetSignalL+HighWetSignalL;
        wetsignalR = LowWetSignalR+MidWetSignalR+HighWetSignalR;
        
        // low pass filter
        wetsignalL = lowpassL.tick(wetsignalL);
        wetsignalR = lowpassR.tick(wetsignalR);
        // * output
        wetsignalL = outputLinear * wetsignalL;
        wetsignalR = outputLinear * wetsignalR;
        
        channelDataL[i] = drymix * drysignalL + wetmix * wetsignalL;
        channelDataR[i] = drymix * drysignalR + wetmix * wetsignalR;
    }
}

//==============================================================================
bool DistortionAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* DistortionAudioProcessor::createEditor()
{
    return new DistortionAudioProcessorEditor (*this);
}

//==============================================================================
void DistortionAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    //chatgpt: when I switched to the soft or hard and close and reopen the vst it will automatically jump back to med, how do I let the vst stay at the algorithm I choose
    // Create an Xml element to save your data
    std::unique_ptr<juce::XmlElement> xml(new juce::XmlElement("MYPLUGINSETTINGS"));
    // Store the current algorithm type
    xml->setAttribute("selectedAlgorithmLow", currentDistortionTypelow);
    xml->setAttribute("selectedAlgorithmMid", currentDistortionTypemid);
    xml->setAttribute("selectedAlgorithmHigh", currentDistortionTypehigh);
    // Convert XML to binary data and store it in destData
    copyXmlToBinary(*xml, destData);
    
}

void DistortionAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // Convert binary data to XML
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));
    if (xmlState != nullptr)
    {
        // Make sure it's the correct XML tag for your settings
        if (xmlState->hasTagName("MYPLUGINSETTINGS"))
        {
            // Restore the algorithm type
            currentDistortionTypelow = static_cast<DistortionType>(xmlState->getIntAttribute("selectedAlgorithmLow", ArctanDistortion));
            currentDistortionTypemid = static_cast<DistortionType>(xmlState->getIntAttribute("selectedAlgorithmMid", ArctanDistortion));
            currentDistortionTypehigh = static_cast<DistortionType>(xmlState->getIntAttribute("selectedAlgorithmHigh", ArctanDistortion));
        }
    }
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new DistortionAudioProcessor();
}
