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

#include <Arduino.h>

#include "config.h"
#include "logger.h"
#include "modules.h"
#include "sdcard.h"
#include "util.h"

#include <string>

void setup() {
	Serial.begin(115200);
	logger::enable();
	sdcard::init();
	config::init();
	logger::write("Config OK");
	screen::init();
	// screen::showSplashScreen();
	screen::showInsertFiatScreen(config::getConfig().fiatCurrency);
	// screen::showTransactionCompleteScreen(0, config::getConfig().fiatCurrency, "HELLO WORLD");
	logger::write("Screen OK");
	coinAcceptor::init();
	coinAcceptor::setFiatCurrency(config::getConfig().fiatCurrency);
	logger::write("Coin Reader OK");
	logger::write("Setup OK");
}

const unsigned long bootTime = millis();// milliseconds
const unsigned long minWaitAfterBootTime = 2000;// milliseconds
const unsigned long minWaitTimeSinceInsertedFiat = 15000;// milliseconds
const unsigned long maxTimeDisplayQrCode = 12000;// milliseconds

void loop() {
	return;
	if (millis() - bootTime >= minWaitAfterBootTime) {
		// Minimum time has passed since boot.
		// Start performing checks.
		coinAcceptor::loop();
		if (screen::getTimeSinceRenderedQRCode() >= maxTimeDisplayQrCode) {
			// Some time has passed.
			screen::showInstructionsScreen();
		} else if (coinAcceptor::coinInserted() && screen::hasRenderedQRCode()) {
			// Coin was inserted.
			screen::showInsertFiatScreen(config::getConfig().fiatCurrency);
		}
		float accumulatedValue = coinAcceptor::getAccumulatedValue();
		if (
			accumulatedValue > 0 &&
			coinAcceptor::getTimeSinceLastInserted() >= minWaitTimeSinceInsertedFiat
		) {
			// The minimum required wait time between coins has passed.
			// Create a withdraw request and render it as a QR code.
			const std::string req = util::createSignedWithdrawRequest(accumulatedValue);
			// Convert to uppercase because it reduces the complexity of the QR code.
			screen::showTransactionCompleteScreen(accumulatedValue, config::getConfig().fiatCurrency, "LIGHTNING:" + util::toUpperCase(req));
			coinAcceptor::reset();
		}
		if (!screen::hasRenderedQRCode() && screen::getRenderedAmount() != accumulatedValue) {
			screen::updateInsertFiatScreenAmount(accumulatedValue, config::getConfig().fiatCurrency);
		}
	}
}
