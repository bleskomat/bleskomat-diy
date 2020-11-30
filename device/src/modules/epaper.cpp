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

#include "modules/epaper.h"

// Using GxEPD2 library:
//   https://github.com/ZinggJM/GxEPD2
// Which builds on Adafruit-GFX-Library:
//   https://github.com/adafruit/Adafruit-GFX-Library

namespace {

	bool initialized = false;
	GxEPD2_BW<GxEPD2_420, GxEPD2_420::HEIGHT> display(GxEPD2_420(EPAPER_CS, EPAPER_DC, EPAPER_RST, EPAPER_BUSY));
	unsigned long lastRenderedQRCodeTime = 0;
	float renderedAmount = 0.00;
	int16_t renderedAmount_box_x = 0;
	int16_t renderedAmount_box_y = 0;
	uint16_t renderedAmount_tbw = 0;
	uint16_t renderedAmount_tbh = 0;
	int16_t insertFiatScreenAmountMarginTop = -32;
	int16_t transactionCompleteScreenMarginTop = 24;
	int16_t transactionCompleteScreenMarginBottom = 64;
	int16_t transactionCompleteScreenQRCodePadding = 12;

	const int backgroundColor = GxEPD_WHITE;
	const int textColor = GxEPD_BLACK;

	int getBestFitQRCodeVersion(const std::string &dataStr) {
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

	int getPrecision(const std::string &fiatCurrency) {
		if (fiatCurrency == "EUR") {
			return 2;
		}
		return 0;
	}

	std::string getAmountFiatCurrencyString(const float &amount, const std::string &fiatCurrency) {
		int precision = getPrecision(fiatCurrency);
		std::ostringstream stream;
		stream << std::fixed << std::setprecision(precision) << amount << " " << fiatCurrency;
		return stream.str();
	}

	void clearRenderedAmount() {
		if (renderedAmount_tbw > 0 && renderedAmount_tbh > 0) {
			display.setPartialWindow(renderedAmount_box_x, renderedAmount_box_y, renderedAmount_tbw, renderedAmount_tbh);
			display.firstPage();
			do {
				display.fillScreen(backgroundColor);
			} while (display.nextPage());
		}
		renderedAmount = 0;
		renderedAmount_box_x = 0;
		renderedAmount_box_y = 0;
		renderedAmount_tbw = 0;
		renderedAmount_tbh = 0;
	}

	void updateTransactionCompleteScreenAmount(const float &amount, const std::string &fiatCurrency) {
		// Render amount + fiat currency symbol (top-center).
		const char* text = getAmountFiatCurrencyString(amount, fiatCurrency).c_str();
		int16_t tbx, tby;
		uint16_t tbw, tbh;
		display.setFont(&FreeMonoBold18pt7b);
		display.setTextColor(textColor);
		display.getTextBounds(text, 0, 0, &tbx, &tby, &tbw, &tbh);
		int16_t box_x = ((display.width() - tbw) / 2);
		int16_t box_y = transactionCompleteScreenMarginTop;
		clearRenderedAmount();
		display.setPartialWindow(box_x, box_y, tbw, tbh);
		display.firstPage();
		do {
			display.fillScreen(backgroundColor);
			display.setCursor(box_x, box_y);
			display.print(text);
		} while (display.nextPage());
		renderedAmount = amount;
		renderedAmount_box_x = box_x;
		renderedAmount_box_y = box_y;
		renderedAmount_tbw = tbw;
		renderedAmount_tbh = tbh;
	}

	void showTransactionScreenQRCode(const std::string &t_data) {
		const char* data = t_data.c_str();
		const int version = getBestFitQRCodeVersion(t_data);
		QRCode qrcode;
		uint8_t qrcodeData[qrcode_getBufferSize(version)];
		qrcode_initText(&qrcode, qrcodeData, version, ECC_LOW, data);
		int padding = transactionCompleteScreenQRCodePadding;
		int16_t max_tbh = display.height() - ((padding*2) + renderedAmount_box_y + renderedAmount_tbh + transactionCompleteScreenMarginBottom);
		int16_t tbw = 0;
		int16_t tbh = 0;
		int scale = std::floor(max_tbh / qrcode.size);
		tbw = qrcode.size * scale;
		tbh = qrcode.size * scale;
		uint8_t box_x = (display.width() - tbw) / 2;
		uint8_t box_y = ((display.height() - tbh) / 2);
		display.setPartialWindow(box_x, box_y, tbw, tbh);
		display.firstPage();
		do {
			display.fillScreen(backgroundColor);
			for (uint8_t y = 0; y < qrcode.size; y++) {
				for (uint8_t x = 0; x < qrcode.size; x++) {
					int color = qrcode_getModule(&qrcode, x, y) ? textColor: backgroundColor;
					display.fillRect(box_x + scale*x, box_y + scale*y, scale, scale, color);
				}
			}
		} while (display.nextPage());
		lastRenderedQRCodeTime = millis();
	}
}

namespace epaper {

