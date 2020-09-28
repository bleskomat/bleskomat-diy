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

#include "config.h"
#include "display.h"
#include "logger.h"
#include "modules.h"
#include "sdcard.h"
#include "util.h"

#include <string>

void setup() {
	Serial.begin(115200);
	logger::enable();

	if ( sdcard::mount() < 0 ) {
	    printf("SD card failed to mount, setting default config.\n");
	    config::setDefault();
	} else {
	    printf("Card mounted, set config from SD card.\n");
		if ( config::setFromFile("/sdcard/bleskomat.conf") < 0 ) {
			printf("Setting from file failed, falling back to default config.\n");
			config::setDefault();
		}
		sdcard::umount();
	}
	logger::write("Config OK");

	display::init();
	display::updateAmount(0.00, config::getFiatCurrency());
	logger::write("Display OK");
	coinAcceptor::init();
	coinAcceptor::setFiatCurrency(config::getFiatCurrency());
	logger::write("Coin Reader OK");
	logger::write("Setup OK");
}

const unsigned long bootTime = millis();// milliseconds
const unsigned long minWaitAfterBootTime = 2000;// milliseconds
const unsigned long minWaitTimeSinceInsertedFiat = 15000;// milliseconds
const unsigned long maxTimeDisplayQrCode = 120000;// milliseconds

void loop() {
  // sdcard::debug();

	if (millis() - bootTime >= minWaitAfterBootTime) {
		// Minimum time has passed since boot.
		// Start performing checks.
		coinAcceptor::loop();
		if (display::getTimeSinceRenderedQRCode() >= maxTimeDisplayQrCode) {
			// Automatically clear the QR code from the screen after some time has passed.
			display::clearQRCode();
		} else if (coinAcceptor::coinInserted() && display::hasRenderedQRCode()) {
			// Clear the QR code when new coins are inserted.
			display::clearQRCode();
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
			display::renderQRCode("LIGHTNING:" + util::toUpperCase(req));
			coinAcceptor::reset();
		}
		if (!display::hasRenderedQRCode() && display::getRenderedAmount() != accumulatedValue) {
		    display::updateAmount(accumulatedValue, config::getFiatCurrency());
		}
	}
}
