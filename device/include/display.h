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

#include "logger.h"
#include "qrcode.h"
#include <iomanip>
#include <sstream>
#include <string>
#include <Arduino.h>


namespace display {
	uint8_t calculateAmountTextHeight(const uint8_t &fontHeight, const uint8_t &textMultiplier);
	std::string toUpperCase(std::string s);
	int calculateQRCodeSize(const std::string &dataStr);
	float calculateQRCodeScale(const uint8_t &size, const uint8_t &width, const uint8_t &height,  const uint8_t &fontHeight, const uint8_t &marginX, const uint8_t &marginY, const uint8_t &textMultiplier);
	uint16_t hexToRGB565(const std::string& hexColor);
	int getPrecision(const std::string &fiatCurrency);
}
