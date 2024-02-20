/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
DistortionAudioProcessorEditor::DistortionAudioProcessorEditor (DistortionAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    lowdriveslider.setLookAndFeel(&customLookAndFeel);
    middriveslider.setLookAndFeel(&customLookAndFeel);
    highdriveslider.setLookAndFeel(&customLookAndFeel);
    outputslider.setLookAndFeel(&customLookAndFeel);
    mixslider.setLookAndFeel(&customLookAndFeel);
    lowcutslider.setLookAndFeel(&customLookAndFeel);
    highcutslider.setLookAndFeel(&customLookAndFeel);
    peakcenterslider.setLookAndFeel(&customLookAndFeel);
    
    setSize (700, 620);
    //juce::Rectangle<int> titleSection(0, 0, 800, 50);
    //juce::Rectangle<int> distortionSection(0, 50, 600, 300);
    //juce::Rectangle<int> controlSection(600, 50, 200, 300);
    //juce::Rectangle<int> bottomSection(0, 350, 800, 150);
    
    // -----------------------Distortion section -----------------------------
    int sectionStartX = 0;
    int sectionWidth = 500;
    int sectionStartY = 165;
    int sectionHeight = 220;
    int sliderWidth = 60;
    int sliderHeight = sectionHeight;
    int lowSliderX = sectionStartX + sectionWidth / 6 - sliderWidth / 2;
    int midSliderX = sectionStartX + sectionWidth / 2 - sliderWidth / 2;
    int highSliderX = sectionStartX + 5 * sectionWidth / 6 - sliderWidth / 2;

    auto& params = processor.getParameters();
    juce::AudioParameterFloat* audioParam = (juce::AudioParameterFloat*)params.getUnchecked(0);
    lowdriveslider.setBounds(lowSliderX, sectionStartY, sliderWidth, sliderHeight);
    lowdriveslider.setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
    lowdriveslider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 150, 20);
    lowdriveslider.setRange(audioParam->range.start, audioParam->range.end);
    lowdriveslider.setValue(*audioParam);
    lowdriveslider.setNumDecimalPlacesToDisplay(2);
    lowdriveslider.setTextValueSuffix ("db");
    lowdriveslider.setDoubleClickReturnValue(true, 0.0f);
    lowdriveslider.addListener(this);
    addAndMakeVisible(lowdriveslider);
    
    audioParam = (juce::AudioParameterFloat*)params.getUnchecked(1);
    middriveslider.setBounds(midSliderX, sectionStartY, sliderWidth, sliderHeight);
    middriveslider.setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
    middriveslider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 150, 20);
    middriveslider.setRange(audioParam->range.start, audioParam->range.end);
    middriveslider.setValue(*audioParam);
    middriveslider.setNumDecimalPlacesToDisplay(2);
    middriveslider.setTextValueSuffix ("db");
    middriveslider.setDoubleClickReturnValue(true, 0.0f);
    middriveslider.addListener(this);
    addAndMakeVisible(middriveslider);
    
    audioParam = (juce::AudioParameterFloat*)params.getUnchecked(2);
    highdriveslider.setBounds(highSliderX, sectionStartY, sliderWidth, sliderHeight);
    highdriveslider.setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
    highdriveslider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 150, 20);
    highdriveslider.setRange(audioParam->range.start, audioParam->range.end);
    highdriveslider.setValue(*audioParam);
    highdriveslider.setNumDecimalPlacesToDisplay(2);
    highdriveslider.setTextValueSuffix ("db");
    highdriveslider.setDoubleClickReturnValue(true, 0.0f);
    highdriveslider.addListener(this);
    addAndMakeVisible(highdriveslider);
    
    //-------------------------------------control section------------------------------------------
    
    int controlSectionStartX = 505;
    int controlSectionWidth = 190;
    int controlSectionStartY = 128;
    int controlSectionHeight = 290;
    int sliderDiameter = 100;
    
    int outputSliderY = controlSectionStartY + controlSectionHeight / 4 - sliderDiameter / 2 + 30;
    int mixSliderY = controlSectionStartY + 3 * controlSectionHeight / 4 - sliderDiameter / 2 + 30;
    
    audioParam = (juce::AudioParameterFloat*)params.getUnchecked(3);
    outputslider.setBounds(controlSectionStartX + controlSectionWidth / 2 - sliderDiameter / 2, outputSliderY, sliderDiameter, sliderDiameter);
    outputslider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    outputslider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
    outputslider.setRange(audioParam->range.start, audioParam->range.end);
    outputslider.setValue(*audioParam);
    outputslider.setNumDecimalPlacesToDisplay(2);
    outputslider.setTextValueSuffix ("db");
    outputslider.setDoubleClickReturnValue(true, 0.0f);
    outputslider.addListener(this);
    addAndMakeVisible(outputslider);
    
    audioParam = (juce::AudioParameterFloat*)params.getUnchecked(4);
    mixslider.setBounds(controlSectionStartX + controlSectionWidth / 2 - sliderDiameter / 2, mixSliderY, sliderDiameter, sliderDiameter);
    mixslider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    mixslider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
    mixslider.setRange(audioParam->range.start, audioParam->range.end);
    mixslider.setValue(*audioParam);
    mixslider.setNumDecimalPlacesToDisplay(2);
    mixslider.setTextValueSuffix ("%");
    mixslider.setDoubleClickReturnValue(true, 100);
    mixslider.addListener(this);
    addAndMakeVisible(mixslider);
    
    //-----------------------------------------------------------------------------------------------
    //set up combo box of choosing distortion type
    typeBoxlow.setBounds(lowSliderX-10, sectionStartY+230, 80, 20);
    typeBoxlow.addItem("Med", 1);
    typeBoxlow.addItem("Hard", 2);
    typeBoxlow.addItem("Soft", 3);
    int distortionTypelow = static_cast<DistortionAudioProcessor&>(processor).getDistortionTypelow();
    typeBoxlow.setSelectedId(distortionTypelow, juce::dontSendNotification);
    typeBoxlow.setColour(juce::ComboBox::textColourId, juce::Colours::black);
    typeBoxlow.setColour(juce::ComboBox::backgroundColourId, juce::Colours::white);
    typeBoxlow.setColour(juce::ComboBox::arrowColourId, juce::Colours::black); // Set arrow color to black
    //typeBox.setSelectedId(1);
    typeBoxlow.addListener(this);
    addAndMakeVisible(typeBoxlow);
    
    //------------------------------mid band combo box --------------------------------
    typeBoxmid.setBounds(midSliderX-10, sectionStartY+230, 80, 20);
    typeBoxmid.addItem("Med", 1);
    typeBoxmid.addItem("Hard", 2);
    typeBoxmid.addItem("Soft", 3);
    int distortionTypemid = static_cast<DistortionAudioProcessor&>(processor).getDistortionTypemid();
    typeBoxmid.setSelectedId(distortionTypemid, juce::dontSendNotification);
    typeBoxmid.setColour(juce::ComboBox::textColourId, juce::Colours::black);
    typeBoxmid.setColour(juce::ComboBox::backgroundColourId, juce::Colours::white);
    typeBoxmid.setColour(juce::ComboBox::arrowColourId, juce::Colours::black); // Set arrow color to black
    typeBoxmid.addListener(this);
    addAndMakeVisible(typeBoxmid);
    
    //------------------------------high band combo box --------------------------------
    typeBoxhigh.setBounds(highSliderX-10, sectionStartY+230, 80, 20);
    typeBoxhigh.addItem("Med", 1);
    typeBoxhigh.addItem("Hard", 2);
    typeBoxhigh.addItem("Soft", 3);
    int distortionTypehigh = static_cast<DistortionAudioProcessor&>(processor).getDistortionTypehigh();
    typeBoxhigh.setSelectedId(distortionTypehigh, juce::dontSendNotification);
    typeBoxhigh.setColour(juce::ComboBox::textColourId, juce::Colours::black);
    typeBoxhigh.setColour(juce::ComboBox::backgroundColourId, juce::Colours::white);
    typeBoxhigh.setColour(juce::ComboBox::arrowColourId, juce::Colours::black); // Set arrow color to black
    typeBoxhigh.addListener(this);
    addAndMakeVisible(typeBoxhigh);
    
    // ------------------------------------tone section-------------------------------------------
    //int bottomSectionStartX = 0;
    //int bottomSectionWidth = 800;
    int bottomSectionStartY = 505;
    //int bottomSectionHeight = 150;
    int FiltersliderDiameter = 80;
    int lowCutSliderX = 55;
    int peakCenterSliderX = 235;
    int highCutSliderX = 415;
    
    
    audioParam = (juce::AudioParameterFloat*)params.getUnchecked(8);
    lowcutslider.setBounds(lowCutSliderX, bottomSectionStartY, FiltersliderDiameter, FiltersliderDiameter);
    lowcutslider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    lowcutslider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
    lowcutslider.setRange(audioParam->range.start, audioParam->range.end);
    lowcutslider.setValue(*audioParam);
    lowcutslider.setNumDecimalPlacesToDisplay(2);
    lowcutslider.setTextValueSuffix ("hz");
    lowcutslider.setDoubleClickReturnValue(true, 20);
    lowcutslider.addListener(this);
    addAndMakeVisible(lowcutslider);
    
    
    audioParam = (juce::AudioParameterFloat*)params.getUnchecked(9);
    highcutslider.setBounds(highCutSliderX, bottomSectionStartY, FiltersliderDiameter, FiltersliderDiameter);
    highcutslider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    highcutslider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
    highcutslider.setRange(audioParam->range.start, audioParam->range.end);
    highcutslider.setValue(*audioParam);
    highcutslider.setNumDecimalPlacesToDisplay(2);
    highcutslider.setTextValueSuffix ("hz");
    highcutslider.setDoubleClickReturnValue(true, 20000);
    highcutslider.addListener(this);
    addAndMakeVisible(highcutslider);
    
    audioParam = (juce::AudioParameterFloat*)params.getUnchecked(10);
    peakcenterslider.setBounds(peakCenterSliderX, bottomSectionStartY, FiltersliderDiameter, FiltersliderDiameter);
    peakcenterslider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    peakcenterslider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
    peakcenterslider.setRange(audioParam->range.start, audioParam->range.end);
    peakcenterslider.setValue(*audioParam);
    peakcenterslider.setNumDecimalPlacesToDisplay(2);
    peakcenterslider.setTextValueSuffix ("hz");
    peakcenterslider.setDoubleClickReturnValue(true, 2000);
    peakcenterslider.addListener(this);
    addAndMakeVisible(peakcenterslider);
    
    
    //----------------------------------labels------------------------------------
    juce::Font toneFont(20.0f);

    lowdrivelabel.setText("Low Drive:", juce::dontSendNotification);
    lowdrivelabel.setFont(toneFont);
    lowdrivelabel.setBounds(lowdriveslider.getX()-30, lowdriveslider.getY() - 25, 100, 20);
    lowdrivelabel.setColour(juce::Label::textColourId, juce::Colours::black);
    addAndMakeVisible(lowdrivelabel);
    
    middrivelabel.setText("Mid Drive:", juce::dontSendNotification);
    middrivelabel.setFont(toneFont);
    middrivelabel.setBounds(middriveslider.getX()-30, middriveslider.getY() - 25, 100, 20);
    middrivelabel.setColour(juce::Label::textColourId, juce::Colours::black);
    addAndMakeVisible(middrivelabel);

    highdrivelabel.setText("High Drive:", juce::dontSendNotification);
    highdrivelabel.setFont(toneFont);
    highdrivelabel.setBounds(highdriveslider.getX()-30, highdriveslider.getY() - 25, 100, 20);
    highdrivelabel.setColour(juce::Label::textColourId, juce::Colours::black);
    addAndMakeVisible(highdrivelabel);

    outputlabel.setText("Output:", juce::dontSendNotification);
    outputlabel.setFont(toneFont);
    outputlabel.setBounds(outputslider.getX()-20, outputslider.getY() - 35, outputslider.getWidth(), 20);
    outputlabel.setColour(juce::Label::textColourId, juce::Colours::black);
    addAndMakeVisible(outputlabel);
    
    mixlabel.setText("Mix:", juce::dontSendNotification);
    mixlabel.setFont(toneFont);
    mixlabel.setBounds(mixslider.getX()-20, mixslider.getY() - 25, mixslider.getWidth(), 20);
    mixlabel.setColour(juce::Label::textColourId, juce::Colours::black);
    addAndMakeVisible(mixlabel);
    
    lowcutlabel.setText("Lowcut:", juce::dontSendNotification);
    lowcutlabel.setFont(toneFont);
    lowcutlabel.setBounds(lowcutslider.getX()-40, lowcutslider.getY() - 33, 80, 25);
    lowcutlabel.setColour(juce::Label::textColourId, juce::Colours::black);
    addAndMakeVisible(lowcutlabel);
    
    peaklabel.setText("Tone:", juce::dontSendNotification);
    peaklabel.setFont(toneFont);
    peaklabel.setBounds(peakcenterslider.getX()-40, peakcenterslider.getY() - 33, 80, 25);
    peaklabel.setColour(juce::Label::textColourId, juce::Colours::black);
    addAndMakeVisible(peaklabel);
    
    highcutlabel.setText("Highcut:", juce::dontSendNotification);
    highcutlabel.setFont(toneFont);
    highcutlabel.setBounds(highcutslider.getX()-40, highcutslider.getY() - 33, 80, 25);
    highcutlabel.setColour(juce::Label::textColourId, juce::Colours::black);
    addAndMakeVisible(highcutlabel);
    
    randomButton.setColour(juce::TextButton::buttonColourId, juce::Colours::white);       // Background colour when off
    randomButton.setColour(juce::TextButton::buttonOnColourId, juce::Colours::darkgrey);    // Background colour when on
    randomButton.setColour(juce::TextButton::textColourOffId, juce::Colours::black);     // Text colour when off
    randomButton.setColour(juce::TextButton::textColourOnId, juce::Colours::white);      // Text colour when on
    randomButton.setBounds(565, bottomSectionStartY+10, 80, 40);
    randomButton.addListener(this);
    addAndMakeVisible(randomButton);
}

