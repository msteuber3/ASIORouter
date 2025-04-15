#pragma once
class CustomListener {
public:	
	virtual ~CustomListener() = default;
	virtual void eventTriggered() = 0;
};

class MainComponentListener {
public:
	virtual ~MainComponentListener() = default;
	virtual void mainComponentEventTriggered() = 0;
};

/*
* Okay so this is kinda jank but it works where the juce listener did not AND it doesn't result in circular dependancy issues. There's probably
* an easier solution with forward declarations but this is what I did. Here's how it works:
* 
* MainComponent inherits from MainComponentListener and overrides the mainComponentEventTriggered to reset the MainMixer object.
* When the menubar is created, it is given a pointer to MainComponent as a MainComponentListener (through setListener (see MenuBar class))
* This sets menubar's parentListener member to be MainComponent without having to let the menubar class know what a MainComponent is 
* 
* MenuBar inherits from CustomListener and overrides eventTriggered to call mainComponentEventTriggered (defined in MainComponent class, declared here) 
* from the MainComponentListener pointer
* 
* PropertiesWindow is instantiated with the menubar's CustomListener pointer (so basically a referecne to menubar) set as it's customListener. 
* PropertiesWindow doesn't inherit shit, it just includes this file
* when the CloseButtonPressed override from DocumentWindow is pressed, it calls eventTriggered on menuBar which starts the cascade down into mixer->resetChannelList
* 
* 
* MainComponent:MainComponentListener				MainComponent::mainComponentEventTriggered
*			|												^
*			V												|		
*	MenuBar::CustomListener							MenuBar:eventTriggered()
*			|												^
*			V												|	
*	PropertiesWindow ----------------------------> PropertiesWindow:listener
* 
*
*					Elite max level dev stuff right here (I'm fucking stupid)
*
*/


