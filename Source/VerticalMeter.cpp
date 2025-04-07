/*
  ==============================================================================

    VerticalMeter.cpp
    Created: 4 Apr 2025 8:39:32pm
    Author:  Michael

  ==============================================================================
*/

#include "VerticalMeter.h"

VerticalMeter::VerticalMeter() : Component() {}

VerticalMeter::~VerticalMeter() { }

void VerticalMeter::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat();

    g.setColour(juce::Colours::white.withBrightness(0.4f));
    g.fillRoundedRectangle(bounds, 5.f);

    g.setColour(juce::Colours::white);

    const auto scaledX = juce::jmap(level, -60.0f, +6.f, 0.f, static_cast<float>(getWidth()));

    g.fillRoundedRectangle(bounds.removeFromLeft(scaledX), 5.f);

}

void VerticalMeter::setLevel(const float value)
{
    level = value;
}
