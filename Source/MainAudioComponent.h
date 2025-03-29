#pragma once

#include <JuceHeader.h>

class MainAudioComponent : public juce::AudioAppComponent {
public:
	MainAudioComponent();
	~MainAudioComponent();

	void prepareToPlay(int, double) override;
	void releaseResources() override;


	void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) override;
private:


};