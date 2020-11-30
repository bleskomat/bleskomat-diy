/*
	Copyright (C) 2020 Samotari (Charles Hill, Carlos Garcia Ortiz)
	Copyright (C) 2020 Tomas Stary

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

#ifndef BLESKOMAT_MODULES_EPAPER_H
#define BLESKOMAT_MODULES_EPAPER_H

#include "images/bleskomat_insert_fiat_screen_400x300.h"
#include "images/bleskomat_instructions_screen_400x300.h"
#include "images/bleskomat_splash_screen_400x300.h"
#include "images/bleskomat_transaction_complete_screen_400x300.h"

#include "logger.h"
#include "util.h"

#ifndef EPAPER_CLK
#define EPAPER_CLK 13
#endif

#ifndef EPAPER_MISO
#define EPAPER_MISO 12
#endif

#ifndef EPAPER_DIN
#define EPAPER_DIN 14
#endif

#ifndef EPAPER_CS
#define EPAPER_CS 15
#endif

#ifndef EPAPER_DC
#define EPAPER_DC 27
#endif

#ifndef EPAPER_RST
#define EPAPER_RST 26
#endif

#ifndef EPAPER_BUSY
#define EPAPER_BUSY 25
#endif

#define ENABLE_GxEPD2_GFX 0

#include <Arduino.h>
#include <qrcode.h>
#include <Wire.h>
#include <GxEPD2_GFX.h>
#include <GxEPD2_BW.h>
#include <Fonts/FreeMonoBold12pt7b.h>
#include <Fonts/FreeMonoBold18pt7b.h>
#include <Fonts/FreeMonoBold24pt7b.h>

namespace epaper {
	void init();
	void showSplashScreen();
	void showInstructionsScreen();
	void showInsertFiatScreen(const std::string &fiatCurrency);
	void updateInsertFiatScreenAmount(const float &amount, const std::string &fiatCurrency);
	void showTransactionCompleteScreen(const float &amount, const std::string &fiatCurrency, const std::string &qrcodeData);
	float getRenderedAmount();
	bool hasRenderedQRCode();
	unsigned long getTimeSinceRenderedQRCode();
}

namespace screen = epaper;

#endif
