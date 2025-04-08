//#include "Menu.h"
//
//IOMenu::IOMenu(juce::AudioDeviceManager& deviceManager)
//{
//    // AudioDeviceSelectorComponent
//
//	juce::Component::addAndMakeVisible(burgerMenu);
//
//	updateDeviceMenu();
//	
//}
//
//IOMenu::~IOMenu()
//{
//}
//
//void IOMenu::resized()
//{
//}
//
//void IOMenu::burgerMenuItemSelected(int itemID)
//{
//}
//
//void IOMenu::updateDeviceMenu() 
//{
//
//    juce::PopupMenu menu;
//
//    // Get available audio input devices
//    auto deviceTypes = &audioDeviceManager.getAvailableDeviceTypes();
//    int itemId = 1;
//
//    for (auto* type : *deviceTypes)
//    {
//        auto deviceNames = type->getDeviceNames(true); // true for input devices
//
//        for (const auto& deviceName : deviceNames)
//        {
//            bool isCurrentDevice = (audioDeviceManager.getCurrentAudioDevice() &&
//                deviceName == audioDeviceManager.getCurrentAudioDevice()->getName());
//
//            // Store the mapping of ID to device name
//            idToDeviceMap[itemId] = deviceName;
//
//            menu.addItem(itemId++, deviceName, true, isCurrentDevice);
//        }
//    }
//
//    // Set the menu on the burger component
//    burgerMenu.setMenuForComponent(this, menu);
//}