#include "coin-acceptor/hx616.h"

namespace {

	bool initialized = false;
	float valueIncrement = 1.00;
	float accumulatedValue = 0.00;
	uint8_t lastPinReadState;
	unsigned long lastPinReadTime = 0;
	unsigned short coinSignalPin;

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
		return digitalRead(coinSignalPin);
	}

	bool pinStateHasChanged() {
		return readPin() != lastPinReadState;
	}
}

namespace coinAcceptor_hx616 {

	void init() {
		coinSignalPin = config::getUnsignedShort("coinSignalPin");
		valueIncrement = config::getFloat("coinValueIncrement");
	}

	void loop() {
		if (initialized) {
			if (pinStateHasChanged()) {
				if (coinWasInserted()) {
					// This code executes once for each pulse from the HX-616.
					logger::write("Coin inserted");
					accumulatedValue += valueIncrement;
				}
				flipPinState();
			}
		} else if (!(coinSignalPin > 0)) {
			logger::write("Cannot initialize coin acceptor: \"coinSignalPin\" not set");
		} else {
			logger::write("Initializing HX616 coin acceptor...");
			initialized = true;
			pinMode(coinSignalPin, INPUT_PULLUP);
			lastPinReadState = readPin();
		}
	}

	float getAccumulatedValue() {
		return accumulatedValue;
	}

	void resetAccumulatedValue() {
		accumulatedValue = 0.00;
	}
}
