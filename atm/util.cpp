#include "util.h"

namespace {
	std::string string_to_hex(const std::string& in) {
		std::stringstream ss;
		ss << std::hex << std::setfill('0');
		for (size_t i = 0; in.length() > i; ++i) {
			ss << std::setw(2) << static_cast<unsigned int>(static_cast<unsigned char>(in[i]));
		}
		return ss.str();
	}
}

namespace std {
	enum class byte : unsigned char {};
}

namespace util {
	std::string bech32_encode(const std::string& hrp, const std::string& value) {
		std::vector<uint8_t> v = ParseHex(string_to_hex(value));
		std::vector<unsigned char> tmp = {};
		tmp.reserve(1 + 32 * 8 / 5);
		ConvertBits<8, 5, true>([&](unsigned char c) { tmp.push_back(c); }, v.begin(), v.end());
		std::string s(v.begin(), v.end());
		return bech32::encode(hrp, tmp);
	}
}
