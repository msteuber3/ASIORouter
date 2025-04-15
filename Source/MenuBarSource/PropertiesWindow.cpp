#pragma once

#include <PropertiesWindow.h>

PropertiesWindow::PropertiesWindow()
    : DocumentWindow("Properties", juce::Colours::darkgrey, DocumentWindow::allButtons), ChangeBroadcaster()
{
    setUsingNativeTitleBar(true);
    setContentOwned(new PropertiesComponent(), true);
    setResizable(true, false);
    centreWithSize(300, 200);
    setVisible(true);
}

PropertiesWindow::~PropertiesWindow(){}

void PropertiesWindow::setListener(PropertiesWindowListener* newListener)
{
    listener = newListener;
}

void PropertiesWindow::closeButtonPressed()
{
    if (listener != nullptr)
        listener->propMenuCloseTriggred();
    delete this;
}