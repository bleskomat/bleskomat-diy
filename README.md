# Bleskomat

An open-source Lightning Network ATM with simple components and a simple setup - just plug it in and it works!

Key features include:
* Works offline - no WiFi required
* Inexpensive, easily-sourced components
* Easily hackable and extendible


## Getting started

This section includes information about the requirements (software + hardware) that you will need to build your own open-source Bleskomat ATM.

### Software Requirements

* [Arduino IDE](https://www.arduino.cc/en/Main/Software)

Once you have installed [Arduino IDE](https://www.arduino.cc/en/Main/Software), you will have an `~/Arduino/libraries` folder in you user directory that you can use to include libraries:

* Clone the repository of [M5Stack](https://github.com/m5stack/M5Stack) into your `~/Arduino/libraries` folder.
* Add uncompressed `TFT_eSPI.zip` from this repository `./atm/libraries` folder to your `~/Arduino/libraries` folder.
* In you arduino IDE go to `File -> Preferences` and `https://dl.espressif.com/dl/package_esp32_index.json` to the input field in `Additional Boards Manager URLs`.


### Hardware Components

* ESP32
* TFT screen
* Coin Acceptor
* 12V DC power adaptor

#### Cable map to connect ESP32 to TFT screen

|  ESP32       | TFT      |
|--------------|----------|
| VIN          | VCC      |
| GND          | GND      |
| GPIO5  (D5)  | CS       |
| GPIO16 (RX2) | RS       |
| GPIO17 (TX2) | AO (DC)  |
| GPIO23 (D23) | SDA      |
| GPIO18 (D18) | SCK      |
| 3.3V (3V3)   | LED (NC) |


