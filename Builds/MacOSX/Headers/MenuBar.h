#pragma once

#include <JuceHeader.h>
#include "PropertiesWindow.h"

class MenuBar : public juce::MenuBarModel {
public:
	MenuBar();
	~MenuBar() override;

	juce::StringArray getMenuBarNames() override;

	juce::PopupMenu getMenuForIndex(int topLevelMenuIndex, const juce::String& menuName) override;

	void menuItemSelected(int menuItemID, int topLevelMenuIndex) override;


private:

};