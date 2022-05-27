#ifndef BLESKOMAT_UTIL_H
#define BLESKOMAT_UTIL_H

#include "config.h"

#include <Arduino.h>
#include <lnurl.h>

#include <chrono>
#include <cmath>
#include <cstring>
#include <sstream>
#include <string>
#include <vector>

namespace util {

	// This is needed to use char* as the key in std::map.
	struct MapCharPointerComparator {
		bool operator()(char const *a, char const *b) const {
			return std::strcmp(a, b) < 0;
		}
	};

	std::string createSignedLnurlWithdraw(const double &amount);
	std::string lnurlEncode(const std::string &text);
	std::string toUpperCase(std::string s);
	std::vector<float> stringListToFloatVector(const std::string &stringList, const char &delimiter = ',');
	std::string floatVectorToStringList(const std::vector<float> floatVector, const char &delimiter = ',');
	std::string urlEncode(const std::string &value);
	std::string floatToStringWithPrecision(const float &value, const unsigned short &precision = 6);
}

#endif
