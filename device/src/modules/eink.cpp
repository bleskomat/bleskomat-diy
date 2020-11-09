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

#include "modules/eink.h"

namespace eink {


#if defined(ESP32)
// mapping of Waveshare ESP32 Driver Board
// BUSY -> 25, RST -> 26, DC -> 27, CS-> 15, CLK -> 13, DIN -> 14

/* ******************** */
/* Bleskomat displays */
#ifdef EINK_200x200
/* - for the 1.54 inch display use */
	GxEPD2_BW<GxEPD2_154, GxEPD2_154::HEIGHT> display(GxEPD2_154(/*CS=*/ 15, /*DC=*/ 27, /*RST=*/ 26, /*BUSY=*/ 25)); // GDEP015OC1 no longer available
#else // if EINK_128x296
/* - for the 2.9 inch display use */
	GxEPD2_BW<GxEPD2_290, GxEPD2_290::HEIGHT> display(GxEPD2_290(/*CS=*/ 15, /*DC=*/ 27, /*RST=*/ 26, /*BUSY=*/ 25));
#endif

#endif

	unsigned long LAST_RENDERED_QRCODE_TIME = 0;
	float RENDERED_AMOUNT = 0.00;

	const int BG_COLOR = GxEPD_WHITE;
	const int TEXT_COLOR = GxEPD_BLACK;


	void init()
	{
		// Serial.begin(115200);
		Serial.println();
		Serial.println("setup");
		display.init(115200); // uses standard SPI pins, e.g. SCK(18), MISO(19), MOSI(23), SS(5)
		// *** special handling for Waveshare ESP32 Driver board *** //
		// ********************************************************* //
		SPI.end(); // release standard SPI pins, e.g. SCK(18), MISO(19), MOSI(23), SS(5)
		//SPI: void begin(int8_t sck=-1, int8_t miso=-1, int8_t mosi=-1, int8_t ss=-1);
		SPI.begin(13, 12, 14, 15); // map and init SPI pins SCK(13), MISO(12), MOSI(14), SS(15)
		// *** end of special handling for Waveshare ESP32 Driver board *** //
		// **************************************************************** //
	}

	void splashScreen()
	{
		display.setRotation(0);
		drawLogo();
		display.setFont(&FreeMonoBold12pt7b);
		display.setRotation(1);
	}


	void drawLogo()
	{
		Serial.println("Drawing Bleskomat logo of the required size.");
#ifdef EINK_200x200
#define bleskomat_logo bleskomat_200x200
#else// if EINK_128x296
#define bleskomat_logo bleskomat_128x296
#endif

#if !defined(__AVR)
		const unsigned char* bitmaps[] =
			{
				bleskomat_logo
			};
#else
		const unsigned char* bitmaps[] =
			{
				bleskomat_logo
			};
#endif
#undef bleskomat_logo


#ifdef EINK_200x200
		if ((display.epd2.panel == GxEPD2::GDEP015OC1) || (display.epd2.panel == GxEPD2::GDEH0154D67))
#else// if EINK_128x296
			if (display.epd2.panel == GxEPD2::GDEH029A1)
#endif
			{
				bool m = display.mirror(true);
				for (uint16_t i = 0; i < sizeof(bitmaps) / sizeof(char*); i++)
				{
					display.firstPage();
					do
					{
						display.fillScreen(GxEPD_WHITE);
						display.drawInvertedBitmap(0, 0, bitmaps[i], display.epd2.WIDTH, display.epd2.HEIGHT, GxEPD_BLACK);
					}
					while (display.nextPage());
					delay(2000);
				}
				display.mirror(m);
			}
}

	void updateAmount(const float &amount, const std::string &fiatCurrency)
	{

		uint16_t box_x = 10;
		uint16_t box_y = 15;
		uint16_t box_w = 120;
		uint16_t box_h = 20;
		uint16_t cursor_y = box_y + box_h - 6;

		display.setPartialWindow(box_x, box_y, box_w, box_h);

		// clearAmount();
		RENDERED_AMOUNT = amount;
		display.setFont(&FreeMonoBold9pt7b);
		int16_t fontHeight = 9;

		display.setTextColor(GxEPD_BLACK);
		int16_t tbx, tby; uint16_t tbw, tbh;

		int precision = display::getPrecision(fiatCurrency);
		std::ostringstream stream;
		stream << std::fixed << std::setprecision(precision) << amount << " " << fiatCurrency;
		const std::string str = stream.str();
		logger::write("Update amount: " + str);
		const char* text = str.c_str();

		display.getTextBounds(text, 0, 0, &tbx, &tby, &tbw, &tbh);

		// center bounding box by transposition of origin:
		uint16_t x = ((display.width() - tbw) / 2) - tbx;
		// uint16_t y = ((display.height() - tbh) / 2) - tby
		uint16_t y = fontHeight + 2;

		do
		{
			display.fillRect(box_x, box_y, box_w, box_h, GxEPD_WHITE);
			display.setCursor(box_x, cursor_y);
			display.print((String) text);
		}
		while (display.nextPage());
	}

	void clearAmount(){}

	float getRenderedAmount() {
		return RENDERED_AMOUNT;
	}

	void renderQRCode(const std::string &dataStr) {
		clearQRCode();

		logger::write("Render QR code: " + dataStr);
		const char* data = display::toUpperCase(dataStr).c_str();
		const int size = display::calculateQRCodeSize(dataStr);
		QRCode qrcode;
		uint8_t qrcodeData[qrcode_getBufferSize(size)];
		qrcode_initText(&qrcode, qrcodeData, size, ECC_LOW, data);

		const float scale = 2;
		// const float scale = calculateQRCodeScale(qrcode.size);
		uint8_t width = qrcode.size * scale;
		// uint8_t offsetX = (display.width() - width) / 2;
		// uint8_t offsetY = display.height() - (width + offsetX);

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
		display.setTextColor(GxEPD_BLACK);
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
		do
		{
			display.fillScreen(GxEPD_WHITE);
			display.setCursor(x, y);
			display.print(text);
		}
		while (display.nextPage());
		Serial.println("helloWorld done");
	}

	bool hasRenderedQRCode() {
		return LAST_RENDERED_QRCODE_TIME > 0;
	}

	unsigned long getTimeSinceRenderedQRCode() {
		return LAST_RENDERED_QRCODE_TIME > 0 ? millis() - LAST_RENDERED_QRCODE_TIME : 0;
	}
}



