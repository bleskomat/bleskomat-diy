#include "json-rpc.h"

namespace {

	uint32_t initTime;
	const uint32_t maxInitialMessageWaitTime = 3000;// milliseconds
	bool sentWaitingMessage = false;
	bool receivedMessage = false;
	bool timedOut = false;

	// https://arduinojson.org/
	// https://www.jsonrpc.org/specification
	const char* jsonRpcVersion = "2.0";

	std::string escapeLine(const std::string &line) {
		const unsigned int numBytes = line.size();
		const unsigned int remainderNumBytes = numBytes % 256;
		const unsigned int allocateNumBytes = remainderNumBytes > 0 ? (numBytes + 256) - remainderNumBytes : numBytes;
		DynamicJsonDocument docLine(allocateNumBytes + 256);
		docLine.set(line);
		String output;// Buffer the serialized JSON string.
		serializeJson(docLine, output);
		std::string escaped = std::string(output.c_str()).substr(1);// Remove first double-quote character.
		escaped.pop_back();// Remove last double-quote character.
		escaped += "\\n";// Escaped line-break character.
		return escaped;
	}

	void onMessage(const std::string &message) {
		try {
			if (message != "" && message.substr(0, 1) == "{") {
				// !! Important !!
				// Keep the JsonDocument instance until done reading from the deserialized document; more info:
				// https://arduinojson.org/v6/issues/garbage-out/
				DynamicJsonDocument docIn(8192);
				const DeserializationError err = deserializeJson(docIn, message);
				if (err) {
					throw std::runtime_error("deserializeJson failed: " + std::string(err.c_str()));
				}
				const JsonObject json = docIn.as<JsonObject>();
				const std::string jsonrpc = json["jsonrpc"].as<const char*>();
				if (jsonrpc != "2.0") {
					throw std::runtime_error("Unknown JSON-RPC version: \"" + jsonrpc + "\"");
				}
				if (!receivedMessage) {
					logger::write("JSON-RPC serial interface active");
					receivedMessage = true;
				}
				const std::string id = json["id"].as<const char*>();
				const std::string method = json["method"].as<const char*>();
				if (method == "") {
					DynamicJsonDocument docOut(512);
					docOut["jsonrpc"] = jsonRpcVersion;
					docOut["id"] = id;
					docOut["error"] = "\"method\" is required";
					serializeJson(docOut, Serial);
					Serial.println();
				} else if (method == "restart") {
					logger::write("Restarting...");
					esp_restart();
				} else if (method == "echo") {
					const std::string text = json["params"][0].as<const char*>();
					DynamicJsonDocument docOut(1024);
					docOut["jsonrpc"] = jsonRpcVersion;
					docOut["id"] = id;
					docOut["result"] = text;
					serializeJson(docOut, Serial);
					Serial.println();
				} else if (method == "getinfo") {
					DynamicJsonDocument docOut(1024);
					docOut["jsonrpc"] = jsonRpcVersion;
					docOut["id"] = id;
					DynamicJsonDocument docInfo(768);
					docInfo["firmwareName"] = firmwareName;
					docInfo["firmwareCommitHash"] = firmwareCommitHash;
					docInfo["firmwareVersion"] = firmwareVersion;
					docOut["result"] = docInfo;
					serializeJson(docOut, Serial);
					Serial.println();
				} else if (method == "getconfig") {
					DynamicJsonDocument docOut(8192);
					docOut["jsonrpc"] = jsonRpcVersion;
					docOut["id"] = id;
					docOut["result"] = config::getConfigurations();
					serializeJson(docOut, Serial);
					Serial.println();
				} else if (method == "setconfig") {
					DynamicJsonDocument docOut(512);
					docOut["jsonrpc"] = jsonRpcVersion;
					docOut["id"] = id;
					try {
						config::saveConfigurations(json["params"].as<JsonObject>());
						docOut["result"] = true;
					} catch (const std::exception &e) {
						docOut["result"] = false;
						logger::write(e.what(), "error");
						std::cerr << e.what() << std::endl;
					}
					serializeJson(docOut, Serial);
					Serial.println();
				} else if (method == "getlogs") {
					if (!spiffs::isInitialized()) {
						const std::string errorMsg = "SPIFFS file system not initialized. Reformat SPIFFS then try again.";
						logger::write("Failed JSON-RPC command: " + errorMsg, "error");
						DynamicJsonDocument docOut(512);
						docOut["jsonrpc"] = jsonRpcVersion;
						docOut["id"] = id;
						docOut["error"] = errorMsg;
						serializeJson(docOut, Serial);
						Serial.println();
					} else {
						logger::write("Reading logs from SPIFFS file system...");
						Serial.print(std::string("{\"jsonrpc\":\"" + std::string(jsonRpcVersion) + "\",\"id\":\"" + std::string(id) + "\",\"result\":\"").c_str());
						for (int num = 3; num >= 0; num--) {
							const std::string logFilePath = logger::getLogFilePath(num);
							if (spiffs::fileExists(logFilePath.c_str())) {
								File file = SPIFFS.open(logFilePath.c_str(), FILE_READ);
								if (file) {
									while (file.available()) {
										Serial.print(escapeLine(file.readStringUntil('\n').c_str()).c_str());
									}
									file.close();
								}
							}
						}
						Serial.println(std::string("\"}").c_str());
					}
				} else if (method == "deletelogs") {
					if (!spiffs::isInitialized()) {
						const std::string errorMsg = "SPIFFS file system not initialized. Reformat SPIFFS then try again.";
						logger::write("Failed JSON-RPC command: " + errorMsg, "error");
						DynamicJsonDocument docOut(512);
						docOut["jsonrpc"] = jsonRpcVersion;
						docOut["id"] = id;
						docOut["error"] = errorMsg;
						serializeJson(docOut, Serial);
						Serial.println();
					} else {
						logger::write("Deleting logs...");
						DynamicJsonDocument docOut(512);
						docOut["jsonrpc"] = jsonRpcVersion;
						docOut["id"] = id;
						try {
							for (int num = 3; num >= 0; num--) {
								const std::string logFilePath = logger::getLogFilePath(num);
								if (spiffs::fileExists(logFilePath.c_str())) {
									spiffs::deleteFile(logFilePath.c_str());
								}
							}
							docOut["result"] = true;
						} catch (const std::exception &e) {
							docOut["result"] = false;
							logger::write(e.what(), "error");
							std::cerr << e.what() << std::endl;
						}
						serializeJson(docOut, Serial);
						Serial.println();
					}
				} else if (method == "spiffs_reformat") {
					logger::write("Reformatting SPIFFS file system...");
					DynamicJsonDocument docOut(512);
					docOut["jsonrpc"] = jsonRpcVersion;
					docOut["id"] = id;
					docOut["result"] = SPIFFS.format();
					serializeJson(docOut, Serial);
					Serial.println();
				} else {
					const std::string errorMsg = "Unknown method: \"" + method + "\"";
					logger::write("Failed JSON-RPC command: " + errorMsg, "error");
					DynamicJsonDocument docOut(512);
					docOut["jsonrpc"] = jsonRpcVersion;
					docOut["id"] = id;
					docOut["error"] = errorMsg;
					serializeJson(docOut, Serial);
					Serial.println();
				}
			}
		} catch (const std::exception &e) {
			logger::write(e.what(), "error");
			std::cerr << e.what() << std::endl;
		}
	}

