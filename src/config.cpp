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
#include "config.h"

namespace {

	// The configuration object:
	BleskomatConfig values;

	const std::string configFileName = "bleskomat.conf";

	// List of configuration keys:
	const std::vector<std::string> configKeys = {
		"apiKey.id",
		"apiKey.key",
		"apiKey.encoding",
		"callbackUrl",
		"shorten",
		"uriSchemaPrefix",
		"fiatCurrency",
		"fiatPrecision",
		"coinValues",
		"coinValueIncrement",
		"tftRotation"
	};

	// Using Preferences library as a wrapper to Non-Volatile Storage (flash memory):
	// https://github.com/espressif/arduino-esp32/tree/master/libraries/Preferences
	// https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/storage/nvs_flash.html
	BleskomatConfig nvs_values;
	const std::string nvs_namespace = "BleskomatConfig";
	const bool nvs_readonly = false;
	Preferences nvs_prefs;
	bool nvs_available = false;

	bool setConfigValue(const std::string &key, const std::string &value, BleskomatConfig &t_values) {
		if (key == "apiKey.id") {
			t_values.lnurl.apiKey.id = value;
		} else if (key == "apiKey.key") {
			t_values.lnurl.apiKey.key = value;
		} else if (key == "apiKey.encoding") {
			t_values.lnurl.apiKey.encoding = value;
		} else if (key == "callbackUrl") {
			t_values.lnurl.callbackUrl = value;
		} else if (key == "shorten") {
			t_values.lnurl.shorten = (value == "true" || value == "1");
		} else if (key == "uriSchemaPrefix") {
			t_values.uriSchemaPrefix = value;
		} else if (key == "fiatCurrency") {
			t_values.fiatCurrency = value;
		} else if (key == "fiatPrecision") {
			// Convert string to short:
			t_values.fiatPrecision = (char)( *value.c_str() - '0' );
		} else if (key == "coinValues") {
			t_values.coinValues = util::stringListToFloatVector(value);
		} else if (key == "coinValueIncrement") {
			// Convert string to float:
			t_values.coinValueIncrement = std::atof(value.c_str());
		} else if (key == "tftRotation") {
			// Convert string to short:
			t_values.tftRotation = (char)( *value.c_str() - '0' );;
		} else {
			return false;
		}
		return true;
	}

	std::string getConfigValue(const std::string &key, const BleskomatConfig &t_values) {
		if (key == "apiKey.id") {
			return t_values.lnurl.apiKey.id;
		} else if (key == "apiKey.key") {
			return t_values.lnurl.apiKey.key;
		} else if (key == "apiKey.encoding") {
			return t_values.lnurl.apiKey.encoding;
		} else if (key == "callbackUrl") {
			return t_values.lnurl.callbackUrl;
		} else if (key == "shorten") {
			return t_values.lnurl.shorten ? "true" : "false";
		} else if (key == "uriSchemaPrefix") {
			return t_values.uriSchemaPrefix;
		} else if (key == "fiatCurrency") {
			return t_values.fiatCurrency;
		} else if (key == "fiatPrecision") {
			return std::to_string(t_values.fiatPrecision);
		} else if (key == "coinValues") {
			return util::floatVectorToStringList(t_values.coinValues);
		} else if (key == "coinValueIncrement") {
			return util::doubleToStringWithPrecision(t_values.coinValueIncrement, t_values.fiatPrecision);
		} else if (key == "tftRotation") {
			return std::to_string(t_values.tftRotation);
		}
		return "";
	}

	bool readFromConfigLine(const std::string &line, BleskomatConfig &t_values) {
		// The character used to separate key/value pair - e.g "key=value".
		const std::string delimiter = "=";
		const auto pos = line.find(delimiter);
		if (pos != std::string::npos) {
			// Found delimiter.
			const std::string key = line.substr(0, pos);
			const std::string value = line.substr(pos + 1);
			if (setConfigValue(key, value, t_values)) {
				return true;
			} else {
				logger::write("Unknown key found in configuration file: \"" + key + "\"");
			}
		}
		return false;
	}

	std::string getConfigFilePath() {
		std::string configFilePath = sdcard::getMountPoint();
		configFilePath += "/" + configFileName;
		return configFilePath;
	}

	bool readFromConfigFile() {
		try {
			const std::string filePath = getConfigFilePath();
			// Open the config file for reading.
			std::ifstream file(filePath);
			if (!file) {
				logger::write("Failed to open configuration file " + filePath);
				return false;
			}
			std::string line = "";
			while (std::getline(file, line)) {
				readFromConfigLine(line, values);
			}
			file.close();
		} catch (const std::exception &e) {
			std::cerr << e.what() << std::endl;
			return false;
		}
		return true;
	}

	bool deleteConfigFile() {
		const std::string filePath = sdcard::getMountedPath(configFileName);
		return std::remove(filePath.c_str()) == 0;
	}

	bool initNVS() {
		const char* name = nvs_namespace.c_str();
		const bool result = nvs_prefs.begin(name, nvs_readonly);
		if (result) {
			nvs_available = true;
		}
		return result;
	}

