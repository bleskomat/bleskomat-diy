#include <SPI.h>
#include <iostream>
#include "lnurl.h"
#include <sstream>
#include <string>
#include <TFT_eSPI.h>
#include "qrcode.h"
#include "util.h"
#define BG_COLOR 0xFFFF// WHITE
#define TEXT_COLOR 0x0000// BLACK

TFT_eSPI tft = TFT_eSPI();

/*
 *  CONFIGURATIONS
*/
// API ID and key that will be used to generate signed LNURLs:
std::string apiKeyId = "";
std::string apiKeySecret = "";
// The base URL for the lnurl server instance:
std::string baseUrl = "https://t1.bleskomat.com/u?";
// The symbol of the fiat currency which the coin acceptor is configured to accept:
std::string fiatCurrency = "CZK";
// The amount by which to increase the internal accumulated fiat value for each signal from the coin acceptor:
const float valueIncrement = 1.00;
// The pin to which the COIN input is connected:
const byte coinSig = 4;
/*
 *  end of CONFIGURATIONS
*/


byte previousCoinSignal;
const unsigned long bootTime = millis();// milliseconds
const unsigned long minWaitAfterBootTime = 2000;// milliseconds
const unsigned long minWaitTimeBetweenCoins = 7000;// milliseconds
const unsigned long maxTimeDisplayQrCode = 60000;// milliseconds
unsigned long lastCoinInsertedTime = 0;// milliseconds
unsigned long lastDisplayedQrCodeTime = 0;// milliseconds
float accumulatedValue = 0.00;

void setup() {
  Serial.begin(115200);
  Serial.println("Setting up...");
  Serial.println("Serial ready");
  tft.begin();
  tft.fillScreen(BG_COLOR);
  Serial.println("Screen ready");
  pinMode(coinSig, INPUT_PULLUP);
  previousCoinSignal = digitalRead(coinSig);
  Serial.println("Coin reader ready");
  Serial.println("Setup complete!");
}

unsigned long lastSignalChangeTime = 0;
void loop() {
  const byte currentCoinSignal = digitalRead(coinSig);
  if (currentCoinSignal != previousCoinSignal) {
    previousCoinSignal = currentCoinSignal;
    const unsigned long timeSinceLastSignalChange = millis() - lastSignalChangeTime;
    lastSignalChangeTime = millis();
//    if (currentCoinSignal == HIGH) {
//      Serial.println("HIGH");
//    } else if (currentCoinSignal == LOW) {
//      Serial.println("LOW");
//    }
//    std::ostringstream msg2;
//    msg2 << "LAST SIGNAL CHANGE WAS " << timeSinceLastSignalChange << " ms AGO";
//    std::cout << msg2.str() << '\n';
    if (
      currentCoinSignal == HIGH &&
      millis() - bootTime >= minWaitAfterBootTime &&
      timeSinceLastSignalChange > 25 &&
      timeSinceLastSignalChange < 35
    ) {
      // A coin was inserted.
      // This code executes once for each value unit the coin represents.
      // For example: A coin worth 5 CZK will execute this code 5 times.
      accumulatedValue = accumulatedValue + valueIncrement;
//      std::ostringstream msg;
//      msg << "ACCUMULATED VALUE: " << accumulatedValue;
//      std::cout << msg.str() << '\n';
      lastCoinInsertedTime = millis();
      if (lastDisplayedQrCodeTime > 0) {
        clear_qrcode();
      }
    }
  }
  if (lastDisplayedQrCodeTime > 0 && millis() - lastDisplayedQrCodeTime >= maxTimeDisplayQrCode) {
    clear_qrcode();
  }
  if (lastCoinInsertedTime > 0 && millis() - lastCoinInsertedTime >= minWaitTimeBetweenCoins) {
    // The minimum required wait time between coins has passed.
    generate_and_display_withdraw_request();
    // Reset accumulated value counter.
    accumulatedValue = 0.00;
    // Reset the last coin inserted time.
    lastCoinInsertedTime = 0;
  }
  update_displayed_accumulated_value();
}

