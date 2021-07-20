# Bleskomat DIY

This is the do-it-yourself (DIY) version of the Bleskomat ATM. If you're looking for the Bleskomat ATM product, you can find more information at [www.bleskomat.com](https://www.bleskomat.com).

Key features of the Bleskomat ATM include:
* Works offline - no WiFi required
* Inexpensive, easily-sourced components
* Easily hackable and extendible
* Plug it in and it's ready to accept fiat in seconds

The Bleskomat ATM must be paired with a [bleskomat-server](https://github.com/samotari/bleskomat-server) which can facilitate Lightning Network payments on behalf of one or more ATMs.

The rest of this document details how you can build your own version of the Bleskomat ATM project:
* [Requirements](#requirements)
	* [Hardware Requirements](#hardware-requirements)
	* [Software Requirements](#software-requirements)
* [Building the Hardware Device](#building-the-hardware-device)
	* [Wiring the Power Supply](#wiring-the-power-supply)
	* [ESP32 devkit pinout](#esp32-devkit-pinout)
	* [Wiring the TFT Display](#wiring-the-tft-display)
		* [Wiring the SD card slot](#wiring-the-sd-card-slot)
	* [Wiring the button](#wiring-the-button)
	* [Wiring the Coin Acceptor](#wiring-the-coin-acceptor)
* [Configure and Train Coin Acceptor](#configure-and-train-coin-acceptor)
* [Installing Libraries and Dependencies](#installing-libraries-and-dependencies)
* [Compiling and Uploading to Device](#compiling-and-uploading-to-device)
* [Prepare SD Card](#prepare-sd-card)
* [License](#license)
* [Trademark](#trademark)


## Requirements

This section includes information about the software and hardware requirements needed to build this project.


### Hardware Requirements

To build the physical device, you will need the following hardware components:
* ESP32 Devkit
	* [ESP-WROOM-32](https://www.espressif.com/en/products/modules/esp-wroom-32/overview) by espressif
	* [laskarduino.cz](https://www.laskarduino.cz/iot-esp-32s-2-4ghz-dual-mode-wifi-bluetooth-rev-1--cp2102/)
* DG600F Coin Acceptor:
	* [Alibaba.com](https://www.alibaba.com/trade/search?fsb=y&IndexArea=product_en&CatId=&SearchText=DG600F)
	* [Sparkfun.com](https://www.sparkfun.com/products/11636)
* 1.8" TFT display with SD card slot:
	* [laskarduino.cz](https://www.laskarduino.cz/128x160-barevny-lcd-tft-displej-1-8--spi/)
* XL4005 step-down converter:
	* [laskarduino.cz](https://www.laskarduino.cz/step-down-menic-s-xl4005/)
	* [Alibaba.com](https://www.alibaba.com//trade/search?fsb=y&IndexArea=product_en&CatId=&SearchText=xl4005)
* USB (F) adapter
	* [laskarduino.cz](https://www.laskarduino.cz/usb-f-na-dip-adapter/)
* Push button (4-pin)
	* [laskarduino.cz](https://www.laskarduino.cz/tlacitko-6x6x5mm/)
* 10k ohm resistor
* Micro SD card with macro adapter
* 12V DC power adaptor with \~1A
* Standard USB to micro USB cable

It is strongly recommended to use a multimeter while wiring the device to avoid burning any of the components due to wrong voltage being supplied.


### Software Requirements

* [make](https://www.gnu.org/software/make/)
* [PlatformIO Core (CLI)](https://docs.platformio.org/en/latest/core/)
	* Version 5 or newer
	* Only the CLI ("Core") is required


## Building the Hardware Device

Before proceeding, be sure that you have all the project's [hardware requirements](#hardware-requirements).

Step-by-step build process for the hardware device.


### Wiring the Power Supply

The first step to building the device is wiring the power supply. If already plugged in, __unplug__ the 12V DC power supply now. Use scissors to cut the end off the power supply so that you can freely access the two wires within.

It is important to test the wires to know for certain which is the ground. Use a [multimeter](https://duckduckgo.com/?q=multimeter&t=canonical&iar=images&iax=images&ia=images) to measure the voltage of the power supply:
* Plug-in the power supply to electricity
* Turn on your multimeter and set it to measure voltage in the appropriate range
* Touch the __red__ lead of your multimeter to one of the wires
* Touch the __black__ lead of your multimeter to the other wire
* If you see a negative voltage reading, swap the leads between the two wires
* The wire touched by the __black__ lead is the ground ("GND")
* The wire touched by the __red__ lead is the hot wire ("DC12V")
* Unplug the power supply again

Note that powering the ESP32 via its micro USB port requires a regulated voltage of approximately 5V. The suggested step-down converter is the [XL4005](https://duckduckgo.com/?q=xl4005&t=canonical&iax=images&ia=images). It does a good job of keeping a steady voltage and doesn't generate much heat. It's possible to instead use a USB car charger in case you have an extra one lying around.

Once you've connected the step-down converter to the power supply, use your multimeter to measure the voltage at the out pins. Use a small screwdriver to turn the screw on the little blue box. Turning the screw counter clockwise should lower the voltage, turning it the opposite direction should increase the voltage.

Once you have the voltage set to 5V, connect the out pins to the center power rails of the breadboard.

Connect the the USB (F) adapter to the out pins

Use a standard USB to micro USB cable to connect the ESP32.


#### ESP32: Common Ground

Do not forget to connect the ESP32 to the common ground. Without this connection, the ESP32 will not be able to receive a clean signal from the coin acceptor when the ESP32 is connected to your computer via USB.


#### ESP32: Power Options Warning

There are other options when powering the ESP32 - e.g via the 3.3V pin or the 5V/VIN pin. You should __never__ power the ESP32 via more than one of these options at the same time. For example, do not power the ESP32 via its 3.3V pin while also connecting the ESP32 via USB to your computer. This can damage the ESP32 and possibly also your computer.


### ESP32 devkit pinout

![](docs/ESP32-devkit-v1-board-pinout-36-gpio-pins.jpg)


### Wiring the TFT Display

Have a look at the [wiring diagram](#wiring-diagram) above or the table of cable mappings below:

|  ESP32       | TFT        |
|--------------|------------|
| VIN (V5)     | VCC        |
| GND          | GND        |
| GPIO22       | CS         |
| GPIO4        | RESET      |
| GPIO2        | AO         |
| GPIO23       | SDA        |
| GPIO18       | SCK (CLK)  |
| 3V3          | LED (NC)   |

Notes on pin naming:
* There are boards where `GPIXXX` are marked as `GXX` instead of `DXX`.
* The `G23` may be there **twice** - the correct one is next to `GND`.
* Some boards have typos so a bit of guess-and-check is necessary sometimes.

Refer to the [ESP32 devkit pinout](#esp32-devkit-pinout) for help identifying the pins on your ESP32.


#### Wiring the SD card slot

The TFT display includes an SD card slot. The pins are located above the screen and not soldered to the module by default but the pins can be added. The table of mappings is the following:

| ESP32  | TFT     |
|--------|---------|
| GPIO15 | SD_CS   |
| GPIO13 | SD_MOSI |
| GPIO12 | SD_MISO |
| GPIO14 | SD_SCK  |

Refer to the [ESP32 devkit pinout](#esp32-devkit-pinout) for help identifying the pins on your ESP32.


### Wiring the button

Wire the button using the following table as a guide:

| ESP32    | Button    |
|----------|-----------|
| VIN (V5) | left pin  |
| GPIO33   | right pin |

Connect the right pin of the button to GND with a 10k ohm resistor.

Refer to the [ESP32 devkit pinout](#esp32-devkit-pinout) for help identifying the pins on your ESP32.


### Wiring the Coin Acceptor

| ESP32       | DG600F   | Power Supply  |
|-------------|----------|---------------|
| GPIO1 (TX0) | INHIBIT  |               |
|             | COUNTER  |               |
|             | GND      | - Ground      |
| GPIO3 (RX0) | SIGNAL   |               |
|             | DC12V    | + 12V DC      |

Refer to the [ESP32 devkit pinout](#esp32-devkit-pinout) for help identifying the pins on your ESP32.


## Configure and Train Coin Acceptor

Physical switches on the DG600F should set as follows:

| Switch           | State         |
|------------------|---------------|
| 1 (Port Level)   | Down (NO)     |
| 2 (Security)     | Down (Normal) |
| 3 (Transmitting) | Up (RS232)    |
| 4 (Inhibiting)   | Down (> +3V)  |

![](docs/DG600F-DIP-switch-configuration.png)

Open the [DG600F manual](docs/DG600F-Coin-Acceptor-Technical-Manual.pdf) to "Coin Acceptor Parameters Setting" on page 18. Set the parameters as follows:

| Parameter | Description                      | Value | Meaning                                          |
|-----------|----------------------------------|-------|--------------------------------------------------|
| A1        | machine charge amount            | 01    | min. coin value before data sent                 |
| A2        | serial output signal pulse-width | 01    | 25ms / 9600 bps (RS232 baud rate)                |
| A3        | faulty alarm option              | 01    | (rings only one time)                            |
| A4        | serial port RS232 signal length  | 03    | 3 bytes: 0xAA, coin value, XOR of prev two bytes |
| A5        | serial port output               | 01    | output to serial pin                             |


To train the coin acceptor, have a look at "Coin Parameters Setting" on page 16 of the [DG600F manual](docs/DG600F-Coin-Acceptor-Technical-Manual.pdf). Be sure to set the "coin value" for each coin in series, incremented by 1. For example:
* 1 CZK = 1 coin value
* 2 CZK = 2 coin value
* 5 CZK = 3 coin value
* 10 CZK = 4 coin value
* 20 CZK = 5 coin value
* 50 CZK = 6 coin value

Then in bleskomat.conf, set the `coinValues` setting as follows:
```
coinValues=1,2,5,10,20,50
```


## Installing Libraries and Dependencies

Before proceeding, be sure that you have all the project's [software requirements](#software-requirements).

Use make to install libraries and dependencies needed to build the firmware:
```bash
make install
```
* The firmware's dependencies are defined in its platformio.ini file located at `./platformio.ini`

If while developing you need to install a new library, use the following as a guide:
```bash
platformio lib install LIBRARY_NAME[@VERSION]
```
You can find PlatformIO's libraries repository [here](https://platformio.org/lib).


## Compiling and Uploading to Device

To compile the firmware (without uploading to a device):
```bash
make compile
```

To compile and upload to your device:
```bash
make upload DEVICE=/dev/ttyUSB0
```
The device path for your operating system might be different. If you receive a "Permission denied" error about `/dev/ttyUSB0` then you will need to set permissions for that file on your system:
```bash
sudo chown $USER:$USER /dev/ttyUSB0
```

To open the serial monitor:
```bash
make monitor DEVICE=/dev/ttyUSB0
```
Again the device path here could be different for your operating system.


## Prepare SD Card

Format the SD card with the FAT32 filesystem.

The following is an example `bleskomat.conf` file that you could use to configure a bleskomat device. Create the file and copy it to the root directory of the SD card.
```
apiKey.id=6d830ddeb0
apiKey.key=b11cd6b002916691ccf3097eee3b49e51759225704dde88ecfced76ad95324c9
apiKey.encoding=hex
callbackUrl=https://your-bleskomat-server/u
shorten=true
uriSchemaPrefix=LIGHTNING:
fiatCurrency=CZK
fiatPrecision=0
coinValues=1,2,5,10,20,50
```


### Configuration Options

The following is a list of all possible configuration options that can be set via the bleskomat.conf configuration file:
* `apiKey.id` - The API key ID of the device. This is needed by the server to verify signatures created by the device.
* `apiKey.key` - The API key secret that is used to generate signatures.
* `apiKey.encoding` - The explicit encoding of the API key secret. This can be "hex", "base64", or empty-string (e.g "") to mean no encoding. When generating a new API key on the server, it will store the encoding along with the ID and secret.
* `callbackUrl` - The LNURL server base URL plus endpoint path. Example:
	* `https://your-bleskomat-server.com/u`
* `shorten` - Whether or not to shorten the LNURL; see the [lnurl-node module](https://github.com/chill117/lnurl-node#signed-lnurls) for more details.
* `uriSchemaPrefix` - The URI schema prefix for LNURLs generated by the device. It has been discovered that some wallet apps mistakenly only support lowercase URI schema prefixes. Uppercase is better because when encoded as a QR code, the generated image is less complex and so easier to scan. Set this config to empty-string (e.g `uriSchemaPrefix=`) to not prepend any URI schema prefix.
* `fiatCurrency` - The fiat currency symbol for which the device is configured; see [ISO 4217](https://en.wikipedia.org/wiki/ISO_4217).
* `fiatPrecision` - The number of digits to the right of the decimal point when rendering fiat currency amounts.
* `coinValues` - The value of coins for which the coin acceptor has been configured. Each value separated by a comma. Integers and floating point (decimal) values are accepted. Examples:
	* CZK: `1,2,5,10,20,50`
	* EUR: `0.05,0.10,0.20,0.50,1,2`


## License

The project is licensed under the [GNU General Public License v3 (GPL-3)](https://tldrlegal.com/license/gnu-general-public-license-v3-(gpl-3)):
> You may copy, distribute and modify the software as long as you track changes/dates in source files. Any modifications to or software including (via compiler) GPL-licensed code must also be made available under the GPL along with build & install instructions.


## Trademark

"Bleskomat" is a registered trademark. You are welcome to hack, fork, build, and use the source code and instructions found in this repository. However, the right to use the name "Bleskomat" with any commercial products or services is withheld and reserved for the trademark owner.