void DistortionAudioProcessorEditor::sliderValueChanged(juce::Slider * slider){
    auto& params = processor.getParameters();
    if (slider == &lowdriveslider)
    {
        juce::AudioParameterFloat* audioParam = (juce::AudioParameterFloat*)params.getUnchecked(0);
        *audioParam = lowdriveslider.getValue();
        DBG("low drive slider changed");
    }
    else if (slider == &middriveslider)
    {
        juce::AudioParameterFloat* audioParam = (juce::AudioParameterFloat*)params.getUnchecked(1);
        *audioParam = middriveslider.getValue();
        DBG("mid drive slider changed");
    }
    else if (slider == &highdriveslider)
    {
        juce::AudioParameterFloat* audioParam = (juce::AudioParameterFloat*)params.getUnchecked(2);
        *audioParam = highdriveslider.getValue();
        DBG("high drive slider changed");
    }
    else if (slider == &outputslider)
    {
        juce::AudioParameterFloat* audioParam = (juce::AudioParameterFloat*)params.getUnchecked(3);
        *audioParam = outputslider.getValue();
        DBG("output slider changed");
    }
    else if (slider == &mixslider)
    {
        juce::AudioParameterFloat* audioParam = (juce::AudioParameterFloat*)params.getUnchecked(4);
        *audioParam = mixslider.getValue();
        DBG("mix slider changed");
    }
    else if (slider == &lowcutslider)
    {
        juce::AudioParameterFloat* audioParam = (juce::AudioParameterFloat*)params.getUnchecked(8);
        *audioParam = lowcutslider.getValue();
        DBG("low cut slider changed");
    }
    else if (slider == &highcutslider)
    {
        juce::AudioParameterFloat* audioParam = (juce::AudioParameterFloat*)params.getUnchecked(9);
        *audioParam = highcutslider.getValue();
        DBG("high cut slider changed");
    }
    else if (slider == &peakcenterslider)
    {
        juce::AudioParameterFloat* audioParam = (juce::AudioParameterFloat*)params.getUnchecked(10);
        *audioParam = peakcenterslider.getValue();
        DBG("peak slider changed");
    }
}

