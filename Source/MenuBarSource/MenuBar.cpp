#pragma once
#include <MenuBar.h>

MenuBar::MenuBar() : juce::MenuBarModel() {}
MenuBar::~MenuBar() {}


juce::StringArray MenuBar::getMenuBarNames()
{
    return { "File", "Edit", "Options"};
}

juce::PopupMenu MenuBar::getMenuForIndex(int topLevelMenuIndex, const juce::String& menuName)
{
    juce::PopupMenu menu;
    juce::PopupMenu removeDevicesMenu;

    switch (topLevelMenuIndex)
    {
    case 0: // File
        menu.addItem("Open", [] { /* handle open */ });
        menu.addItem("Save", [] { /* handle save */ });
        menu.addSeparator();
        menu.addItem("Quit", [] { juce::JUCEApplication::getInstance()->systemRequestedQuit(); });
        break;

    case 1: // Edit
        menu.addSubMenu("Remove Devices", removeDevicesMenu, false); // TODO
        break;
    case 2: //Options
        menu.addItem("Properties", [this] { createPropertiesWindow(); });
        break;
    
    }
    return menu;
}

// Handle the menu item selection (if not using lambdas directly in the menu)
void MenuBar::menuItemSelected(int menuItemID, int topLevelMenuIndex)
{}

void MenuBar::createPropertiesWindow()
{
    auto* propertiesWindow = new PropertiesWindow();
    
}

void MenuBar::removeDevices() {}