	bool pinConflict = false;
	void checkForPinConflicts() {
		const std::vector<const char*> pinConfigKeys = {
			"coinSignalPin", "coinInhibitPin",
			"buttonPin"
		};
		for (int index = 0; index < pinConfigKeys.size(); index++) {
			const char* key = pinConfigKeys[index];
			const unsigned short value = config::getUnsignedShort(key);
			if (value == 3 || value == 1) {
				logger::write("GPIO conflict detected (\"" + std::string(key) + "\" = " + std::to_string(value) + "). Do not use GPIO3 or GPIO1.", "warn");
				pinConflict = true;
			}
		}
	}

	void parseSerialInput() {
		if (Serial.available() > 0) {
			onMessage(std::string(Serial.readStringUntil('\n').c_str()));
		}
	}
}

namespace jsonRpc {

	void init() {
		logger::write("Initializing JSON-RPC serial interface...");
		checkForPinConflicts();
		if (!pinConflict) {
			logger::write("JSON-RPC serial interface now listening...");
		}
		initTime = millis();
	}

	void loop() {
		if (pinConflict) {
			if (!sentWaitingMessage) {
				sentWaitingMessage = true;
				logger::write("JSON-RPC serial interface now listening...");
			}
			if (!timedOut && !receivedMessage && millis() - initTime > maxInitialMessageWaitTime) {
				timedOut = true;
				logger::write("Timed-out while waiting for initial JSON-RPC message");
				delay(50);// Allow some time to finish writing the above log message.
			}
			if (jsonRpc::inUse()) {
				parseSerialInput();
			}
		} else {
			parseSerialInput();
		}
	}

	bool inUse() {
		// Consider JSON-RPC interface to be "in-use" when either:
		// A JSON-RPC message has been received or we are still waiting for an initial message.
		return receivedMessage || !timedOut;
	}

	bool hasPinConflict() {
		return pinConflict;
	}
}
