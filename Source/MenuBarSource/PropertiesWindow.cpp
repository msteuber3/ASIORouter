#pragma once

#include <PropertiesWindow.h>

PropertiesWindow::PropertiesWindow()
    : DocumentWindow("Properties", juce::Colours::lightgrey, DocumentWindow::allButtons)
{
    setUsingNativeTitleBar(true);
    setContentOwned(new PropertiesComponent(), true);
    setResizable(true, false);
    centreWithSize(300, 200);
    setVisible(true);
}

void PropertiesWindow::closeButtonPressed()
{
    delete this; // self-destruct when closed
}