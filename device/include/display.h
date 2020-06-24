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

#include "SPI.h"
#include "TFT_eSPI.h"
#include "logger.h"
#include "qrcode.h"
#include <iomanip>
#include <sstream>
#include <string>
#include <Arduino.h>

#ifndef BG_COLOR_HEX
	#define BG_COLOR_HEX "FFFFFF"
#endif

#ifndef TEXT_COLOR_HEX
	#define TEXT_COLOR_HEX "000000"
#endif

namespace display {
	void init();
	void updateAmount(const float &amount, const std::string &fiatCurrency);
	void clearAmount();
	float getRenderedAmount();
	void renderQRCode(const std::string &dataStr);
	void clearQRCode();
	bool hasRenderedQRCode();
	unsigned long getTimeSinceRenderedQRCode();
}
