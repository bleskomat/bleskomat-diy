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

#ifndef BLESKOMAT_UTIL_H
#define BLESKOMAT_UTIL_H

#include "config.h"

#include <Arduino.h>
#include <lnurl.h>

#include <chrono>
#include <cmath>
#include <sstream>
#include <string>
#include <vector>

namespace util {

	std::string createSignedLnurlWithdraw(const double &amount);
	std::string lnurlEncode(const std::string &text);
	std::string toUpperCase(std::string s);
	std::vector<std::string> stringListToStringVector(const std::string &stringList, const char &delimiter = ',');
	std::vector<float> stringListToFloatVector(const std::string &stringList, const char &delimiter = ',');
	std::string floatVectorToStringList(const std::vector<float> floatVector, const char &delimiter = ',');
	std::string urlEncode(const std::string &value);
	std::string doubleToStringWithPrecision(const double &value, const int &precision = 6);
}

#endif
