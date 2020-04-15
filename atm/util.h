#include "bech32.h"
#include <cctype>
#include <cstdint>
#include <cstring>
#include <iomanip>
#include <stdexcept>
#include <stdio.h>
#include <sstream>
#include <string>
#include "utilstrencodings.h"

namespace util {
	std::string bech32_encode(const std::string& hrp, const std::string& value);
}
