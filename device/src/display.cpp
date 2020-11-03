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

#include "display.h"

namespace display {

	uint8_t calculateAmountTextHeight(const uint8_t &fontHeight, const uint8_t &textMultiplier) {
		return fontHeight * textMultiplier;
	}

	std::string toUpperCase(std::string s) {
		std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c){ return std::toupper(c); });
		return s;
	}

	int calculateQRCodeSize(const std::string &dataStr) {
		int size = 12;
		int sizes[17] = { 25, 47, 77, 114, 154, 195, 224, 279, 335, 395, 468, 535, 619, 667, 758, 854, 938 };
		int len = dataStr.length();
		for (int i = 0; i < 17; i++) {
			if (sizes[i] > len) {
				return i + 1;
			}
		}
		return size;
	}

	float calculateQRCodeScale(const uint8_t &size, const uint8_t &width, const uint8_t &height,  const uint8_t &fontHeight, const uint8_t &marginX, const uint8_t &marginY, const uint8_t &textMultiplier) {
		const uint8_t maxWidth = width - (marginX * 2);
		const uint8_t maxHeight = height - (calculateAmountTextHeight(fontHeight, textMultiplier) + marginY);
		return std::fmin(
			std::floor(maxWidth / size),
			std::floor(maxHeight / size)
		);
	}

	// http://www.barth-dev.de/about-rgb565-and-how-to-convert-into-it/
	uint16_t hexToRGB565(const std::string& hexColor) {
		uint16_t Rgb565 = 0;
		unsigned int x;
		std::stringstream ss;
		ss << std::hex << hexColor;
		ss >> x;
		uint8_t red = (x >> 16) & 255;
		uint8_t green = (x >> 8) & 255;
		uint8_t blue = x & 255;
		Rgb565 = (((red & 0xf8)<<8) + ((green & 0xfc)<<3) + (blue>>3));
		return Rgb565;
	}

	int getPrecision(const std::string &fiatCurrency) {
		if (fiatCurrency == "EUR") {
			return 2;
		}
		return 0;
	}
}
