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

#include "modules/oled.h"
#include "display.h"

namespace oled {
	TFT_eSPI tft = TFT_eSPI();
	uint16_t BG_COLOR;
	uint16_t TEXT_COLOR;
	const uint8_t TEXT_MULTIPLIER = 1;
	const uint8_t TEXT_FONT = 2;
	const uint8_t MARGIN_X = 4;
	const uint8_t MARGIN_Y = 12;

	unsigned long LAST_RENDERED_QRCODE_TIME = 0;
	float RENDERED_AMOUNT = 0.00;

	void init() {
		tft.begin();
		BG_COLOR = display::hexToRGB565(BG_COLOR_HEX);
		TEXT_COLOR = display::hexToRGB565(TEXT_COLOR_HEX);
		tft.fillScreen(BG_COLOR);
		tft.setTextFont(TEXT_FONT);
	}

	void splashScreen(){
		tft.println("\tBLESKOMAT");
		delay(2000);
	}

	void resetScreen(){	
		Serial.println("screen reset");
		tft.fillScreen(BG_COLOR);
		tft.println("\n\n\tinsert coin");
	}


	void updateAmount(const float &amount, const std::string &fiatCurrency) {
		clearAmount();
		oled::RENDERED_AMOUNT = amount;
		int precision = display::getPrecision(fiatCurrency);
		std::ostringstream stream;
		stream << std::fixed << std::setprecision(precision) << amount << " " << fiatCurrency;
		const std::string str = stream.str();
		logger::write("Update amount: " + str);
		const char* text = str.c_str();
		tft.setTextSize(oled::TEXT_MULTIPLIER);
		tft.setTextColor(TEXT_COLOR);
		const uint8_t textWidth = tft.textWidth(text);
		tft.setCursor((tft.width() - textWidth) / 2, oled::MARGIN_Y);
		tft.println(text);
	}

	void clearAmount() {
		// Clear previous text by drawing a white rectangle over it.
		tft.fillRect(0, oled::MARGIN_Y, tft.width(), display::calculateAmountTextHeight(tft.fontHeight(),  oled::TEXT_MULTIPLIER), BG_COLOR);
	}

	float getRenderedAmount() {
		return oled::RENDERED_AMOUNT;
	}

	void renderQRCode(const std::string &dataStr) {
		clearQRCode();
		logger::write("Render QR code: " + dataStr);
		const char* data = display::toUpperCase(dataStr).c_str();
		const int size = display::calculateQRCodeSize(dataStr);
		QRCode qrcode;
		uint8_t qrcodeData[qrcode_getBufferSize(size)];
		qrcode_initText(&qrcode, qrcodeData, size, ECC_LOW, data);
		const float scale = display::calculateQRCodeScale(qrcode.size, tft.width(), tft.height(), tft.fontHeight(), oled::MARGIN_X, oled::MARGIN_Y, oled::TEXT_MULTIPLIER);
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
		const uint8_t offsetY = display::calculateAmountTextHeight(tft.fontHeight(),  oled::TEXT_MULTIPLIER) + oled::MARGIN_Y;
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
