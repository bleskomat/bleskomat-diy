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

	struct TextBoundingBox {
		int16_t x = 0;
		int16_t y = 0;
		uint16_t w = 0;
		uint16_t h = 0;
	};

	bool initialized = false;
	GxEPD2_BW<GxEPD2_420, GxEPD2_420::HEIGHT> display(GxEPD2_420(EPAPER_CS, EPAPER_DC, EPAPER_RST, EPAPER_BUSY));

	TextBoundingBox renderedAmountTextBoundingBox;

	int16_t insertFiatScreenAmountMarginTop = -32;

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

	// !! TODO !!
	// Move to a configuration in bleskomat.conf
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

	void clearPartialArea(const int16_t &x, const int16_t &y, const uint16_t &w, const uint16_t &h) {
		display.setPartialWindow(x, y, w, h);
		display.firstPage();
		do {
			display.fillScreen(backgroundColor);
		} while (display.nextPage());
	}

	void clearText(const TextBoundingBox &box) {
		clearPartialArea(box.x, box.y, box.w, box.h);
	}

	void renderText(const std::string &t_text, const GFXfont *f, const int16_t &x, const int16_t &y, TextBoundingBox *textBoundingBox) {
		const char* text = t_text.c_str();
		display.setFont(f);
		display.setTextColor(textColor);
		int16_t tbx, tby;
		uint16_t tbw, tbh;
		display.getTextBounds(text, 0, 0, &tbx, &tby, &tbw, &tbh);
		uint16_t w_adjust = tbw * .15;
		uint16_t h_adjust = tbh * .5;
		tbw += w_adjust;
		tbh += h_adjust;
		int16_t box_x = x - (tbw / 2);
		int16_t box_y = y - (tbh / 2);
		int16_t cursor_x = box_x + (w_adjust / 2);
		int16_t cursor_y = (box_y + tbh) - (h_adjust * .8);// Cursor y-coordinate is the text baseline.
		display.setPartialWindow(box_x, box_y, tbw, tbh);
		display.firstPage();
		do {
			display.fillScreen(backgroundColor);
			// Uncomment the following line to draw the outer bounding box.
			// display.drawRect(box_x, box_y, tbw, tbh, textColor);
			display.setCursor(cursor_x, cursor_y);
			display.print(text);
		} while (display.nextPage());
		if (textBoundingBox != NULL) {
			textBoundingBox->x = box_x;
			textBoundingBox->y = box_y;
			textBoundingBox->w = tbw;
			textBoundingBox->h = tbh;
		}
	}

	void renderQRCode(const std::string &t_data, const int16_t &x, const int16_t &y, const uint16_t &max_w, const uint16_t &max_h) {
		const char* data = t_data.c_str();
		const int version = getBestFitQRCodeVersion(t_data);
		QRCode qrcode;
		uint8_t qrcodeData[qrcode_getBufferSize(version)];
		qrcode_initText(&qrcode, qrcodeData, version, ECC_LOW, data);
		int16_t w = 0;
		int16_t h = 0;
		int scale = std::min(std::floor(max_w / qrcode.size), std::floor(max_h / qrcode.size));
		w = qrcode.size * scale;
		h = qrcode.size * scale;
		int16_t box_x = x - (w / 2);
		int16_t box_y = y - (h / 2);
		display.setPartialWindow(box_x, box_y, w, h);
		display.firstPage();
		do {
			display.fillScreen(backgroundColor);
			// Uncomment the following line to draw the outer bounding box.
			display.drawRect(box_x, box_y, w, h, textColor);
			for (uint8_t y = 0; y < qrcode.size; y++) {
				for (uint8_t x = 0; x < qrcode.size; x++) {
					int color = qrcode_getModule(&qrcode, x, y) ? textColor: backgroundColor;
					display.fillRect(box_x + scale*x, box_y + scale*y, scale, scale, color);
				}
			}
		} while (display.nextPage());
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
			screen::showSplashScreen();
			// !!!
			delay(2000);
			screen::showInstructionsScreen();
			delay(2000);
			screen::showInsertFiatScreen(config::getConfig().fiatCurrency);
			delay(2000);
			screen::showTransactionCompleteScreen(0, config::getConfig().fiatCurrency, "LNURL1DP68GURN8GHJ7UM9WFMXJCM99E3K7MF0V9CXJ0M385EKVCENXC6R2C35XVUKXEFCV5MKVV34X5EKZD3EV56NYD3HXQURZEPEXEJXXEPNXSCRVWFNV9NXZCN9XQ6XYEFHVGCXXCMYXYMNSERXFQ5FNS");
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

		// Render zero amount.
		updateInsertFiatScreenAmount(0, fiatCurrency);

		// Instructional text #1 (center, below amount).
		const std::string text = "insert bills  and/or coins";
		int16_t margin = 16;
		int16_t x = display.width() / 2;// center
		// Render the next text line below the amount text.
		int16_t text_y = renderedAmountTextBoundingBox.y + renderedAmountTextBoundingBox.h + margin;
		TextBoundingBox text_box;
		renderText(text, &OpenSans_Light12pt7b, x, text_y, &text_box);

		// Instructional text #2 (center, below instructional text #1).
		const std::string text2 = "(press button when done)";
		int16_t text2_y = text_box.y + text_box.h + margin;
		renderText(text2, &OpenSans_Light9pt7b, x, text2_y, NULL);
	}

	void updateInsertFiatScreenAmount(const float &amount, const std::string &fiatCurrency) {
		if (!isInitialized()) return;
		if (renderedAmountTextBoundingBox.w > 0) {
			// Clear previously rendered amount.
			clearText(renderedAmountTextBoundingBox);
		}
		// Render amount + fiat currency symbol (top-center).
		const std::string text = getAmountFiatCurrencyString(amount, fiatCurrency);
		int16_t x = (display.width() / 2);
		int16_t y = (display.height() / 2) + insertFiatScreenAmountMarginTop;
		renderText(text, &Courier_Prime_Code32pt7b, x, y, &renderedAmountTextBoundingBox);
	}

	void showTransactionCompleteScreen(const float &amount, const std::string &fiatCurrency, const std::string &qrcodeData) {
		if (!isInitialized()) return;

		display.clearScreen();

		// Render QR code.
		int16_t qr_x = display.width() / 2;
		int16_t qr_y = display.height() / 2;
		uint16_t qr_max_w = display.width();
		uint16_t qr_max_h = 216;
		renderQRCode(qrcodeData, qr_x, qr_y, qr_max_w, qr_max_h);

		// Render amount + fiat currency symbol (top-center).
		const std::string text = getAmountFiatCurrencyString(amount, fiatCurrency);
		int16_t margin = 24;
		int16_t text_x = (display.width() / 2);// center
		int16_t text_y = margin;
		renderText(text, &Courier_Prime_Code12pt7b, text_x, text_y, NULL);

		// Render instructional text (bottom-center).
		const std::string text2 = "scan with your mobile wallet app";
		int16_t text2_x = (display.width() / 2);// center
		int16_t text2_y = display.height() - margin;// bottom + margin
		renderText(text2, &OpenSans_Light9pt7b, text2_x, text2_y, NULL);
	}
}
