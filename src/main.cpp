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
#include "main.h"

void setup() {
	Serial.begin(MONITOR_SPEED);
	sdcard::init();
	logger::write("Bleskomat DIY firmware commit=" + firmwareCommitHash);
	config::init();
	logger::write("Config OK");
	modules::init();
	logger::write("Modules OK");
	logger::write("Setup OK");
	screen::showInsertFiatScreen(0);
}

float amountShown = 0;

void loop() {
	modules::loop();
	const std::string currentScreen = screen::getCurrentScreen();
	float accumulatedValue = 0;
	#ifdef COIN_ACCEPTOR
		accumulatedValue += coinAcceptor::getAccumulatedValue();
	#endif
	if (
		accumulatedValue > 0 &&
		currentScreen != "insertFiat" &&
		currentScreen != "transactionComplete"
	) {
		screen::showInsertFiatScreen(accumulatedValue);
		amountShown = accumulatedValue;
	}
	if (currentScreen == "insertFiat") {
		#ifdef COIN_ACCEPTOR
			if (coinAcceptor::isOff()) {
				// The coin acceptor should be enabled while insert fiat screen shown.
				coinAcceptor::on();
			}
		#endif
		if (button::isPressed()) {
			if (accumulatedValue > 0) {
				// Button pushed while insert fiat screen shown and accumulated value greater than 0.
				// Create a withdraw request and render it as a QR code.
				const std::string signedUrl = util::createSignedLnurlWithdraw(accumulatedValue);
				const std::string encoded = util::lnurlEncode(signedUrl);
				const std::string uriSchemaPrefix = config::get("uriSchemaPrefix");
				std::string qrcodeData = "";
				if (uriSchemaPrefix != "") {
					// Allows upper or lower case URI schema prefix via a configuration option.
					// Some wallet apps might not support uppercase URI prefixes.
					qrcodeData += uriSchemaPrefix + ":";
				}
				// QR codes with only uppercase letters are less complex (easier to scan).
				qrcodeData += util::toUpperCase(encoded);
				screen::showTransactionCompleteScreen(accumulatedValue, qrcodeData);
				#ifdef COIN_ACCEPTOR
					coinAcceptor::off();
				#endif
			}
		} else {
			// Button not pressed.
			// Ensure that the amount shown is correct.
			if (amountShown != accumulatedValue) {
				screen::showInsertFiatScreen(accumulatedValue);
				amountShown = accumulatedValue;
			}
		}
	} else if (currentScreen == "transactionComplete") {
		#ifdef COIN_ACCEPTOR
			if (coinAcceptor::isOn()) {
				// Don't allow inserting more coins while transaction complete screen shown.
				coinAcceptor::off();
			}
		#endif
		if (button::isPressed()) {
			// Button pushed while showing the transaction complete screen.
			// Reset accumulated values.
			#ifdef COIN_ACCEPTOR
				coinAcceptor::reset();
			#endif
			amountShown = 0;
			screen::showInsertFiatScreen(0);
		}
	}
}
