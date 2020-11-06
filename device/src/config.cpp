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

	int readFromFile(const char* fileName) {
		File dataFile = SD_MMC.open(fileName, FILE_READ);
		if (!dataFile) {
			logger::write("Failed to open file " + std::string(fileName));
			return -1;
		}
		std::string buffer = "";
		while (dataFile.available()) {
			buffer += dataFile.read();
		}
		std::cout << buffer;
		dataFile.close();
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
