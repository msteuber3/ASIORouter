#pragma once
#include <JuceHeader.h>
#include <PropertiesComponent.h>
#include <CustomListener.h>

class PropertiesWindow : public juce::DocumentWindow
{
public:
    PropertiesWindow();
    ~PropertiesWindow() override;

	void setListener(CustomListener* newListener);

    void closeButtonPressed() override;

private:

	CustomListener* listener = nullptr;

};
