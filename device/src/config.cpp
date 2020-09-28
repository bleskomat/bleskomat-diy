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
	std::string trimQuotes(const std::string &str) {
		return str.substr(1, str.length() - 2);
	}
}

namespace config {
  std::string apiKeyId;
  std::string apiKeySecret;
  std::string callbackUrl;
  std::string fiatCurrency;

  void init() {
    // std::string apiKeyId(trimQuotes(STRINGIFY(API_KEY_ID)));
    // std::string apiKeySecret(trimQuotes(STRINGIFY(API_KEY_SECRET)));
    // std::string callbackUrl(trimQuotes(STRINGIFY(CALLBACK_URL)));
    // std::string fiatCurrency(trimQuotes(STRINGIFY(FIAT_CURRENCY)));

    logger::write("apiKeyId: " + apiKeyId);
    logger::write("apiKeySecret: " + apiKeySecret);
    logger::write("callbackUrl: " + callbackUrl);
    logger::write("fiatCurrency: " + fiatCurrency);
  }

  void setDefault(){
    // apiKeyId(trimQuotes(STRINGIFY(API_KEY_ID)));
    // apiKeySecret(trimQuotes(STRINGIFY(API_KEY_SECRET)));
    // callbackUrl(trimQuotes(STRINGIFY(CALLBACK_URL)));
    // fiatCurrency(trimQuotes(STRINGIFY(FIAT_CURRENCY)));

  }

  int setConfig(std::ifstream fpd){

    if(!fpd)
      {
	printf("Error: config file could not be open,\n"
	       "please create 'bleskomat.conf' in the root directory of the SD card.");
	return -1;
      }
	

    std::getline(fpd, config::apiKeyId);
    std::getline(fpd, config::apiKeySecret);
    std::getline(fpd, config::callbackUrl);
    std::getline(fpd, config::fiatCurrency);

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
