#pragma once
#include <JuceHeader.h>
#include <PropertiesComponent.h>

class PropertiesWindow : public juce::DocumentWindow, public juce::ChangeBroadcaster
{
public:
	struct PropertiesWindowListener
	{
		virtual ~PropertiesWindowListener() = default;
		virtual void propMenuCloseTriggred() = 0;
	};

    PropertiesWindow();
    ~PropertiesWindow() override;

	void setListener(PropertiesWindowListener* newListener);

    void closeButtonPressed() override;

private:

	PropertiesWindowListener* listener = nullptr;

};
