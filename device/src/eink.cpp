#include "eink.h"

#include <stdlib.h>
#include <string.h>

namespace eink {
#if defined(ESP32)
/* ******************** */
/* Bleskomat displays */
/* - for the 2.9 inch display use */
/* GxEPD2_BW<GxEPD2_290, GxEPD2_290::HEIGHT> display(GxEPD2_290(/\*CS=*\/ 15, /\*DC=*\/ 27, /\*RST=*\/ 26, /\*BUSY=*\/ 25));  */
/* - for the 1.54 inch display use */
	GxEPD2_BW<GxEPD2_154, GxEPD2_154::HEIGHT> display(GxEPD2_154(/*CS=*/ 15, /*DC=*/ 27, /*RST=*/ 26, /*BUSY=*/ 25)); // GDEP015OC1 no longer available
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
		// first update should be full refresh
		helloWorld();
		delay(1000);
		// // partial refresh mode can be used to full screen,
		// // effective if display panel hasFastPartialUpdate
		// helloFullScreenPartialMode();
		// delay(1000);
		// helloArduino();
		// delay(1000);
		// helloEpaper();
		// delay(1000);
		// showFont("FreeMonoBold9pt7b", &FreeMonoBold9pt7b);
		// delay(1000);
		// drawBitmaps();
		// if (display.epd2.hasPartialUpdate)
		// {
		// 	showPartialUpdate();
		// 	delay(1000);
		// } // else // on GDEW0154Z04 only full update available, doesn't look nice
		// //drawCornerTest();
		// //showBox(16, 16, 48, 32, false);
		// //showBox(16, 56, 48, 32, true);
		display.powerOff();
		// deepSleepTest();
		Serial.println("setup done");
	}

    // note for partial update window and setPartialWindow() method:
    // partial update window size and position is on byte boundary in physical x direction
    // the size is increased in setPartialWindow() if x or w are not multiple of 8 for even rotation, y or h for odd rotation
    // see also comment in GxEPD2_BW.h, GxEPD2_3C.h or GxEPD2_GFX.h for method setPartialWindow()

	const char HelloWorld[] = "BORDEL";
	const char name[] = "Mario!";
	const char HelloArduino[] = "Hello Arduino!";
	const char HelloEpaper[] = "Hello E-Paper!";

	void helloWorld()
	{
		//Serial.println("helloWorld");
		display.setRotation(1);
		display.setFont(&FreeMonoBold24pt7b);
		display.setTextColor(GxEPD_BLACK);
		int16_t tbx, tby; uint16_t tbw, tbh;
		display.getTextBounds(HelloWorld, 0, 0, &tbx, &tby, &tbw, &tbh);
		// center bounding box by transposition of origin:
		uint16_t x = ((display.width() - tbw) / 2) - tbx;
		uint16_t y = ((display.height() - tbh) / 2) - tby;
		display.setFullWindow();
		display.firstPage();
		do
		{
			display.fillScreen(GxEPD_WHITE);
			display.setCursor(x, y);
			display.print(HelloWorld);
		}
		while (display.nextPage());
		//Serial.println("helloWorld done");
	}

	void updateAmount(const float &amount, const std::string &fiatCurrency)
	{
	}
	void clearAmount(){}
	float getRenderedAmount(){
		return 3.2;
	}
	void renderQRCode(const std::string &dataStr){}
	void clearQRCode(){}
	bool hasRenderedQRCode()
	{
		return true;
	}

	unsigned long getTimeSinceRenderedQRCode(){}
}



