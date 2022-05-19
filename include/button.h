#ifndef BLESKOMAT_BUTTON_H
#define BLESKOMAT_BUTTON_H

#include "config.h"
#include "logger.h"

#include <Arduino.h>

namespace button {
	void init();
	void loop();
	bool isPressed();
}

#endif
