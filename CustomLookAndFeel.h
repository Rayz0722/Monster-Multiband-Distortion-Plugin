/*
  ==============================================================================

    CustomLookAndFeel.h
    Created: 11 Dec 2023 1:36:41pm
    Author:  周睿洋

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

class CustomLookAndFeel : public juce::LookAndFeel_V4 {
public:
    void drawRotarySlider(juce::Graphics &g, int x, int y, int width, int height, float slidersPos,
                          float rotaryStartAngle, float rotaryEndAngle, juce::Slider &slider)override
    {
        float diameter =juce::jmin(width,height);
        float radius = diameter/2;
        float centerx = x+width/2;
        float centery = y + height/2;
        float rx = centerx -radius;
        float ry = centery - radius;
        float angle = rotaryStartAngle + (slidersPos * (rotaryEndAngle - rotaryStartAngle));
        juce::Rectangle<float> knobArea (rx,ry,diameter,diameter);
        g.setColour(juce::Colours::black);
        g.fillEllipse(knobArea);
        g.setColour(juce::Colours::white);
        
        juce::Path knobTick;
        knobTick.addRectangle(0, -radius, 4.0f, radius *0.33);
        g.fillPath(knobTick,juce::AffineTransform::rotation(angle).translated(centerx, centery));
        g.setColour(juce::Colours::white);
        g.drawEllipse(rx, ry, diameter, diameter, 2.0f);
    }
    
    
    void drawLinearSlider(juce::Graphics& g, int x, int y, int width, int height, float sliderPos,
                              float minSliderPos, float maxSliderPos, const juce::Slider::SliderStyle style,
                              juce::Slider& slider) override {
            if (style == juce::Slider::LinearVertical) {
                // Your custom drawing code for linear vertical sliders
                int trackwidth = 40;
                int trackx = x+(width-trackwidth)/2;
                g.setColour(juce::Colours::black);
                g.fillRect(trackx, y, trackwidth, height);

                // Example: Draw the thumb
                g.setColour(juce::Colours::white);
                int thumbHeight = 15;
                int thumbY = sliderPos - thumbHeight / 2;
                juce::Rectangle<int> thumbRect(x, thumbY, width, thumbHeight);
                g.fillRect(thumbRect);
                g.setColour(juce::Colours::darkgrey);
                g.drawRect(thumbRect, 2);
            }
    }
    
    void drawLabel(juce::Graphics& g, juce::Label& label) override {
            label.setColour(juce::Label::backgroundColourId, juce::Colours::white);
            label.setColour(juce::Label::textColourId, juce::Colours::black);
            label.setColour(juce::Label::outlineColourId, juce::Colours::white);
            
            LookAndFeel_V4::drawLabel(g, label);
    }

};
