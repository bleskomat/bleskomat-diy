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

	int getPrecision(const std::string &fiatCurrency) {
		if (fiatCurrency == "EUR") {
			return 2;
		}
		return 0;
	}
}

namespace epaper {

	GxEPD2_BW<GxEPD2_420, GxEPD2_420::HEIGHT> display(GxEPD2_420(EPAPER_CS, EPAPER_DC, EPAPER_RST, EPAPER_BUSY));
	unsigned long LAST_RENDERED_QRCODE_TIME = 0;
	float RENDERED_AMOUNT = 0.00;
	const int BG_COLOR = GxEPD_WHITE;
	const int TEXT_COLOR = GxEPD_BLACK;

	void init() {
		display.init(0);
		SPI.end();// Release standard SPI pins, e.g. SCK(18), MISO(19), MOSI(23), SS(5)
		SPI.begin(EPAPER_CLK, EPAPER_MISO, EPAPER_DIN, EPAPER_CS);
	}

	void showSplashPage() {
		if (display.epd2.panel == GxEPD2::GDEW042T2) {
			display.clearScreen();
			display.drawImage(BLESKOMAT_SPLASH_400x300, 0, 0, display.epd2.WIDTH, display.epd2.HEIGHT, false, false, true);
		}
	}

	void updateAmount(const float &amount, const std::string &fiatCurrency) {
		uint16_t box_x = 10;
		uint16_t box_y = 15;
		uint16_t box_w = 120;
		uint16_t box_h = 20;
		uint16_t cursor_y = box_y + box_h - 6;
		display.setPartialWindow(box_x, box_y, box_w, box_h);
		RENDERED_AMOUNT = amount;
		display.setFont(&FreeMonoBold9pt7b);
		display.setTextColor(TEXT_COLOR);
		int16_t tbx, tby; uint16_t tbw, tbh;
		int precision = getPrecision(fiatCurrency);
		std::ostringstream stream;
		stream << std::fixed << std::setprecision(precision) << amount << " " << fiatCurrency;
		const std::string str = stream.str();
		logger::write("Update amount: " + str);
		const char* text = str.c_str();
		display.getTextBounds(text, 0, 0, &tbx, &tby, &tbw, &tbh);
		do {
			display.fillRect(box_x, box_y, box_w, box_h, BG_COLOR);
			display.setCursor(box_x, cursor_y);
			display.print((String) text);
		} while (display.nextPage());
	}

	void clearAmount() {}

	float getRenderedAmount() {
		return RENDERED_AMOUNT;
	}

	void renderQRCode(const std::string &dataStr) {
		clearQRCode();
		logger::write("Render QR code: " + dataStr);
		const char* data = util::toUpperCase(dataStr).c_str();
		const int size = calculateQRCodeSize(dataStr);
		QRCode qrcode;
		uint8_t qrcodeData[qrcode_getBufferSize(size)];
		qrcode_initText(&qrcode, qrcodeData, size, ECC_LOW, data);
		const float scale = 2;
		uint8_t width = qrcode.size * scale;
		// place the qr code in the centre
		uint8_t offsetX = (display.width() - width)/2;
		uint8_t offsetY = (display.height() - width)/2;
		for (uint8_t y = 0; y < qrcode.size; y++) {
			for (uint8_t x = 0; x < qrcode.size; x++) {
				int color = qrcode_getModule(&qrcode, x, y) ?  TEXT_COLOR: BG_COLOR;
				display.fillRect(offsetX + scale*x, offsetY + scale*y, scale, scale, color);
			}
		}
		display.nextPage();
		LAST_RENDERED_QRCODE_TIME = millis();
		logger::write("QR code rendered.");
	}

	void clearQRCode() {
		logger::write("Clear QR code");
		display.setFullWindow();
		display.firstPage();
		LAST_RENDERED_QRCODE_TIME = 0;
	}

	void resetScreen() {
		logger::write("Reseting screen");

		display.setFont(&FreeMonoBold12pt7b);
		display.setTextColor(TEXT_COLOR);
		display.setRotation(1);
		display.setFullWindow();
		display.firstPage();
		int16_t tbx, tby; uint16_t tbw, tbh;
		String text = "BLESKOMAT\n" "  insert coin";
		display.getTextBounds(text, 0, 0, &tbx, &tby, &tbw, &tbh);
		// center bounding box by transposition of origin:
		uint16_t x = ((display.width() - tbw) / 2) - tbx;
		uint16_t y = ((display.height() - tbh) / 2) - tby;
		display.setFullWindow();
		display.firstPage();
		do {
			display.fillScreen(BG_COLOR);
			display.setCursor(x, y);
			display.print(text);
		} while (display.nextPage());
	}

	bool hasRenderedQRCode() {
		return LAST_RENDERED_QRCODE_TIME > 0;
	}

	unsigned long getTimeSinceRenderedQRCode() {
		return LAST_RENDERED_QRCODE_TIME > 0 ? millis() - LAST_RENDERED_QRCODE_TIME : 0;
	}
}



