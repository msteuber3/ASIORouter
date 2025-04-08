#pragma once
#include <MenuBar.h>

MenuBar::MenuBar() : juce::MenuBarModel()
{}
MenuBar::~MenuBar() {}

juce::StringArray MenuBar::getMenuBarNames()
{
    return { "File", "Edit", "Options"};
}

juce::PopupMenu MenuBar::getMenuForIndex(int topLevelMenuIndex, const juce::String& menuName)
{
    juce::PopupMenu menu;

    switch (topLevelMenuIndex)
    {
    case 0: // File
        menu.addItem("Open", [] { /* handle open */ });
        menu.addItem("Save", [] { /* handle save */ });
        menu.addSeparator();
        menu.addItem("Quit", [] { juce::JUCEApplication::getInstance()->systemRequestedQuit(); });
        break;

    case 1: // Edit
        menu.addItem("Undo", [] { /* handle undo */ });
        menu.addItem("Redo", [] { /* handle redo */ });
        break;
    case 2:
        menu.addItem("Properties", [this] {auto* propertiesWindow = new PropertiesWindow(); });
        break;
    
    }


    return menu;
}

// Handle the menu item selection (if not using lambdas directly in the menu)
void MenuBar::menuItemSelected(int menuItemID, int topLevelMenuIndex)
{
    // Optional: if not using lambdas
}
