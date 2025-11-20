#ifndef BUTTONHANDLER_H
#define BUTTONHANDLER_H

#include <Arduino.h>
#include "config.h"

class ButtonHandler 
{
	public:
		ButtonHandler::ButtonHandler();
		
		const ButtonConfig* getPressedButtonConfig();
		void provideFeedBack(const ButtonConfig* config);
		void enableDeepSleepWakeup();
};

#endif