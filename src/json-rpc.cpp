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

#include "json-rpc.h"

namespace {

	uint32_t bootTime = micros();
	uint32_t maxInitialMessageWaitTime = 2000000;// microseconds
	bool sentWaitingMessage = false;
	bool receivedMessage = false;
	bool timedOut = false;

	// https://arduinojson.org/
	// https://www.jsonrpc.org/specification
	const std::string jsonRpcVersion = "2.0";

	void onMessage(const std::string &message) {
		try {
			if (message != "" && message.substr(0, 1) == "{") {
				// !! Important !!
				// Keep the JsonDocument instance until done reading from the deserialized document; more info:
				// https://arduinojson.org/v6/issues/garbage-out/
				DynamicJsonDocument docIn(1024);
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
					DynamicJsonDocument docOut(256);
					docOut["jsonrpc"] = jsonRpcVersion;
					docOut["id"] = id;
					docOut["error"] = "\"method\" is required";
					serializeJson(docOut, Serial);
					Serial.println();
				} else if (method == "restart") {
					esp_restart();
				} else if (method == "echo") {
					const std::string text = json["params"][0].as<const char*>();
					DynamicJsonDocument docOut(256);
					docOut["jsonrpc"] = jsonRpcVersion;
					docOut["id"] = id;
					docOut["result"] = text;
					serializeJson(docOut, Serial);
					Serial.println();
				} else if (method == "getinfo") {
					DynamicJsonDocument docOut(512);
					docOut["jsonrpc"] = jsonRpcVersion;
					docOut["id"] = id;
					DynamicJsonDocument docInfo(384);
					docInfo["firmwareName"] = firmwareName;
					docInfo["firmwareCommitHash"] = firmwareCommitHash;
					docInfo["firmwareVersion"] = firmwareVersion;
					docOut["result"] = docInfo;
					serializeJson(docOut, Serial);
					Serial.println();
				} else if (method == "getconfig") {
					DynamicJsonDocument docOut(4096);
					const JsonObject configurations = config::getConfigurations();
					docOut["jsonrpc"] = jsonRpcVersion;
					docOut["id"] = id;
					docOut["result"] = configurations;
					serializeJson(docOut, Serial);
					Serial.println();
				} else if (method == "setconfig") {
					DynamicJsonDocument docOut(4096);
					const JsonObject configurations = json["params"];
					docOut["jsonrpc"] = jsonRpcVersion;
					docOut["id"] = id;
					docOut["result"] = config::saveConfigurations(configurations);
					serializeJson(docOut, Serial);
					Serial.println();
				} else {
					DynamicJsonDocument docOut(256);
					docOut["jsonrpc"] = jsonRpcVersion;
					docOut["id"] = id;
					docOut["error"] = "Unknown method: \"" + method + "\"";
					serializeJson(docOut, Serial);
					Serial.println();
				}
			}
		} catch (const std::exception &e) {
			logger::write("JSON-RPC: " + std::string(e.what()), "error");
		}
	}
}

namespace jsonRpc {

	void loop() {
		if (!sentWaitingMessage) {
			sentWaitingMessage = true;
			logger::write("JSON-RPC serial interface now listening...");
		}
		if (!timedOut && !receivedMessage && micros() - bootTime > maxInitialMessageWaitTime) {
			timedOut = true;
			logger::write("Timed-out while waiting for initial JSON-RPC message");
			delay(50);// Allow some time to finish writing the above log message.
		}
		if (jsonRpc::inUse()) {
			if (Serial.available() > 0) {
				onMessage(std::string(Serial.readStringUntil('\n').c_str()));
			}
		}
	}

	bool inUse() {
		// Consider JSON-RPC interface to be "in-use" when either:
		// A JSON-RPC message has been received or we are still waiting for an initial message.
		return receivedMessage || !timedOut;
	}
}
