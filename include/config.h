#ifndef BLESKOMAT_CONFIG_H
#define BLESKOMAT_CONFIG_H

#include "logger.h"
#include "sdcard.h"
#include "util.h"

#include <ArduinoJson.h>
#include <lnurl.h>
#include <Preferences.h>

#include <algorithm>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>

namespace config {
	void init();
	Lnurl::SignerConfig getLnurlSignerConfig();
	std::string getString(const char* key);
	unsigned int getUnsignedInt(const char* key);
	unsigned short getUnsignedShort(const char* key);
	float getFloat(const char* key);
	std::vector<float> getFloatVector(const char* key);
	bool getBool(const char* key);
	JsonObject getConfigurations();
	std::string getConfigurationsAsString();
	bool saveConfigurations(const JsonObject &configurations);
}

#endif
