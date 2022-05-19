#include "main.h"

void setup() {
	Serial.begin(MONITOR_SPEED);
	sdcard::init();
	logger::write(firmwareName);
	logger::write("Firmware version = " + firmwareVersion + ", commit hash = " + firmwareCommitHash);
	config::init();
	jsonRpc::init();
	screen::init();
	coinAcceptor::init();
	button::init();
	logger::write("Setup OK");
}

float amountShown = 0;

void runAppLoop() {
	coinAcceptor::loop();
	button::loop();
	const std::string currentScreen = screen::getCurrentScreen();
	if (currentScreen == "") {
		screen::showInsertFiatScreen(0);
	}
	float accumulatedValue = 0;
	accumulatedValue += coinAcceptor::getAccumulatedValue();
	if (
		accumulatedValue > 0 &&
		currentScreen != "insertFiat" &&
		currentScreen != "tradeComplete"
	) {
		screen::showInsertFiatScreen(accumulatedValue);
		amountShown = accumulatedValue;
	}
	if (currentScreen == "insertFiat") {
		if (coinAcceptor::isInhibited()) {
			// The coin acceptor should be enabled while insert fiat screen shown.
			coinAcceptor::disinhibit();
		}
		if (button::isPressed()) {
			if (accumulatedValue > 0) {
				// Button pushed while insert fiat screen shown and accumulated value greater than 0.
				// Create a withdraw request and render it as a QR code.
				const std::string signedUrl = util::createSignedLnurlWithdraw(accumulatedValue);
				const std::string encoded = util::lnurlEncode(signedUrl);
				std::string qrcodeData = "";
				// Allows upper or lower case URI schema prefix via a configuration option.
				// Some wallet apps might not support uppercase URI prefixes.
				qrcodeData += config::getString("uriSchemaPrefix");
				// QR codes with only uppercase letters are less complex (easier to scan).
				qrcodeData += util::toUpperCase(encoded);
				screen::showTradeCompleteScreen(accumulatedValue, qrcodeData);
				coinAcceptor::inhibit();
				delay(config::getUnsignedInt("buttonDelay"));
			}
		} else {
			// Button not pressed.
			// Ensure that the amount shown is correct.
			if (amountShown != accumulatedValue) {
				screen::showInsertFiatScreen(accumulatedValue);
				amountShown = accumulatedValue;
			}
		}
	} else if (currentScreen == "tradeComplete") {
		if (!coinAcceptor::isInhibited()) {
			// Don't allow inserting more coins while trade complete screen shown.
			coinAcceptor::inhibit();
		}
		if (button::isPressed()) {
			// Button pushed while showing the trade complete screen.
			// Reset accumulated values.
			coinAcceptor::resetAccumulatedValue();
			amountShown = 0;
			screen::showInsertFiatScreen(0);
		}
	}
}

void loop() {
	// Un-comment the following to enable extra debugging information:
	debugger::loop();
	jsonRpc::loop();
	if (!jsonRpc::hasPinConflict() || !jsonRpc::inUse()) {
		runAppLoop();
	}
}
