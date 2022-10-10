#include "coin-acceptor/hx616.h"

namespace {

	enum class State {
		uninitialized,
		initialized,
		failed
	};
	State state = State::uninitialized;

	float valueIncrement = 1.00;
	uint16_t numPulses = 0;
	unsigned short coinSignalPin;

	uint8_t minPulseTime = 20;// milliseconds
	uint8_t maxPulseTime = 200;// milliseconds
	unsigned long pinFallTime;

	void IRAM_ATTR onPinStateChange() {
		const auto pinState = digitalRead(coinSignalPin);
		if (pinState == LOW) {
			pinFallTime = millis();
		} else if (pinFallTime > 0) {
			const unsigned long diffTime = millis() - pinFallTime;
			if (diffTime >= minPulseTime && diffTime <= maxPulseTime) {
				numPulses++;
			}
		}
	}
}

namespace coinAcceptor_hx616 {

	void init() {
		coinSignalPin = config::getUnsignedShort("coinSignalPin");
		valueIncrement = config::getFloat("coinValueIncrement");
	}

	void loop() {
		if (state == State::uninitialized) {
			if (!(coinSignalPin > 0)) {
				logger::write("Cannot initialize coin acceptor: \"coinSignalPin\" not set", "warn");
				state = State::failed;
			} else {
				logger::write("Initializing HX616 coin acceptor...");
				pinMode(coinSignalPin, INPUT_PULLUP);
				attachInterrupt(coinSignalPin, onPinStateChange, CHANGE);
				state = State::initialized;
			}
		}
	}

	float getAccumulatedValue() {
		return numPulses * valueIncrement;
	}

	void resetAccumulatedValue() {
		numPulses = 0;
	}
}
