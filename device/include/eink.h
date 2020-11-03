
#include "logger.h"
#include "qrcode.h"
#include <iomanip>
#include <sstream>
#include <string>
#include <Arduino.h>

// Display Library example for SPI e-paper panels from Dalian Good Display and boards from Waveshare.
// Requires HW SPI and Adafruit_GFX. Caution: the e-paper panels require 3.3V supply AND data lines!
//
// Display Library based on Demo Example from Good Display: http://www.e-paper-display.com/download_list/downloadcategoryid=34&isMode=false.html
//
// Author: Jean-Marc Zingg
//
// Version: see library.properties
//
// Library: https://github.com/ZinggJM/GxEPD2

// GxEPD2_WS_ESP32_Driver : GxEPD2_Example variant for Universal e-Paper Raw Panel Driver Board, ESP32 WiFi / Bluetooth Wireless
// https://www.waveshare.com/product/e-paper-esp32-driver-board.htm

// Supporting Arduino Forum Topics:
// Waveshare e-paper displays with SPI: http://forum.arduino.cc/index.php?topic=487007.0
// Good Display ePaper for Arduino: https://forum.arduino.cc/index.php?topic=436411.0

// mapping of Waveshare ESP32 Driver Board
// BUSY -> 25, RST -> 26, DC -> 27, CS-> 15, CLK -> 13, DIN -> 14

// NOTE: this board uses "unusual" SPI pins and requires re-mapping of HW SPI to these pins in SPIClass
//       this example shows how this can be done easily

// base class GxEPD2_GFX can be used to pass references or pointers to the display instance as parameter, uses ~1.2k more code
// enable or disable GxEPD2_GFX base class
#define ENABLE_GxEPD2_GFX 0

#include <Wire.h>

// uncomment next line to use class GFX of library GFX_Root instead of Adafruit_GFX
//#include <GFX.h>
// Note: if you use this with ENABLE_GxEPD2_GFX 1:
//       uncomment it in GxEPD2_GFX.h too, or add #include <GFX.h> before any #include <GxEPD2_GFX.h>

#include <GxEPD2_BW.h>
#include <GxEPD2_3C.h>
#include <Fonts/FreeMonoBold9pt7b.h>
#include <Fonts/FreeMonoBold12pt7b.h>
#include <Fonts/FreeMonoBold24pt7b.h>

// #include "bitmaps/Bitmaps200x200.h" // 1.54" b/w
#include "bitmaps/Bitmaps128x296.h" // 2.9"  b/w
#include "bleskomat_128x296.h" // 2.9"  b/w

// comment out unused bitmaps to reduce code space used
// #include "bitmaps/Bitmaps200x200.h" // 1.54" b/w
// #include "bitmaps/Bitmaps104x212.h" // 2.13" b/w flexible GDEW0213I5F
// #include "bitmaps/Bitmaps128x250.h" // 2.13" b/w
// #include "bitmaps/Bitmaps128x296.h" // 2.9"  b/w
// #include "bitmaps/Bitmaps176x264.h" // 2.7"  b/w
// #include "bitmaps/Bitmaps400x300.h" // 4.2"  b/w
// #include "bitmaps/Bitmaps640x384.h" // 7.5"  b/w
// // 3-color
// #include "bitmaps/Bitmaps3c200x200.h" // 1.54" b/w/r
// #include "bitmaps/Bitmaps3c104x212.h" // 2.13" b/w/r
// #include "bitmaps/Bitmaps3c128x296.h" // 2.9"  b/w/r
// #include "bitmaps/Bitmaps3c176x264.h" // 2.7"  b/w/r
// #include "bitmaps/Bitmaps3c400x300.h" // 4.2"  b/w/r


namespace eink {
	void init();
	void splashScreen();
	void resetScreen();

#ifdef _GxBitmaps200x200_H_
	void drawBitmaps200x200();
#endif

#ifdef _GxBitmaps128x296_H_
	void drawBitmaps128x296();
#endif

	void updateAmount(const float &amount, const std::string &fiatCurrency);
	void clearAmount();
	float getRenderedAmount();
	void renderQRCode(const std::string &dataStr);
	void clearQRCode();
	bool hasRenderedQRCode();
	unsigned long getTimeSinceRenderedQRCode();
}
