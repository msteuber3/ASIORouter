#pragma once
#include <JuceHeader.h>
#include <CustomListener.h>
#include <PropertiesWindow.h>

class MenuBar : public juce::MenuBarModel, public CustomListener {
public:

	MenuBar();
	~MenuBar() override;

	void setListener(MainComponentListener* l);

	juce::StringArray getMenuBarNames() override;

	juce::PopupMenu getMenuForIndex(int topLevelMenuIndex, const juce::String& menuName) override;

	void menuItemSelected(int menuItemID, int topLevelMenuIndex) override;

	void createPropertiesWindow();

	void eventTriggered() override;

private:

	MainComponentListener* parentListener = nullptr;

};
