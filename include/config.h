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

#include "logger.h"
#include "sdcard.h"
#include "util.h"

#include <lnurl.h>
#include <Preferences.h>

#include <algorithm>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>

struct BleskomatConfig {
	struct Lnurl::SignerConfig lnurl;
	std::string uriSchemaPrefix = "LIGHTNING";
	std::string fiatCurrency = "EUR";
	unsigned short fiatPrecision = 2;
	std::vector<float> coinValues;
	float coinValueIncrement;
	unsigned short tftRotation = 2;
};

namespace config {
	void init();
	Lnurl::SignerConfig getLnurlSignerConfig();
	std::string get(const char* &t_key);
	std::string get(const std::string &key);
	unsigned short getFiatPrecision();
	std::vector<float> getCoinValues();
	float getCoinValueIncrement();
	unsigned short getTftRotation();
}

#endif
