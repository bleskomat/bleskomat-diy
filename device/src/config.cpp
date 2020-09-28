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
#include "sdcard.h"

struct LNconf {
	char id[63];
	char secret[63];
	char url[255];
	char currency[7];
};

namespace config {
	std::string apiKeyId;
	std::string apiKeySecret;
	std::string callbackUrl;
	std::string fiatCurrency;


	void init() {
		if ( sdcard::mount() < 0 ) {
			printf("SD card failed to mount, setting default config.\n");
			setDefault();
		} else {
			printf("Card mounted, set config from SD card.\n");
			if ( setFromFile("/sdcard/bleskomat.conf") < 0 ) {
				printf("Setting from file failed, falling back to default config.\n");
				setDefault();
			}
			sdcard::umount();
		}
	}

	void printConfig() {
		logger::write("apiKeyId: " + apiKeyId);
		logger::write("apiKeySecret: " + apiKeySecret);
		logger::write("callbackUrl: " + callbackUrl);
		logger::write("fiatCurrency: " + fiatCurrency);
	}

	void setConfig(const LNconf config) {
		apiKeyId.assign(config.id);
		apiKeySecret.assign(config.secret);
		callbackUrl.assign(config.url);
		fiatCurrency.assign(config.currency);
	}

	void setDefault() {
		LNconf config;

		strcpy(config.id, API_KEY_ID);
		strcpy(config.secret, API_KEY_SECRET);
		strcpy(config.url, CALLBACK_URL);
		strcpy(config.currency, FIAT_CURRENCY);

		setConfig(config);
	}

	int setFromFile(const char* fileName) {
		LNconf config;
		FILE* fp = fopen(fileName, "r");

		if ( !fp ) {
			printf("Failed opening config file %s.\n", fileName);
			return ERR_FILE;
		}

		if ( !fscanf(fp, "%s", config.id) ){
			printf("Failed reading ID.\n");
			return ERR_VARS;
		}

		if ( !fscanf(fp, "%s", config.secret) ) {
			printf("Failed reading secret.\n");
			return ERR_VARS;
		}

		if ( !fscanf(fp, "%s", config.url) ) {
			printf("Failed reading URL.\n");
			return ERR_VARS;
		}

		if ( !fscanf(fp, "%s", config.currency) ) {
			printf("Failed reading currency.\n");
			return ERR_VARS;
		}

		setConfig(config);
		fclose(fp);

		return 0;
	}

	std::string getApiKeyId(){
		return apiKeyId;
	}
	std::string getApiKeySecret(){
		return apiKeySecret;
	}
	std::string getCallbackUrl(){
		return callbackUrl;
	}
	std::string getFiatCurrency(){
		return fiatCurrency;
	}

}
