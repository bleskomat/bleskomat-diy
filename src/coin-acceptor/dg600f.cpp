#include "coin-acceptor/dg600f.h"

namespace {

	enum class State {
		uninitialized,
		initialized,
		failed
	};
	State state = State::uninitialized;

	float accumulatedValue = 0.00;
	std::deque<int> buffer;
	std::vector<float> coinValues;
	unsigned short coinSignalPin;
	unsigned short coinInhibitPin;
	unsigned int coinBaudRate;
	unsigned short fiatPrecision;

	float getCoinValue(const int &byteIn) {
		const int index = byteIn - 1;
		if (index >= 0 && index < coinValues.size()) {
			return coinValues[index];
		}
		return 0;
	}

	void parseBuffer() {
		while (buffer.size() >= 3) {
			const int byte1 = buffer.front();
			buffer.pop_front();
			if (byte1 == 0xAA) {
				// "Title" byte found.
				// The next byte should be the coin value reference.
				const int byte2 = buffer.front();
				buffer.pop_front();
				const float coinValue = getCoinValue(byte2);
				if (coinValue > 0) {
					// Coin value found.
					// The next byte is the XOR of the previous two bytes.
					const int byte3 = buffer.front();
					buffer.pop_front();
					if (byte3 == (byte1 ^ byte2)) {
						logger::write("Coin inserted with value = " + util::floatToStringWithPrecision(coinValue, fiatPrecision));
						accumulatedValue += coinValue;
					}
				}
			}
		}
	}
}

namespace coinAcceptor_dg600f {

	void init() {
		coinSignalPin = config::getUnsignedShort("coinSignalPin");
		coinInhibitPin = config::getUnsignedShort("coinInhibitPin");
		coinBaudRate = config::getUnsignedInt("coinBaudRate");
		coinValues = config::getFloatVector("coinValues");
		fiatPrecision = config::getUnsignedShort("fiatPrecision");
	}

	void loop() {
		if (state == State::initialized) {
			while (Serial2.available()) {
				const int byteReceived = Serial2.read();
				if (byteReceived > 0 && byteReceived < 254) {
					buffer.push_back(byteReceived);
				}
			}
			parseBuffer();
		} else if (state == State::uninitialized) {
			if (!(coinSignalPin > 0)) {
				logger::write("Cannot initialize coin acceptor: \"coinSignalPin\" not set", "warn");
				state = State::failed;
			} else if (!(coinInhibitPin > 0)) {
				logger::write("Cannot initialize coin acceptor: \"coinInhibitPin\" not set", "warn");
				state = State::failed;
			} else if (!(coinBaudRate > 0)) {
				logger::write("Cannot initialize coin acceptor: \"coinBaudRate\" not set", "warn");
				state = State::failed;
			} else {
				logger::write("Initializing DG600F coin acceptor...");
				Serial2.begin(coinBaudRate, SERIAL_8E1, coinSignalPin, 0);
				pinMode(coinInhibitPin, OUTPUT);
				state = State::initialized;
				coinAcceptor_dg600f::disinhibit();
			}
		}
	}

	float getAccumulatedValue() {
		return accumulatedValue;
	}

	void resetAccumulatedValue() {
		accumulatedValue = 0.00;
	}

	void inhibit() {
		if (state == State::initialized) {
			digitalWrite(coinInhibitPin, LOW);
		}
	}

	void disinhibit() {
		if (state == State::initialized) {
			digitalWrite(coinInhibitPin, HIGH);
		}
	}
}
