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

#include "modules/hx616.h"

namespace {

	bool initialized = false;
	bool inhibited = false;
	float valueIncrement = 1.00;
	float accumulatedValue = 0.00;
	uint8_t lastPinReadState;
	unsigned long lastPinReadTime = 0;

	bool coinWasInserted() {
		unsigned long diffTime = millis() - lastPinReadTime;
		return lastPinReadState == LOW && diffTime > 25 && diffTime < 35;
	}

	void flipPinState() {
		// Flip the state of the last read.
		lastPinReadState = lastPinReadState == HIGH ? LOW : HIGH;
		lastPinReadTime = millis();
	}

	uint8_t readPin() {
		return digitalRead(COIN_ACCEPTOR_SIGNAL);
	}

	bool pinStateHasChanged() {
		return readPin() != lastPinReadState;
	}
}

namespace hx616 {

	void init() {}

	void loop() {
		if (!initialized) {
			initialized = true;
			logger::write("Initializing HX616 coin acceptor...");
			pinMode(COIN_ACCEPTOR_SIGNAL, INPUT_PULLUP);
			lastPinReadState = readPin();
			valueIncrement = config::getCoinValueIncrement();
		}
		if (pinStateHasChanged()) {
			if (coinWasInserted()) {
				// This code executes once for each pulse from the HX-616.
				logger::write("Coin inserted");
				accumulatedValue += valueIncrement;
			}
			flipPinState();
		}
	}

	float getAccumulatedValue() {
		return accumulatedValue;
	}

	void reset() {
		accumulatedValue = 0.00;
	}

	bool isOff() {
		return inhibited;
	}

	bool isOn() {
		return !inhibited;
	}

	void on() {
		// Not implemented for the HX-616.
	}

	void off() {
		// Not implemented for the HX-616.
	}
}
