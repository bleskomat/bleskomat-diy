#include "eink.h"

namespace {

	// GxEPD2_BW<GxEPD2_154, GxEPD2_154::HEIGHT> display(GxEPD2_154(/*CS=*/ 15, /*DC=*/ 27, /*RST=*/ 26, /*BUSY=*/ 25)); // GDEP015OC1 no longer available

	GxEPD2_BW<GxEPD2_290, GxEPD2_290::HEIGHT> display(GxEPD2_290(/*CS=*/ 15, /*DC=*/ 27, /*RST=*/ 26, /*BUSY=*/ 25));

	const uint8_t MARGIN_X = 4;
	const uint8_t MARGIN_Y = 12;

	unsigned long LAST_RENDERED_QRCODE_TIME = 0;
	float RENDERED_AMOUNT = 0.00;

	const int BG_COLOR = GxEPD_WHITE;
	const int TEXT_COLOR = GxEPD_BLACK;

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
		const uint8_t maxWidth = display.width() - (MARGIN_X * 2);
		// const uint8_t maxHeight = tft.height() - (calculateAmountTextHeight() + MARGIN_Y);
		const uint8_t maxHeight = display.height() - (MARGIN_Y);

		return std::fmin(
			std::floor(maxWidth / size),
			std::floor(maxHeight / size)
		);
	}

	int getPrecision(const std::string &fiatCurrency) {
		if (fiatCurrency == "EUR") {
			return 2;
		}
		return 0;
	}
}

namespace eink {


#if defined(ESP32)
// mapping of Waveshare ESP32 Driver Board
// BUSY -> 25, RST -> 26, DC -> 27, CS-> 15, CLK -> 13, DIN -> 14

/* ******************** */
/* Bleskomat displays */
/* - for the 2.9 inch display use */
	GxEPD2_BW<GxEPD2_290, GxEPD2_290::HEIGHT> display(GxEPD2_290(/*CS=*/ 15, /*DC=*/ 27, /*RST=*/ 26, /*BUSY=*/ 25));
/* - for the 1.54 inch display use */
	// GxEPD2_BW<GxEPD2_154, GxEPD2_154::HEIGHT> display(GxEPD2_154(/*CS=*/ 15, /*DC=*/ 27, /*RST=*/ 26, /*BUSY=*/ 25)); // GDEP015OC1 no longer available
#endif

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

		// display.setRotation(0);
		display.setFont(&FreeMonoBold12pt7b);
		display.setTextColor(GxEPD_BLACK);
		display.setRotation(0);
		display.setFullWindow();
		display.firstPage();

		display.powerOff();
		// deepSleepTest();
		Serial.println("setup done");

	}

    // note for partial update window and setPartialWindow() method:
    // partial update window size and position is on byte boundary in physical x direction
    // the size is increased in setPartialWindow() if x or w are not multiple of 8 for even rotation, y or h for odd rotation
    // see also comment in GxEPD2_BW.h, GxEPD2_3C.h or GxEPD2_GFX.h for method setPartialWindow()

	void splashScreen()
	{


#ifdef _GxBitmaps200x200_H_
		Serial.println("_GxBitmaps200x200_H_");
		drawBitmaps200x200();
#endif

#ifdef _GxBitmaps128x296_H_
		Serial.println("_GxBitmaps128x296_H_");
		drawBitmaps128x296();
#endif

		display.setFont(&FreeMonoBold12pt7b);
		display.setRotation(1);

		// int16_t tbx, tby; uint16_t tbw, tbh;
		
		// String text = "BLESKOMAT\n" "  insert coin";

		// display.getTextBounds(text, 0, 0, &tbx, &tby, &tbw, &tbh);
		// // center bounding box by transposition of origin:
		// uint16_t x = ((display.width() - tbw) / 2) - tbx;
		// uint16_t y = ((display.height() - tbh) / 2) - tby;
		// display.setFullWindow();
		// display.firstPage();
		// do
		// {
		// 	display.fillScreen(GxEPD_WHITE);
		// 	display.setCursor(x, y);
		// 	display.print(text);
		// }
		// while (display.nextPage());
		Serial.println("helloWorld done");
	}


