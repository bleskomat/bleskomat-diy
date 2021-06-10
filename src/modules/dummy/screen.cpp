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

#include "modules/dummy/screen.h"

// Dummy implementation of screen module.
// This module does nothing.

namespace {
	std::string currentScreen = "";
}

namespace screen {

	void init() {
		logger::write("Dummy screen module initialized");
	}

	std::string getCurrentScreen() {
		return currentScreen;
	}

	void showInsertFiatScreen(const float &amount) {
		currentScreen = "insertFiat";
	}

	void showTransactionCompleteScreen(const float &amount, const std::string &qrcodeData) {
		currentScreen = "transactionComplete";
	}
}