void DistortionAudioProcessorEditor::comboBoxChanged(juce::ComboBox* comboBox)
{
    //low band
    if (comboBox == &typeBoxlow){
        int selectedId = typeBoxlow.getSelectedId();
        if (selectedId == 1){
            audioProcessor.setDistortionTypelow(1);     // arctan
        }else if (selectedId == 2){
            audioProcessor.setDistortionTypelow(2);     // hardclipping
        }else{
            audioProcessor.setDistortionTypelow(3);     //soft clipping
        }
    }
    // mid band
    else if (comboBox == &typeBoxmid){
        int selectedId = typeBoxmid.getSelectedId();
        if (selectedId == 1){
            audioProcessor.setDistortionTypemid(1);
        }else if (selectedId == 2){
            audioProcessor.setDistortionTypemid(2);
        }else{
            audioProcessor.setDistortionTypemid(3);
        }
    }
    //high band
    else if (comboBox == &typeBoxhigh){
        int selectedId = typeBoxhigh.getSelectedId();
        if (selectedId == 1){
            audioProcessor.setDistortionTypehigh(1);
        }else if (selectedId == 2){
            audioProcessor.setDistortionTypehigh(2);
        }else{
            audioProcessor.setDistortionTypehigh(3);
        }
    }
    
}
DistortionAudioProcessorEditor::~DistortionAudioProcessorEditor()
{
    lowdriveslider.setLookAndFeel(nullptr);
    middriveslider.setLookAndFeel(nullptr);
    highdriveslider.setLookAndFeel(nullptr);
    outputslider.setLookAndFeel(nullptr);
    mixslider.setLookAndFeel(nullptr);
    lowcutslider.setLookAndFeel(nullptr);
    highcutslider.setLookAndFeel(nullptr);
    peakcenterslider.setLookAndFeel(nullptr);
}

