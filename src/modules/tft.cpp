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

#include "modules/tft.h"

namespace {

	TFT_eSPI display = TFT_eSPI();
	const auto bg_color = TFT_WHITE;
	const auto text_color = TFT_BLACK;
	const uint8_t text_font = 2;
	const uint8_t text_size = 1;
	const uint8_t margin_x = 4;
	const uint8_t margin_y = 12;
	std::string current_screen = "";

	struct BoundingBox {
		int16_t x = 0;
		int16_t y = 0;
		uint16_t w = 0;
		uint16_t h = 0;
	};

	BoundingBox amount_text_bbox;

	std::string getAmountFiatCurrencyString(const float &amount) {
		std::ostringstream stream;
		const std::string fiat_currency = config::get("fiatCurrency");
		const unsigned short precision = config::getFiatPrecision();
		stream << std::fixed << std::setprecision((int) precision) << amount << " " << fiat_currency;
		return stream.str();
	}

	BoundingBox renderText(
		const std::string &t_text,
		const int16_t &x,
		const int16_t &y,
		const bool &center = true
	) {
		logger::write("tft.renderText \"" + t_text + "\"");
		const char* text = t_text.c_str();
		display.setTextColor(text_color);
		display.setTextFont(text_font);
		display.setTextSize(text_size);
		int16_t tbw = display.textWidth(text);
		int16_t tbh = display.fontHeight() * text_size;
		int16_t box_x = x;
		int16_t box_y = y;
		if (center) {
			box_x -= (tbw / 2);
		}
		int16_t cursor_x = box_x;
		int16_t cursor_y = box_y;
		display.setCursor(cursor_x, cursor_y);
		display.println(text);
		BoundingBox bbox;
		bbox.x = box_x;
		bbox.y = box_y;
		bbox.w = tbw;
		bbox.h = tbh;
		return bbox;
	}

	int qrcode_sizes[17] = { 25, 47, 77, 114, 154, 195, 224, 279, 335, 395, 468, 535, 619, 667, 758, 854, 938 };
	int getBestFitQRCodeVersion(const std::string &data) {
		int size = 12;
		int len = data.length();
		for (int i = 0; i < 17; i++) {
			if (qrcode_sizes[i] > len) {
				return i + 1;
			}
		}
		return size;
	}

	BoundingBox renderQRCode(
		const std::string &t_data,
		const int16_t &x,
		const int16_t &y,
		const uint16_t &max_w,
		const uint16_t &max_h,
		const bool &center = true
	) {
		logger::write("tft.renderQRCode \"" + t_data + "\"");
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
		int16_t box_x = x;
		int16_t box_y = y;
		if (center) {
			box_x -= (w / 2);
		}
		display.fillRect(box_x, box_y, w, h, bg_color);
		for (uint8_t y = 0; y < qrcode.size; y++) {
			for (uint8_t x = 0; x < qrcode.size; x++) {
				int color = qrcode_getModule(&qrcode, x, y) ? text_color: bg_color;
				display.fillRect(box_x + scale*x, box_y + scale*y, scale, scale, color);
			}
		}
		BoundingBox bbox;
		bbox.x = box_x;
		bbox.y = box_y;
		bbox.w = w;
		bbox.h = h;
		return bbox;
	}

	void clearScreen() {
		logger::write("tft.clearScreen");
		display.fillScreen(bg_color);
	}
}

namespace tft {

	void init() {
		logger::write("tft.init");
		display.begin();
		clearScreen();
	}

	std::string getCurrentScreen() {
		return current_screen;
	}

	void showInsertFiatScreen(const float &amount) {
		logger::write("tft.showInsertFiatScreen");
		if (current_screen == "insertFiat") {
			// Clear previous text by drawing a rectangle over it.
			display.fillRect(
				amount_text_bbox.x,
				amount_text_bbox.y,
				amount_text_bbox.w,
				amount_text_bbox.h,
				bg_color
			);
		} else if (current_screen == "tradeComplete") {
			// Clear the whole screen.
			clearScreen();
		}
		const std::string text = getAmountFiatCurrencyString(amount);
		const int16_t center_x = display.width() / 2;
		const int16_t text_x = center_x;
		const int16_t text_y = margin_y;
		amount_text_bbox = renderText(text, text_x, text_y, true/* center */);
		current_screen = "insertFiat";
	}

	void showTradeCompleteScreen(const float &amount, const std::string &qrcodeData) {
		logger::write("tft.showTradeCompleteScreen");
		clearScreen();
		const std::string text = getAmountFiatCurrencyString(amount);
		const int16_t center_x = display.width() / 2;
		const int16_t text_x = center_x;
		const int16_t text_y = margin_y;
		amount_text_bbox = renderText(text, text_x, text_y, true/* center */);
		const int16_t qr_x = center_x;
		const int16_t qr_y = amount_text_bbox.y + amount_text_bbox.h + margin_y;
		const int16_t qr_max_w = display.width();
		const int16_t qr_max_h = display.height() - (qr_y + margin_y);
		renderQRCode(qrcodeData, qr_x, qr_y, qr_max_w, qr_max_h, true/* center */);
		current_screen = "tradeComplete";
	}
}
