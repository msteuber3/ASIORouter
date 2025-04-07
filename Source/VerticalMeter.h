/*
  ==============================================================================

    VerticalMeter.h
    Created: 4 Apr 2025 8:38:42pm
    Author:  Michael

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

class VerticalMeter : public juce::Component 
{
public:
    VerticalMeter();
    ~VerticalMeter() override;
    void paint(juce::Graphics& g) override;

    void setLevel(const float value);

private:
    float level = -60.0f;

};