	void init() {
		display.init(0);
		SPI.end();// Release standard SPI pins.
		SPI.begin(EPAPER_CLK, EPAPER_MISO, EPAPER_DIN, EPAPER_CS);
		if (display.epd2.panel == GxEPD2::GDEW042T2) {
			logger::write("E-Paper display initialized and ready for use");
			initialized = true;
			display.setRotation(0);
		} else {
			logger::write("Unknown display connected. This device supports WaveShare 4.2 inch e-paper b/w");
		}
	}

	bool isInitialized() {
		return initialized;
	}

	void showSplashScreen() {
		if (!isInitialized()) return;
		display.clearScreen();
		display.drawImage(BLESKOMAT_SPLASH_SCREEN_400x300, 0, 0, display.epd2.WIDTH, display.epd2.HEIGHT, false, false, true);
	}

	void showInstructionsScreen() {
		if (!isInitialized()) return;
		display.clearScreen();
		display.drawImage(BLESKOMAT_INSTRUCTIONS_SCREEN_400x300, 0, 0, display.epd2.WIDTH, display.epd2.HEIGHT, false, false, true);
	}

	void showInsertFiatScreen(const std::string &fiatCurrency) {
		if (!isInitialized()) return;
		display.clearScreen();
		display.drawImage(BLESKOMAT_INSERT_FIAT_SCREEN_400x300, 0, 0, display.epd2.WIDTH, display.epd2.HEIGHT, false, false, true);
		// Render zero amount.
		updateInsertFiatScreenAmount(0, fiatCurrency);
	}

	void updateInsertFiatScreenAmount(const float &amount, const std::string &fiatCurrency) {
		if (!isInitialized()) return;
		// Render amount + fiat currency symbol (top-center).
		const char* text = getAmountFiatCurrencyString(amount, fiatCurrency).c_str();
		int16_t tbx, tby;
		uint16_t tbw, tbh;
		display.setFont(&FreeMonoBold24pt7b);
		display.setTextColor(textColor);
		display.getTextBounds(text, 0, 0, &tbx, &tby, &tbw, &tbh);
		int16_t box_x = ((display.width() - tbw) / 2);
		int16_t box_y = ((display.height() - tbh) / 2) + insertFiatScreenAmountMarginTop;
		clearRenderedAmount();
		int16_t cursor_x = box_x;
		int16_t cursor_y = box_y;
		cursor_y += tbh;// Cursor y-coordinate is the text baseline.
		display.setPartialWindow(box_x, box_y, display.width(), tbh * 1.3);
		display.firstPage();
		do {
			display.fillScreen(backgroundColor);
			display.setCursor(cursor_x, cursor_y);
			display.print(text);
		} while (display.nextPage());
		renderedAmount = amount;
		renderedAmount_box_x = box_x;
		renderedAmount_box_y = box_y;
		renderedAmount_tbw = tbw;
		renderedAmount_tbh = tbh;
	}

	void showTransactionCompleteScreen(const float &amount, const std::string &fiatCurrency, const std::string &qrcodeData) {
		if (!isInitialized()) return;
		display.clearScreen();
		display.drawImage(BLESKOMAT_TRANSACTION_COMPLETE_SCREEN_400x300, 0, 0, display.epd2.WIDTH, display.epd2.HEIGHT, false, false, true);
		updateTransactionCompleteScreenAmount(amount, fiatCurrency);
		showTransactionScreenQRCode(qrcodeData);
	}

	float getRenderedAmount() {
		return renderedAmount;
	}

	bool hasRenderedQRCode() {
		return lastRenderedQRCodeTime > 0;
	}

	unsigned long getTimeSinceRenderedQRCode() {
		return lastRenderedQRCodeTime > 0 ? millis() - lastRenderedQRCodeTime : 0;
	}
}
