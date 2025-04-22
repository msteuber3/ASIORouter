/*
  ==============================================================================

    VerticalMeter.cpp
    Created: 4 Apr 2025 8:39:32pm
    Author:  Michael

  ==============================================================================
*/

#include <VerticalMeter.h>

VerticalMeter::VerticalMeter() : Component() {}

VerticalMeter::~VerticalMeter() { }

void VerticalMeter::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat();

    g.setColour(juce::Colours::white.withBrightness(0.2f));
    g.fillRoundedRectangle(bounds, 5.f);

    juce::Colour meterColor;

    if (level > -60) {
        if (level <= 0 && level <= 3) {
            juce::uint8 red = juce::jmap((juce::uint8)std::ceil(level * 10), (juce::uint8)0, (juce::uint8)30, (juce::uint8)0, (juce::uint8)255);
            meterColor = juce::Colour(red, (juce::uint8)255, (juce::uint8)0);
        }
        else if (level > 3) {
            int greenOffset = juce::jmap((int)std::ceil(level * 10), 30, 60, 0, 255);
            meterColor = juce::Colour((juce::uint8)255, (juce::uint8)std::abs(255 - greenOffset), (juce::uint8)0);
        }
        else {
            meterColor = juce::Colours::green;
        }
        g.setColour(meterColor);
        const auto scaledX = juce::jmap(level, -60.0f, +6.f, 0.f, static_cast<float>(getHeight()));
        g.fillRoundedRectangle(bounds.removeFromBottom(scaledX), 5.f);
    }
}

void VerticalMeter::setLevel(const float value)
{
    level = value;
    repaint();
}
