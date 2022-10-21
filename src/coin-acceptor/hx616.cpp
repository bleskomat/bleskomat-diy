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
	unsigned short coinInhibitPin;
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
		coinInhibitPin = config::getUnsignedShort("coinInhibitPin");
		valueIncrement = config::getFloat("coinValueIncrement");
	}

	void loop() {
		if (state == State::uninitialized) {
			if (!(coinSignalPin > 0)) {
				logger::write("Cannot initialize coin acceptor: \"coinSignalPin\" not set", "warn");
				state = State::failed;
			} else if (coinInhibitPin > 0) {
				// Only initialize if coinInhibitPin is set.
				// Otherwise, don't fail, because it's optional.
				pinMode(coinInhibitPin, OUTPUT);
			} else {
				logger::write("Initializing HX616 coin acceptor...");
				pinMode(coinSignalPin, INPUT_PULLUP);
				attachInterrupt(coinSignalPin, onPinStateChange, CHANGE);
				state = State::initialized;
				coinAcceptor_hx616::disinhibit();
			}
		}
	}

	float getAccumulatedValue() {
		return numPulses * valueIncrement;
	}

	void resetAccumulatedValue() {
		numPulses = 0;
	}

	void inhibit() {
                if (state == State::initialized && coinInhibitPin > 0) {
			digitalWrite(coinInhibitPin, LOW);
		} else {
			logger::write("Not inhibiting coin acceptor because state is not initialized or coinInhibitPin is not set.", "warn");
                }
        }

        void disinhibit() {
                if (state == State::initialized && coinInhibitPin > 0) {
			digitalWrite(coinInhibitPin, HIGH);
		} else {
			logger::write("Not disinhibiting coin acceptor because state is not initialized or coinInhibitPin is not set.", "warn");
		}
        }

}
