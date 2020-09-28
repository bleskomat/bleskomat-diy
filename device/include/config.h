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

#ifndef BLESKOMAT_CONFIG_H
#define BLESKOMAT_CONFIG_H

#ifndef API_KEY_ID
	#error "Missing required build flag: API_KEY_ID"
#endif

#ifndef API_KEY_SECRET
	#error "Missing required build flag: API_KEY_SECRET"
#endif

#ifndef CALLBACK_URL
	#error "Missing required build flag: CALLBACK_URL"
#endif

#ifndef FIAT_CURRENCY
	#error "Missing required build flag: FIAT_CURRENCY"
#endif

#include "logger.h"

#define ERR_FILE -1
#define ERR_VARS -2

struct LNconf;

namespace config {
	void init();
	void printConfig();
	void setConfig(const LNconf config);
	void setDefault();
	int  setFromFile(const char* fileName);
	std::string getApiKeyId();
	std::string getApiKeySecret();
	std::string getCallbackUrl();
	std::string getFiatCurrency();
}

#endif
