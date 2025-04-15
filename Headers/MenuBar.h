#pragma once
#include <JuceHeader.h>
#include "../CustomListener.cpp"
#include <PropertiesWindow.h>

class MenuBar : public juce::MenuBarModel, public PropertiesWindow::PropertiesWindowListener {
public:
	struct MenuListener {
		virtual ~MenuListener() = default;
		virtual void menuBarPropertiesWindowClosed() = 0;
	};

	MenuBar();
	~MenuBar() override;

	void setListener(MenuListener* l);

	juce::StringArray getMenuBarNames() override;

	juce::PopupMenu getMenuForIndex(int topLevelMenuIndex, const juce::String& menuName) override;

	void menuItemSelected(int menuItemID, int topLevelMenuIndex) override;

	void createPropertiesWindow();

	void propMenuCloseTriggred() override;

private:

	MenuListener* parentListener = nullptr;

};
