#ifndef BUTTONHANDLER_H
#define BUTTONHANDLER_H

#include <Arduino.h>
#include <functional>
#include "config.h"

class ButtonHandler 
{
	public:
		ButtonHandler();
		
		const ButtonConfig* getPressedButtonConfig();
		void provideFeedBack(const ButtonConfig* config, std::function<void()> callback);
		void enableDeepSleepWakeup();
		
	private:
		const ButtonConfig* getButtonConfig(int pin);
};

#endif