	bool readKeyValueFromNVS(const std::string &key) {
		// Maximum NVS key length is 15 characters.
		const std::string value = nvs_prefs.getString(key.substr(0, 15).c_str(), "").c_str();
		return setConfigValue(key, value, nvs_values) && setConfigValue(key, value, values);
	}

	bool readFromNVS() {
		if (!nvs_available) {
			if (!initNVS()) {
				return false;
			}
		}
		for (int index = 0; index < configKeys.size(); index++) {
			const std::string key = configKeys[index];
			readKeyValueFromNVS(key);
		}
		return true;
	}

	bool saveKeyValueToNVS(const std::string &key, const std::string &value) {
		// Maximum NVS key length is 15 characters.
		return nvs_prefs.putString(key.substr(0, 15).c_str(), value.c_str()) != 0;
	}

	bool saveConfigurationsToNVS(const BleskomatConfig &t_values) {
		if (!nvs_available) {
			if (!initNVS()) {
				return false;
			}
		}
		for (int index = 0; index < configKeys.size(); index++) {
			const std::string key = configKeys[index];
			const std::string value = getConfigValue(key, t_values);
			if (value != getConfigValue(key, nvs_values)) {
				// Configuration has been changed.
				// Save the new value to non-volatile storage.
				if (!saveKeyValueToNVS(key, value)) {
					logger::write("Failed to save configuration to non-volatile storage ( " + key + "=" + value + " )");
				}
			}
		}
		return true;
	}

	void endNVS() {
		nvs_prefs.end();
		nvs_available = false;
	}

	void printConfig(const BleskomatConfig &t_values) {
		std::string msg = "Printing Bleskomat configurations:\n";
		for (int index = 0; index < configKeys.size(); index++) {
			const std::string key = configKeys[index];
			const std::string value = getConfigValue(key, t_values);
			msg += "  " + key + "=";
			if (value != "") {
				if (key == "apiKey.key") {
					// Don't print some configuration value(s).
					msg += "XXX";
				} else {
					msg += value;
				}
			}
			msg += "\n";
		}
		msg.pop_back();// Remove the last line-break character.
		logger::write(msg);
	}
}

namespace config {

	void init() {
		if (initNVS()) {
			logger::write("Non-volatile storage initialized");
			if (!readFromNVS()) {
				logger::write("Failed to read configurations from non-volatile storage");
			}
		} else {
			logger::write("Failed to initialize non-volatile storage");
		}
		if (sdcard::isMounted()) {
			if (readFromConfigFile()) {
				if (saveConfigurationsToNVS(values)) {
					if (deleteConfigFile()) {
						logger::write("Deleted configuration file");
					} else {
						logger::write("Failed to delete configuration file");
					}
				} else {
					logger::write("Failed to save configurations to non-volatile storage");
				}
			} else {
				logger::write("Failed to read configurations from file");
			}
		}
		endNVS();
		// Hard-coded configuration overrides - for development purposes.
		// Uncomment the following lines, as needed, to override config options.
		// values.lnurl.apiKey.id = "";
		// values.lnurl.apiKey.key = "";
		// values.lnurl.apiKey.encoding = "";
		// values.lnurl.callbackUrl = "https://your-bleskomat-server.com/u";
		// values.lnurl.shorten = true;
		// values.uriSchemaPrefix = "";
		// values.fiatCurrency = "EUR";
		// values.fiatPrecision = 2;
		// values.coinValues = { 0.05, 0.10, 0.20, 0.50, 1.00, 2.00 };// DG600F
		// values.coinValueIncrement = 0.05;// HX616
		// values.tftRotation = 2;
		printConfig(values);
	}

	Lnurl::SignerConfig getLnurlSignerConfig() {
		return values.lnurl;
	}

	std::string get(const char* t_key) {
		const std::string key = std::string(t_key);
		return get(key);
	}

	std::string get(const std::string &key) {
		return getConfigValue(key, values);
	}

	unsigned short getFiatPrecision() {
		return values.fiatPrecision;
	}

	std::vector<float> getCoinValues() {
		return values.coinValues;
	}

	float getCoinValueIncrement() {
		return values.coinValueIncrement;
	}

	unsigned short getTftRotation() {
		return values.tftRotation;
	}

	JsonObject getConfigurations() {
		DynamicJsonDocument doc(4096);
		for (int index = 0; index < configKeys.size(); index++) {
			const std::string key = configKeys[index];
			const std::string value = getConfigValue(key, values);
			if (value != "" && key == "apiKey.key") {
				doc[key] = "XXX";
			} else {
				doc[key] = value;
			}
		}
		return doc.to<JsonObject>();
	}

	bool saveConfigurations(const JsonObject &configurations) {
		if (!nvs_available && !initNVS()) {
			return false;
		}
		for (int index = 0; index < configKeys.size(); index++) {
			const std::string key = configKeys[index];
			if (configurations.containsKey(key)) {
				const std::string value = configurations[key];
				if (value != getConfigValue(key, nvs_values)) {
					saveKeyValueToNVS(key, value);
				}
				setConfigValue(key, value, values);
			}
		}
		endNVS();
		return true;
	}
}
