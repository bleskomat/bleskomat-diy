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

#include "display.h"

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


// #include "bitmaps/Bitmaps128x296.h" // 2.9"  b/w

#ifdef EINK_200x200
#include "modules/bleskomat_200x200.h" // 1.54" b/w
#else  // if EINK_128x296
#include "modules/bleskomat_128x296.h" // 2.9"  b/w
#endif

namespace eink {
	void init();
	void splashScreen();
	void resetScreen();
	void drawLogo();
	void updateAmount(const float &amount, const std::string &fiatCurrency);
	void clearAmount();
	float getRenderedAmount();
	void renderQRCode(const std::string &dataStr);
	void clearQRCode();
	bool hasRenderedQRCode();
	unsigned long getTimeSinceRenderedQRCode();
}


namespace screen = eink;
