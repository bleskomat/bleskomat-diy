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

	LnurlSignerConfig values;

	void printConfig() {
		logger::write("config.apiKey.id: " + values.apiKey.id);
		logger::write("config.apiKey.key: " + values.apiKey.key);
		logger::write("config.apiKey.encoding: " + values.apiKey.encoding);
		logger::write("config.callbackUrl: " + values.callbackUrl);
		logger::write("config.fiatCurrency: " + values.fiatCurrency);
	}

	void readFromLine(const std::string &line) {
		// The character used to separate key/value pair - e.g "key=value".
		const std::string delimiter = "=";
		const auto pos = line.find(delimiter);
		if (pos != std::string::npos) {
			// Found delimiter.
			const std::string key = line.substr(0, pos);
			const std::string value = line.substr(pos + 1);
			if (key == "apiKey.id") {
				values.apiKey.id = value;
			} else if (key == "apiKey.key") {
				values.apiKey.key = value;
			} else if (key == "apiKey.encoding") {
				values.apiKey.encoding = value;
			} else if (key == "callbackUrl") {
				values.callbackUrl = value;
			} else if (key == "fiatCurrency") {
				values.fiatCurrency = value;
			} else if (key == "shorten") {
				values.shorten = (value == "true" || value == "1");
			} else {
				logger::write("Unknown key found in configuration file: \"" + key + "\"");
			}
		}
	}

	int readFromFile(const char* fileName) {
		try {
			std::ifstream file(fileName);
			if (!file) {
				logger::write("Failed to open file " + std::string(fileName));
				return -1;
			}
			std::string line = "";
			while (std::getline(file, line)) {
				readFromLine(line);
			}
			file.close();
		} catch (const std::exception &e) {
			std::cerr << e.what() << std::endl;
		}
		return 0;
	}
}

namespace config {

	void init() {
		if (!sdcard::isReady()) {
			logger::write("SD card failed to mount. Using default configuration.");
		} else {
			logger::write("SD card mounted. Reading configuration file from SD card.");
			if (readFromFile("/sdcard/bleskomat.conf") != 0) {
				logger::write("Failed to read configuration file. Falling back to defaults.");
			}
		}
		printConfig();
	}

	LnurlSignerConfig getConfig() {
		return values;
	}
}
