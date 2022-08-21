#include "bill-acceptor/nv10.h"

namespace {

	enum class State {
		uninitialized,
		initialized,
		failed
	};
	State state = State::uninitialized;

	std::deque<int> buffer;
	float accumulatedValue = 0.00;
	std::vector<float> billValues;
	unsigned short billTxPin;
	unsigned short billRxPin;
	unsigned int billBaudRate;

	const std::map<const char*, const uint8_t> SIO_Codes = {
		{ "note_accepted_c1", 1 },
		{ "note_accepted_c2", 2 },
		{ "note_accepted_c3", 3 },
		{ "note_accepted_c4", 4 },
		{ "note_accepted_c5", 5 },
		{ "note_accepted_c6", 6 },
		{ "note_accepted_c7", 7 },
		{ "note_accepted_c8", 8 },
		{ "note_accepted_c9", 9 },
		{ "note_accepted_c10", 10 },
		{ "note_accepted_c11", 11 },
		{ "note_accepted_c12", 12 },
		{ "note_accepted_c13", 13 },
		{ "note_accepted_c14", 14 },
		{ "note_accepted_c15", 15 },
		{ "note_accepted_c16", 16 },
		{ "note_not_recognized", 20 },
		{ "mechanism_running_slow", 30 },
		{ "strimming_attempted", 40 },
		{ "note_rejected_channel_5_fraud", 50 },
		{ "stacker_full_or_jammed", 60 },
		{ "abort_during_escrow", 70 },
		{ "note_may_have_been_taken_to_clear_jam", 80 },
		{ "validator_busy", 120 },
		{ "validator_not_busy", 121 },
		{ "inhibit_c1", 131 },
		{ "inhibit_c2", 132 },
		{ "inhibit_c3", 133 },
		{ "inhibit_c4", 134 },
		{ "inhibit_c5", 135 },
		{ "inhibit_c6", 136 },
		{ "inhibit_c7", 137 },
		{ "inhibit_c8", 138 },
		{ "inhibit_c9", 139 },
		{ "inhibit_c10", 140 },
		{ "inhibit_c11", 141 },
		{ "inhibit_c12", 142 },
		{ "inhibit_c13", 143 },
		{ "inhibit_c14", 144 },
		{ "inhibit_c15", 145 },
		{ "inhibit_c16", 146 },
		{ "uninhibit_c1", 151 },
		{ "uninhibit_c2", 152 },
		{ "uninhibit_c3", 153 },
		{ "uninhibit_c4", 154 },
		{ "uninhibit_c5", 155 },
		{ "uninhibit_c6", 156 },
		{ "uninhibit_c7", 157 },
		{ "uninhibit_c8", 158 },
		{ "uninhibit_c9", 159 },
		{ "uninhibit_c10", 160 },
		{ "uninhibit_c11", 161 },
		{ "uninhibit_c12", 162 },
		{ "uninhibit_c13", 163 },
		{ "uninhibit_c14", 164 },
		{ "uninhibit_c15", 165 },
		{ "uninhibit_c16", 166 },
		{ "enable_escrow_mode", 170 },
		{ "disable_escrow_mode", 171 },
		{ "accept_escrow", 172 },
		{ "reject_escrow", 173 },
		{ "status", 182 },
		{ "enable_all", 184 },
		{ "disable_all", 185 },
		{ "disable_escrow_timeout", 190 },
		{ "enable_escrow_timeout", 191 },
		{ "request_firmware", 192 },
		{ "request_dataset", 193 },
		{ "command_error", 255 }
	};

	uint8_t getSIOCodeByte(const char* key) {
		if (SIO_Codes.count(key) > 0) {
			return SIO_Codes.at(key);
		}
		return 0;
	}

	void serialWriteSIOCode(const char* key) {
		if (state == State::initialized) {
			const uint8_t byteOut = getSIOCodeByte(key);
			if (byteOut > 0) {
				logger::write("Sending SIO code to bill acceptor: \"" + std::string(key) + "\"");
				Serial1.write(byteOut);
			}
		}
	}

	float getBillValue(const uint8_t &byteIn) {
		const uint8_t index = byteIn - 1;
		if (index >= 0 && index < billValues.size()) {
			return billValues[index];
		}
		return 0;
	}

	std::string getSIOCodeKey(const uint8_t &byteIn) {
		for (auto kv : SIO_Codes) {
			if (kv.second == byteIn) {
				return std::string(kv.first);
			}
		}
		return "";
	}

	void parseBuffer() {
		while (buffer.size() >= 3) {
			const uint8_t byteIn1 = buffer.front();
			buffer.pop_front();
			if (byteIn1 == getSIOCodeByte("validator_busy")) {
				const uint8_t byteIn2 = buffer.front();
				buffer.pop_front();
				if (byteIn2 == getSIOCodeByte("validator_not_busy")) {
					const uint8_t byteIn3 = buffer.front();
					buffer.pop_front();
					if (byteIn3 >= getSIOCodeByte("note_accepted_c1") && byteIn3 <= getSIOCodeByte("note_accepted_c16")) {
						// The third byte is the number of the bill from the dataset.
						float billValue = getBillValue(byteIn3);
						if (billValue > 0) {
							logger::write("Bill inserted with value = " + std::to_string(billValue));
							accumulatedValue += billValue;
						}
					}
				} else if (byteIn2 == getSIOCodeByte("note_not_recognized")) {
					const uint8_t byteIn3 = buffer.front();
					if (byteIn3 == getSIOCodeByte("validator_not_busy")) {
						buffer.pop_front();
					}
				}
			}
		}
	}
}

namespace billAcceptor_nv10 {

	void init() {
		billTxPin = config::getUnsignedShort("billTxPin");
		billRxPin = config::getUnsignedShort("billRxPin");
		billBaudRate = config::getUnsignedInt("billBaudRate");
		billValues = config::getFloatVector("billValues");
	}

	void loop() {
		if (state == State::initialized) {
			while (Serial1.available()) {
				const uint8_t byteIn = Serial1.read();
				if (byteIn > 0) {
					const std::string code = getSIOCodeKey(byteIn);
					if (code != "") {
						logger::write("NV10 Bill acceptor SIO code received: " + code);
						buffer.push_back(byteIn);
					}
				}
			}
			parseBuffer();
		} else if (state == State::uninitialized) {
			if (!(billRxPin > 0)) {
				logger::write("Cannot initialize bill acceptor: \"billRxPin\" not set", "warn");
				state = State::failed;
			} else if (!(billTxPin > 0)) {
				logger::write("Cannot initialize bill acceptor: \"billTxPin\" not set", "warn");
				state = State::failed;
			} else if (!(billBaudRate > 0)) {
				logger::write("Cannot initialize bill acceptor: \"billBaudRate\" not set", "warn");
				state = State::failed;
			} else {
				logger::write("Initializing NV10 bill acceptor...");
				Serial1.begin(billBaudRate, SERIAL_8N1, billTxPin, billRxPin);
				billAcceptor_nv10::disinhibit();
				serialWriteSIOCode("disable_escrow_mode");
				state = State::initialized;
			}
		}
	}

	float getAccumulatedValue() {
		return accumulatedValue;
	}

	void resetAccumulatedValue() {
		accumulatedValue = 0.00;
	}

	void disinhibit() {
		serialWriteSIOCode("enable_all");
	}

	void inhibit() {
		serialWriteSIOCode("disable_all");
	}
}
