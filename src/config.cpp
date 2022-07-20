#include "config.h"

namespace {

	const char* configFilePath = "/bleskomat.conf";

	typedef std::pair<const std::string, const std::string> KeyValuePair;

	const std::map<const char*, const char*> defaultValues = {
		{ "apiKey.id", "" },
		{ "apiKey.key", "" },
		{ "apiKey.encoding", "" },
		{ "callbackUrl", "https://p.bleskomat.com/u" },
		{ "shorten", "true" },
		{ "uriSchemaPrefix", "" },
		{ "fiatCurrency", "EUR" },
		{ "fiatPrecision", "2" },
		{ "coinValues", "0.05,0.10,0.20,0.50,1.00,2.00" },// DG600F
		{ "coinValueIncrement", "0.05" },// HX616
		{ "coinSignalPin", "3" },
		{ "coinInhibitPin", "1" },
		{ "coinBaudRate", "9600" },
		{ "coinAcceptorType", "hx616" },
		{ "buttonPin", "33" },
		{ "buttonDelay", "5000" },
		{ "buttonDebounce", "100" },
		{ "tftRotation", "2" },
		{ "logLevel", "info" },
		{ "spiffsFormatted", "false" }
	};

	// https://arduinojson.org/v6/api/dynamicjsondocument/
	DynamicJsonDocument configJsonDoc(8192);
	JsonObject values;// current configuration values

	// Using Preferences library as a wrapper to Non-Volatile Storage (flash memory):
	// https://github.com/espressif/arduino-esp32/tree/master/libraries/Preferences
	// https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/storage/nvs_flash.html
	const char* nvs_namespace = "BleskomatConfig";
	const bool nvs_readonly = false;
	Preferences nvs_prefs;
	bool nvs_available = false;

	bool isConfigKey(const char* key) {
		if (values.containsKey(key)) {
			return true;
		}
		const auto pos = defaultValues.find(key);
		return pos != defaultValues.end();
	}

	bool setConfigValue(const char* key, const std::string &value) {
		if (isConfigKey(key)) {
			values[key] = value;
			return true;
		}
		return false;
	}

	std::string getConfigValue(const char* key) {
		if (values.containsKey(key)) {
			const std::string value = values[key].as<const char*>();
			return value;
		}
		return "";
	}

	bool initNVS() {
		const bool result = nvs_prefs.begin(nvs_namespace, nvs_readonly);
		if (result) {
			nvs_available = true;
		}
		return result;
	}

	void endNVS() {
		nvs_prefs.end();
		nvs_available = false;
	}

	// Maximum NVS key length is 15 characters.
	const unsigned short nvsKeyMaxLength = 15;

	std::string truncateNVSKey(const char* key) {
		return std::string(key).substr(0, nvsKeyMaxLength);
	}

	bool keyExistsInNVS(const char* t_key) {
		const std::string key = truncateNVSKey(t_key);
		return nvs_prefs.isKey(key.c_str());
	}

	std::string readValueFromNVS(const char* t_key) {
		const std::string key = truncateNVSKey(t_key);
		return std::string(nvs_prefs.getString(key.c_str(), "").c_str());
	}

	void saveKeyValueToNVS(const char* t_key, const std::string &value) {
		if (!keyExistsInNVS(t_key) || readValueFromNVS(t_key) != value) {
			const std::string key = truncateNVSKey(t_key);
			nvs_prefs.putString(key.c_str(), value.c_str());
		}
	}

	bool readFromNVS() {
		if (!nvs_available && !initNVS()) {
			return false;
		}
		for (auto const& defaultValue : defaultValues) {
			const char* key = defaultValue.first;
			if (keyExistsInNVS(key)) {
				setConfigValue(key, readValueFromNVS(key));
			} else {
				setConfigValue(key, defaultValue.second);
			}
		}
		return true;
	}

	KeyValuePair readFromConfigLine(const std::string &line) {
		// The character used to separate key/value pair - e.g "key=value".
		const std::string delimiter = "=";
		const auto pos = line.find(delimiter);
		if (pos != std::string::npos) {
			// Found delimiter.
			const std::string key = line.substr(0, pos);
			if (isConfigKey(key.c_str())) {
				const std::string value = line.substr(pos + 1);
				return std::make_pair(key, value);
			} else {
				logger::write("Unknown key found in configuration file: \"" + std::string(key) + "\"");
			}
		}
		return std::make_pair("", "");
	}

	bool readFromConfigFile() {
		try {
			File file = SD.open(configFilePath, FILE_READ);
			if (!file || file.isDirectory()) {
				logger::write("Failed to open configuration file " + std::string(configFilePath));
				return false;
			}
			while (file.available()) {
				const std::string line = file.readStringUntil('\n').c_str();
				const KeyValuePair kv = readFromConfigLine(line);
				const std::string key = kv.first;
				if (key != "") {
					const std::string value = kv.second;
					setConfigValue(key.c_str(), value);
					saveKeyValueToNVS(key.c_str(), value);
				}
			}
			file.close();
		} catch (const std::exception &e) {
			logger::write(e.what(), "error");
			std::cerr << e.what() << std::endl;
			return false;
		}
		return true;
	}