//==============================================================================
void DistortionAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    //g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
    g.fillAll(juce::Colours::white);
    //g.setColour (juce::Colours::white);
    juce::Rectangle<int> titleSection(0, 0, 700, 120);
    g.setColour(juce::Colours::black);
    g.setFont(juce::Font(90.0f)); // You can adjust the font size as needed
    juce::String titleText = "IIIONSTER";
    g.drawText(titleText, titleSection, juce::Justification::centred, true);
    
    g.setColour(juce::Colours::darkgrey);
    //juce::Rectangle<int> titleSection(0, 0, 800, 120);
    juce::Rectangle<int> distortionSection(5, 125, 490, 320);
    juce::Rectangle<int> controlSection(505, 125, 190, 320);
    juce::Rectangle<int> bottomSection(5, 465, 690, 145);

    // Draw the outlines for each section
    //g.drawRect(titleSection);
    g.drawRect(distortionSection,4);
    g.drawRect(controlSection,4);
    g.drawRect(bottomSection,4);
    

    //g.drawFittedText ("Hello World!", getLocalBounds(), juce::Justification::centred, 1);
}

void DistortionAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
}


float DistortionAudioProcessorEditor::randomGenerator(float min, float max) {
    float value = juce::Random::getSystemRandom().nextFloat() * (max - min) + min;
    return std::round(value * 100.0f) / 100.0f;
}


void DistortionAudioProcessorEditor::buttonClicked(juce::Button* button)
{
    if (button == &randomButton)
    {
        // Set random values to sliders
        lowdriveslider.setValue(randomGenerator(-36,36));
        middriveslider.setValue(randomGenerator(-36,36));
        highdriveslider.setValue(randomGenerator(-36,36));
        
        outputslider.setValue(randomGenerator(-24,6));
        mixslider.setValue(randomGenerator(0,100));
        
        lowcutslider.setValue(randomGenerator(20,1000));
        peakcenterslider.setValue(randomGenerator(100,10000));
        highcutslider.setValue(randomGenerator(1000,20000));
    }
}
