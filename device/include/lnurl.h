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

#include "esp_system.h"
#include "mbedtls/md.h"
#include "bech32.h"
#include <cstring>
#include <iomanip>
#include <sstream>
#include <string>
#include "utilstrencodings.h"

namespace lnurl {
	std::string create_signed_withdraw_request(
		const float &amount,
		const std::string &fiatCurrency,
		const std::string &apiKeyId,
		const std::string &apiKeySecret,
		const std::string &callbackUrl
	);
}