#ifdef _GxBitmaps200x200_H_
void drawBitmaps200x200()
{
#if defined(__AVR)
  const unsigned char* bitmaps[] =
  {
    logo200x200, first200x200 //, second200x200, third200x200, fourth200x200, fifth200x200, sixth200x200, senventh200x200, eighth200x200
  };
#elif defined(_BOARD_GENERIC_STM32F103C_H_)
  const unsigned char* bitmaps[] =
  {
    logo200x200, first200x200, second200x200, third200x200, fourth200x200, fifth200x200 //, sixth200x200, senventh200x200, eighth200x200
  };
#else
  const unsigned char* bitmaps[] =
  {
    logo200x200, first200x200, second200x200, third200x200, fourth200x200, fifth200x200, sixth200x200, senventh200x200, eighth200x200
  };
#endif
  if ((display.epd2.panel == GxEPD2::GDEP015OC1) || (display.epd2.panel == GxEPD2::GDEH0154D67))
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
  //else
  {
    bool mirror_y = (display.epd2.panel != GxEPD2::GDE0213B1);
    display.clearScreen(); // use default for white
    int16_t x = (int16_t(display.epd2.WIDTH) - 200) / 2;
    int16_t y = (int16_t(display.epd2.HEIGHT) - 200) / 2;
    for (uint16_t i = 0; i < sizeof(bitmaps) / sizeof(char*); i++)
    {
      display.drawImage(bitmaps[i], x, y, 200, 200, false, mirror_y, true);
      delay(2000);
    }
  }
  bool mirror_y = (display.epd2.panel != GxEPD2::GDE0213B1);
  for (uint16_t i = 0; i < sizeof(bitmaps) / sizeof(char*); i++)
  {
    int16_t x = -60;
    int16_t y = -60;
    for (uint16_t j = 0; j < 10; j++)
    {
      display.writeScreenBuffer(); // use default for white
      display.writeImage(bitmaps[i], x, y, 200, 200, false, mirror_y, true);
      display.refresh(true);
      if (display.epd2.hasFastPartialUpdate)
      {
        // for differential update: set previous buffer equal to current buffer in controller
        display.epd2.writeScreenBufferAgain(); // use default for white
        display.epd2.writeImageAgain(bitmaps[i], x, y, 200, 200, false, mirror_y, true);
      }
      delay(2000);
      x += 40;
      y += 40;
      if ((x >= int16_t(display.epd2.WIDTH)) || (y >= int16_t(display.epd2.HEIGHT))) break;
    }
    if (!display.epd2.hasFastPartialUpdate) break; // comment out for full show
    break; // comment out for full show
  }
  display.writeScreenBuffer(); // use default for white
  display.writeImage(bitmaps[0], int16_t(0), 0, 200, 200, false, mirror_y, true);
  display.writeImage(bitmaps[0], int16_t(int16_t(display.epd2.WIDTH) - 200), int16_t(display.epd2.HEIGHT) - 200, 200, 200, false, mirror_y, true);
  display.refresh(true);
  delay(2000);
}
#endif

#ifdef _GxBitmaps128x296_H_
void drawBitmaps128x296()
{
	Serial.println("Drawing Bleskomat logo");
#if !defined(__AVR)
  const unsigned char* bitmaps[] =
  {
	  bleskomat_128x296 // , pplogo_128x296, Bitmap128x296_1, logo128x296, first128x296, second128x296, third128x296// 
  };
#else
  const unsigned char* bitmaps[] =
  {
	  bleskomat_128x296// ,    pplogo_128x296,	  Bitmap128x296_1, logo128x296 //, first128x296, second128x296, third128x296// 
  };
#endif
  if (display.epd2.panel == GxEPD2::GDEH029A1)
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
#endif

	void updateAmount(const float &amount, const std::string &fiatCurrency)
	{

		uint16_t box_x = 10;
		uint16_t box_y = 15;
		uint16_t box_w = 70;
		uint16_t box_h = 20;
		uint16_t cursor_y = box_y + box_h - 6;

		display.setPartialWindow(box_x, box_y, box_w, box_h);

		// clearAmount();
		RENDERED_AMOUNT = amount;
		display.setFont(&FreeMonoBold9pt7b);
		int16_t fontHeight = 9;

		display.setTextColor(GxEPD_BLACK);
		int16_t tbx, tby; uint16_t tbw, tbh;

		int precision = getPrecision(fiatCurrency);
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

		// display.firstPage();
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
		const char* data = toUpperCase(dataStr).c_str();
		const int size = calculateQRCodeSize(dataStr);
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

	bool hasRenderedQRCode() {
		return LAST_RENDERED_QRCODE_TIME > 0;
	}

	unsigned long getTimeSinceRenderedQRCode() {
		return LAST_RENDERED_QRCODE_TIME > 0 ? millis() - LAST_RENDERED_QRCODE_TIME : 0;
	}
}



