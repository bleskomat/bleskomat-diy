#include "screen.h"

namespace {
	std::string currentScreen = "";
}

namespace screen {

	void init() {
		screen_tft::init();
	}

	std::string getCurrentScreen() {
		return currentScreen;
	}

	void showInsertFiatScreen(const float &amount) {
		screen_tft::showInsertFiatScreen(amount);
		currentScreen = "insertFiat";
	}

	void showTradeCompleteScreen(const float &amount, const std::string &qrcodeData) {
		screen_tft::showTradeCompleteScreen(amount, qrcodeData);
		currentScreen = "tradeComplete";
	}
}