	bool deleteConfigFile() {
		return SD.remove(configFilePath);
	}
}

namespace config {

	void init() {
		values = configJsonDoc.createNestedObject("values");
		if (initNVS()) {
			std::cout << "Non-volatile storage initialized" << std::endl;
			if (!readFromNVS()) {
				std::cout << "Failed to read configurations from non-volatile storage" << std::endl;
			}
		} else {
			std::cout << "Failed to initialize non-volatile storage" << std::endl;
		}
		if (sdcard::isMounted()) {
			if (readFromConfigFile()) {
				if (deleteConfigFile()) {
					std::cout << "Deleted configuration file" << std::endl;
				} else {
					std::cout << "Failed to delete configuration file" << std::endl;
				}
			} else {
				std::cout << "Failed to read configurations from file" << std::endl;
			}
		}
		endNVS();
		// Hard-coded configuration overrides - for development purposes.
		// Uncomment the following lines, as needed, to override config options.
		// values["apiKey.id"] = "";
		// values["apiKey.key"] = "";
		// values["apiKey.encoding"] = "";
		// values["callbackUrl"] = "https://p.bleskomat.com/u";
		// values["shorten"] = "true";
		// values["uriSchemaPrefix"] = "";
		// values["fiatCurrency"] = "EUR";
		// values["fiatPrecision"] = "2";
		// values["coinValues"] = "0.05,0.10,0.20,0.50,1.00,2.00";// DG600F
		// values["coinValueIncrement"] = "0.05";// HX616
		// values["coinSignalPin"] = "9";
		// values["coinInhibitPin"] = "10";
		// values["coinBaudRate"] = "9600";
		// values["coinAcceptorType"] = "hx616";
		// values["buttonPin"] = "33";
		// values["buttonDelay"] = "2000";
		// values["buttonDebounce"] = "50";
		// values["tftRotation"] = "2";
		// values["logLevel"] = "info";
	}

	Lnurl::SignerConfig getLnurlSignerConfig() {
		struct Lnurl::SignerConfig lnurl;
		lnurl.apiKey.id = config::getString("apiKey.id");
		lnurl.apiKey.key = config::getString("apiKey.key");
		lnurl.apiKey.encoding = config::getString("apiKey.encoding");
		lnurl.callbackUrl = config::getString("callbackUrl");
		lnurl.shorten = config::getBool("shorten");
		return lnurl;
	}

	std::string getString(const char* key) {
		return getConfigValue(key);
	}

	unsigned int getUnsignedInt(const char* key) {
		const std::string value = getConfigValue(key);
		if (value != "") {
			return (unsigned int) std::stoi(value);
		}
		return 0;
	}

	unsigned short getUnsignedShort(const char* key) {
		const std::string value = getConfigValue(key);
		if (value != "") {
			return (unsigned short) std::stoi(value);
		}
		return 0;
	}

	float getFloat(const char* key) {
		const std::string value = getConfigValue(key);
		if (value != "") {
			return std::atof(value.c_str());
		}
		return 0;
	}

	std::vector<float> getFloatVector(const char* key) {
		return util::stringListToFloatVector(getConfigValue(key));
	}

	bool getBool(const char* key) {
		const std::string value = getConfigValue(key);
		return (value == "true" || value == "1");
	}

	JsonObject getConfigurations() {
		DynamicJsonDocument docConfigs(8192);
		for (JsonPair kv : values) {
			const char* key = kv.key().c_str();
			const std::string value = kv.value().as<const char*>();
			if (key == "apiKey.key") {
				docConfigs[key] = "XXX";
			} else {
				docConfigs[key] = value;
			}
		}
		return docConfigs.as<JsonObject>();
	}

	std::string getConfigurationsAsString() {
		std::string str = "Bleskomat configurations:\n";
		for (JsonPair kv : values) {
			const char* key = kv.key().c_str();
			const std::string value = kv.value().as<const char*>();
			str += "  " + std::string(key) + "=";
			if (value != "") {
				if (key == "apiKey.key") {
					str += "XXX";
				} else {
					str += value;
				}
			}
			str += "\n";
		}
		str.pop_back();// remove last line-break character
		return str;
	}

	bool saveConfigurations(const JsonObject &configurations) {
		if (!nvs_available && !initNVS()) {
			return false;
		}
		for (JsonPair kv : configurations) {
			const char* key = kv.key().c_str();
			if (isConfigKey(key)) {
				const std::string value = kv.value().as<const char*>();
				saveKeyValueToNVS(key, value);
				setConfigValue(key, value);
			}
		}
		endNVS();
		return true;
	}
}
