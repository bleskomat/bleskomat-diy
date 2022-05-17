/*
	Copyright (C) 2020 Samotari (Charles Hill, Carlos Garcia Ortiz)

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include "modules/button.h"

namespace {
	bool pressed = false;
	int lastState;
	unsigned long lastStateChangeTime = 0;// Last time the button pin was toggled.
	unsigned long debounceDelay = 50;// Debounce time; increase it if the output flickers
}

namespace button {

	void init() {
		logger::write("Initializing button...");
		pinMode(BUTTON_PIN, INPUT);
	}

	void loop() {
		const int state = digitalRead(BUTTON_PIN);
		if ((millis() - lastStateChangeTime) > debounceDelay) {
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