void generate_and_display_withdraw_request() {
  std::string req = lnurl::create_signed_withdraw_request(
    accumulatedValue,
    fiatCurrency,
    apiKeyId,
    apiKeySecret,
    baseUrl
  );
  std::ostringstream msg;
  msg << "LNURL: " << req;
  std::cout << msg.str() << '\n';
  display_qrcode(req);
}

float lastDisplayedAccumulatedValue = 0.00;
unsigned long lastDisplayedAccumulatedValueTime = 0;

void update_displayed_accumulated_value() {
  if (
    (lastDisplayedQrCodeTime == 0) && (
      (lastDisplayedAccumulatedValueTime == 0) ||
      (lastDisplayedAccumulatedValue != accumulatedValue && millis() - lastDisplayedAccumulatedValueTime >= 500)
    )
  ) {
    display_accumulated_value();
  }
}

const uint8_t baseFontCharWidth = 6;
const uint8_t baseFontCharHeight = 8;
const uint8_t accumulatedValue_textSize = 2;
const uint8_t accumulatedValue_displayedTextHeight = baseFontCharHeight * accumulatedValue_textSize;

void clear_accumulated_value() {
  Serial.println("Clearing accumulated value...");
  // Clear previous text by drawing a white rectangle over it.
  tft.fillRect(0, accumulatedValue_displayedTextHeight, tft.width(), accumulatedValue_displayedTextHeight, BG_COLOR);
  lastDisplayedAccumulatedValue = 0.00;
  lastDisplayedAccumulatedValueTime = 0;
}

void display_accumulated_value() {
  clear_accumulated_value();
  Serial.println("Displaying accumulated value...");
  std::ostringstream fiat;
  fiat << accumulatedValue << " " << fiatCurrency;
  const std::string str = fiat.str();
  const char* text = str.c_str();
  const uint8_t displayedTextWidth = baseFontCharWidth * accumulatedValue_textSize * str.length();
  tft.setTextSize(accumulatedValue_textSize);
  tft.setTextColor(TEXT_COLOR);
  tft.setCursor((tft.width() - displayedTextWidth) / 2, accumulatedValue_displayedTextHeight);
  tft.println(text);
  lastDisplayedAccumulatedValue = float(accumulatedValue);
  lastDisplayedAccumulatedValueTime = millis();
}

const uint8_t qrcode_width = 98;
const uint8_t qrcode_height = 98;
const uint8_t qrcode_offsetX = (tft.width() - qrcode_width) / 2;
const uint8_t qrcode_offsetY = accumulatedValue_displayedTextHeight * 3;

void clear_qrcode() {
  Serial.println("Clearing QR code...");
  tft.fillRect(qrcode_offsetX, qrcode_offsetY, qrcode_width, qrcode_height, BG_COLOR);
  lastDisplayedQrCodeTime = 0;
}

std::string str_toupper(std::string s) {
  std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c){ return std::toupper(c); });
  return s;
}

void display_qrcode(const std::string &dataStr) {
  clear_qrcode();
  Serial.println("Displaying QR code...");
  const char* data = str_toupper(dataStr).c_str();
  int qrSize = 12;
  int sizes[17] = { 25, 47, 77, 114, 154, 195, 224, 279, 335, 395, 468, 535, 619, 667, 758, 854, 938 };
  int len = std::string(data).length();
  for (int i = 0; i < 17; i++) {
    if (sizes[i] > len) {
      qrSize = i + 1;
      break;
    }
  }
  QRCode qrcode;
  uint8_t qrcodeData[qrcode_getBufferSize(qrSize)];
  qrcode_initText(&qrcode, qrcodeData, qrSize, ECC_LOW, data);
  float scale = 2;
  for (uint8_t y = 0; y < qrcode.size; y++) {
    for (uint8_t x = 0; x < qrcode.size; x++) {
      if (qrcode_getModule(&qrcode, x, y)) {
        tft.fillRect(qrcode_offsetX + scale*x, qrcode_offsetY + scale*y, scale, scale, TEXT_COLOR);
      } else {
        tft.fillRect(qrcode_offsetX + scale*x, qrcode_offsetY + scale*y, scale, scale, BG_COLOR);
      }
    }
  }
  lastDisplayedQrCodeTime = millis();
}
