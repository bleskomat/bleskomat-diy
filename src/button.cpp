#include "button.h"

namespace {
	bool initialized = false;
	bool pressed = false;
	int lastState;
	unsigned long lastStateChangeTime = 0;// Last time the button pin was toggled.
	unsigned int debounceDelay;// Debounce time; increase it if the output flickers
	unsigned short pinNumber;
}

namespace button {

	void init() {
		pinNumber = config::getUnsignedShort("buttonPin");
		debounceDelay = config::getUnsignedInt("buttonDebounce");
		if (!(pinNumber > 0)) {
			logger::write("Cannot initialize button: \"buttonPin\" not set");
		} else {
			logger::write("Initializing button...");
			pinMode(pinNumber, INPUT);
			initialized = true;
		}
	}

	void loop() {
		if (initialized && (millis() - lastStateChangeTime) > debounceDelay) {
			const int state = digitalRead(pinNumber);
			if (state != lastState) {
				if (state == HIGH) {
					pressed = true;
					logger::write("Button pressed");
				} else {
					pressed = false;
					logger::write("Button released");
				}
				// Reset the debouncing timer.
				// We track time in order to avoid noise state changes.
				lastStateChangeTime = millis();
				lastState = state;
			}
		}
	}

	bool isPressed() {
		return pressed;
	}
}
