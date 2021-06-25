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

#ifndef BLESKOMAT_MODULES_TFT_H
#define BLESKOMAT_MODULES_TFT_H

#include "logger.h"
#include "util.h"

#include <Arduino.h>
#include <qrcode.h>
#include <SPI.h>
#include <TFT_eSPI.h>

#include <cmath>
#include <stdexcept>
#include <string>

namespace tft {
	void init();
	std::string getCurrentScreen();
	void showInsertFiatScreen(const float &amount);
	void showTradeCompleteScreen(const float &amount, const std::string &qrcodeData);
}

#endif
