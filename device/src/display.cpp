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

#include "display.h"

namespace {

	TFT_eSPI tft = TFT_eSPI();
	const uint8_t MARGIN_X = 4;
	const uint8_t MARGIN_Y = 12;
	const uint8_t TEXT_MULTIPLIER = 1;
	const uint8_t TEXT_FONT = 2;
	int BG_COLOR;
	int TEXT_COLOR;
	unsigned long LAST_RENDERED_QRCODE_TIME = 0;
	float RENDERED_AMOUNT = 0.00;

	uint8_t calculateAmountTextHeight() {
		return tft.fontHeight() * TEXT_MULTIPLIER;
	}

	std::string toUpperCase(std::string s) {
		std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c){ return std::toupper(c); });
		return s;
	}

	int calculateQRCodeSize(const std::string &dataStr) {
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

	float calculateQRCodeScale(const uint8_t &size) {
		const uint8_t maxWidth = tft.width() - (MARGIN_X * 2);
		const uint8_t maxHeight = tft.height() - (calculateAmountTextHeight() + MARGIN_Y);
		return std::fmin(
			std::floor(maxWidth / size),
			std::floor(maxHeight / size)
		);
	}

	// http://www.barth-dev.de/about-rgb565-and-how-to-convert-into-it/
	uint16_t hexToRGB565(const std::string& hexColor) {
		uint16_t Rgb565 = 0;
		unsigned int x;
		std::stringstream ss;
		ss << std::hex << hexColor;
		ss >> x;
		uint8_t red = (x >> 16) & 255;
		uint8_t green = (x >> 8) & 255;
		uint8_t blue = x & 255;
		Rgb565 = (((red & 0xf8)<<8) + ((green & 0xfc)<<3) + (blue>>3));
		return Rgb565;
	}

	int getPrecision(const std::string &fiatCurrency) {
		if (fiatCurrency == "EUR") {
			return 2;
		}
		return 0;
	}
}

namespace display {

	void init() {
		tft.begin();
		BG_COLOR = hexToRGB565(BG_COLOR_HEX);
		TEXT_COLOR = hexToRGB565(TEXT_COLOR_HEX);
		tft.fillScreen(BG_COLOR);
		tft.setTextFont(TEXT_FONT);
	}

	void updateAmount(const float &amount, const std::string &fiatCurrency) {
		clearAmount();
		RENDERED_AMOUNT = amount;
		int precision = getPrecision(fiatCurrency);
		std::ostringstream stream;
		stream << std::fixed << std::setprecision(precision) << amount << " " << fiatCurrency;
		const std::string str = stream.str();
		logger::write("Update amount: " + str);
		const char* text = str.c_str();
		tft.setTextSize(TEXT_MULTIPLIER);
		tft.setTextColor(TEXT_COLOR);
		const uint8_t textWidth = tft.textWidth(text);
		tft.setCursor((tft.width() - textWidth) / 2, MARGIN_Y);
		tft.println(text);
	}

	void clearAmount() {
		// Clear previous text by drawing a white rectangle over it.
		tft.fillRect(0, MARGIN_Y, tft.width(), calculateAmountTextHeight(), BG_COLOR);
	}

	float getRenderedAmount() {
		return RENDERED_AMOUNT;
	}

	void renderQRCode(const std::string &dataStr) {
		clearQRCode();
		logger::write("Render QR code: " + dataStr);
		const char* data = toUpperCase(dataStr).c_str();
		const int size = calculateQRCodeSize(dataStr);
		QRCode qrcode;
		uint8_t qrcodeData[qrcode_getBufferSize(size)];
		qrcode_initText(&qrcode, qrcodeData, size, ECC_LOW, data);
		const float scale = calculateQRCodeScale(qrcode.size);
		uint8_t width = qrcode.size * scale;
		uint8_t offsetX = (tft.width() - width) / 2;
		uint8_t offsetY = tft.height() - (width + offsetX);
		for (uint8_t y = 0; y < qrcode.size; y++) {
			for (uint8_t x = 0; x < qrcode.size; x++) {
				int color = qrcode_getModule(&qrcode, x, y) ? TEXT_COLOR : BG_COLOR;
				tft.fillRect(offsetX + scale*x, offsetY + scale*y, scale, scale, color);
			}
		}
		LAST_RENDERED_QRCODE_TIME = millis();
	}

	void clearQRCode() {
		logger::write("Clear QR code");
		const uint8_t offsetY = calculateAmountTextHeight() + MARGIN_Y;
		tft.fillRect(0, offsetY, tft.width(), tft.height() - offsetY, BG_COLOR);
		LAST_RENDERED_QRCODE_TIME = 0;
	}

	bool hasRenderedQRCode() {
		return LAST_RENDERED_QRCODE_TIME > 0;
	}

	unsigned long getTimeSinceRenderedQRCode() {
		return LAST_RENDERED_QRCODE_TIME > 0 ? millis() - LAST_RENDERED_QRCODE_TIME : 0;
	}
}
