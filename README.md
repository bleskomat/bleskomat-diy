# Bleskomat

The world's simplest Lightning Network ATM.

![](images/logo.png)

* [Overview](#overview)
* [Requirements](#requirements)
	* [Hardware Requirements](#hardware-requirements)
	* [Software Requirements](#software-requirements)
* [Setup](#setup)
	* [Building the Hardware Device](#building-the-hardware-device)
		* [Wiring Diagram](#wiring-diagram)
		* [Wiring the Power Supply](#wiring-the-power-supply)
		* [Wiring the TFT Display](#wiring-the-tft-display)
		* [Wiring the SD card](#wiring-the-sd-card)
		* [Wiring the Coin Acceptor](#wiring-the-coin-acceptor)
	* [Training the Coin Acceptor](#training-the-coin-acceptor)
	* [Installing Libraries and Dependencies](#installing-libraries-and-dependencies)
* [Compiling and Uploading to Device](#compiling-and-uploading-to-device)
* [Running the HTTP Server](#running-the-http-server)
* [Remote Tunneling](#remote-tunneling)
	* [Using SSH and a VPS](#using-ssh-and-a-vps)
	* [Using ngrok](#using-ngrok)
	* [Using TOR](#using-tor)
* [Generate and Test Signed LNURLs](#generate-and-test-signed-lnurls)
* [Notes](#notes)
* [License](#license)


## Overview

Key features of the Bleskomat ATM include:
* Works offline - no WiFi required
* Inexpensive, easily-sourced components
* Easily hackable and extendible
* Plug it in and it's ready to accept fiat in seconds

![](images/ux-flow.png)

The project consists of two parts:
* __HTTP Server__ - supports [lnurl-withdraw](https://github.com/btcontract/lnurl-rfc/blob/master/lnurl-withdraw.md) with additional request handlers for fiat-currency -> satoshi conversion and request signing.
* __Physical Device (ATM)__ - user inserts coins, device generates a signed LNURL and displays as QR code, user's app (which supports lnurl-withdraw) scans QR code and makes request to HTTP server, withdraw process is completed and the user has successfully bought satoshis with fiat coins.


## Requirements

This section includes information about the requirements (software + hardware) that you will need to build your own Bleskomat ATM.


### Hardware Requirements

To build the physical device, you will need the following hardware components:
* [ESP-WROOM-32](https://www.espressif.com/en/products/modules/esp-wroom-32/overview) by espressif
* TFT Display - The 1.8 inch (128x160 pixel) model is assumed by default
* Coin Acceptor - "Model HX-616"
* 12V DC power adaptor with \~1A
* Step-down converter with USB (F) adapter - alternatively you could use a USB car charger
* Standard USB to micro USB cable

It is strongly recommended to use a multimeter while wiring the device.


### Software Requirements

* [make](https://www.gnu.org/software/make/) - used as a meta task runner
* [PlatformIO Core (CLI)](https://docs.platformio.org/en/latest/core/) - used to compile and upload the firmware to the ESP32
* [nodejs](https://nodejs.org/) - used to run the HTTP server and as a task runner
	* On Linux and Mac install node via [nvm](https://github.com/creationix/nvm)


## Setup

Step-by-step setup process including both hardware and software.


### Building the Hardware Device

Before proceeding, be sure that you have all the project's [hardware requirements](#hardware-requirements).


#### Wiring Diagram

Here is a wiring diagram for the Bleskomat ATM:

![](images/wiring-diagram.png)


#### Wiring the Power Supply

The first step to building the device is wiring the power supply. If already plugged in, __unplug__ the 12V DC power supply now. Use scissors to cut the end off the power supply so that you can freely access the two wires within, as shown in the following image:

![](images/12v-dc-power-supply.jpg)

It is important to test the wires to know for certain which is the ground. Use a [multimeter](https://duckduckgo.com/?q=multimeter&t=canonical&iar=images&iax=images&ia=images) to measure the voltage of the power supply:
* Plug-in the power supply to electricity
* Turn on your multimeter and set it to measure voltage in the appropriate range
* Touch the __red__ lead of your multimeter to one of the wires
* Touch the __black__ lead of your multimeter to the other wire
* If you see a negative voltage reading, swap the leads between the two wires
* The wire touched by the __black__ lead is the ground ("gnd")
* The wire touched by the __red__ lead is the hot wire ("pwr")
* Unplug the power supply again

Now use the [wiring diagram](#wiring-diagram) above as a guide to wire the ESP32 to the power supply.

Note that powering the ESP32 via its micro USB port requires a regulated voltage of approximately 5V. The suggested step-down converter is the [XL4005](https://duckduckgo.com/?q=xl4005&t=canonical&iax=images&ia=images). It does a good job of keeping a steady voltage and doesn't generate much heat.

Once you've connected the step-down converter to the power supply, use your multimeter to measure the voltage at the out pins. Use a small screwdriver to turn the screw on the little blue box. Turning the screw counter clockwise should lower the voltage, turning it the opposite direction should increase the voltage. Once you have the voltage set to 5V, you can connect the USB (F) adapter to the out pins.

Use a standard USB to micro USB cable to connect the ESP32.

Do not forget to connect the ESP32 to the common ground. Without this connection, the ESP32 will not be able to receive pulses from the coin acceptor when the ESP32 is connected to your computer via USB.

There are other options when powering the ESP32 - e.g via the 3.3V pin or the 5V/VIN pin. You should __never__ power the ESP32 via more than one of these options at the same time. For example, do not power the ESP32 via its 3.3V pin while also connecting the ESP32 via USB to your computer. This can damage the ESP32 and possibly also your computer.



#### Wiring the TFT Display

Have a look at the [wiring diagram](#wiring-diagram) above or the table of cable mappings below:

|  ESP32       | TFT        |
|--------------|------------|
| VIN          | VCC        |
| GND          | GND        |
| GPIO5  (D5)  | CS         |
| GPIO16 (RX2) | RST        |
| GPIO17 (TX2) | RS         |
| GPIO23 (D23) | SDA        |
| GPIO18 (D18) | CLK (SCK)  |
| 3.3V (3V3)   | LED (NC)   |

Notes on pin naming:
* There are boards where `GPIXXX` are marked as `GXX` instead of `DXX`.
* The `G23` may be there **twice** - the correct one is next to `GND`.
* Some boards have typos so a bit of guess-and-check is necessary sometimes.


#### Wiring the SD card

The TTF display we are using contain a SD card module. The pins are located above the screen and not soldered to the module by default but the pins can be added. The table of mappings is the following:

|  ESP32       | TFT        |
|--------------|------------|
|  GPIO2  (D2) |  SD_MISO   |
|  GPIO15 (D15)|  SD_MOSI   |
|  GPIO14 (D14)|  SD_SCK    |
|  GPIO13 (D13)|  SD_CS     |


#### Wiring the Coin Acceptor

Have a look at the [wiring diagram](#wiring-diagram) above or the table of cable mappings below:

|  ESP32      | HX 616   | Power Supply  |
|-------------|----------|---------------|
|             | DC12V    | + 12V DC      |
| GPIO4 (D4)  | COIN     |               |
|             | GND      | - Ground      |

__Important notes__:
* There are two switches on the back of the HX-616 coin acceptor:
	* "NO" (normally open) / "NC" (normally closed) - This should be set to "NO"
	* "Fast" / "Medium" / "Slow" - This should be set to "Fast"


### Training the Coin Acceptor

Be sure that you've already finished wiring the power supply to all the components before continuing with this step.

Plug-in the power supply so that all the electronic components have power.

An instruction sheet is included with the coin acceptor that will guide you through the training process:

![](images/coin-acceptor-hx-616-instructions-side-1.jpg)

| PANEL  | Meaning                                     |
|--------|---------------------------------------------|
| E      | number of coin values                       |
| Hn     | sampling quantity of coin n                 |
| Pn     | signal output value of coin n               |
| Fn     | recognition accuracy of coin n (8 advised)  |
| A      | training the acceptor                       |
| An     | start inserting coins for coin n            |

Press ADD + MINUS to get to the setting menu for Hn, Pn, Fn. Then press SET to swap between possibilities. To change the option press ADD or MINUS to select the desired value and confirm with SET.

Press SET to enter the training menu (A appears on the panel). After pressing SET again you can start inserting the coins.


### Installing Libraries and Dependencies

Before proceeding, be sure that you have all the project's [software requirements](#software-requirements).

Use make to install libraries and dependencies for both the device firmware and HTTP server:
```bash
make install
```
* The server's dependencies are defined in its package.json file located at `./server/package.json`
* The firmware's dependencies are defined in its platformio.ini file located at `./device/platformio.ini`

If while developing you need to install a new library for the device firmware, use the following as a guide:
```bash
cd ./device
platformio lib install LIBRARY_NAME[@VERSION]
```
You can find PlatformIO's libraries repository [here](https://platformio.org/lib).

If you need to add dependencies to the server:
```bash
cd ./server
npm install --save LIBRARY_NAME[@VERSION]
```


## Compiling and Uploading to Device

Be sure that you've already [installed libraries and dependencies](#installing-libraries-and-dependencies) before continuing here.

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


## Running the HTTP Server

Be sure that you've already [installed libraries and dependencies](#installing-libraries-and-dependencies) before continuing here.

To start the server:
```bash
make server
```

To run the server while printing debug messages:
```bash
DEBUG=lnurl* make server
```
Example output as follows:
```
lnurl:info Creating web server... +0ms
lnurl:mock:c-lightning:info JSON-RPC API listening at /path/to/this/project/server/c-lightning.sock +0ms
lnurl:mock:c-lightning:info Listening for TCP connections at 127.0.0.1:9735 +0ms
lnurl:info Web server listening at http://127.0.0.1:3000/ +5ms
```

A configuration file is automatically generated for you to help get you started. The auto-generated config file is located at `./server/config.json`.

By default the server uses a mock c-lightning node. Once you're ready to connect your own Lightning Network node, have a look at the [lnurl-node](https://github.com/chill117/lnurl-node#lightning-backend-configuration-options) documentation for how to configure the server's lightning backend.

The server is only accessible on your local machine. But in order for the whole UX flow to work, you will need to expose it to the internet - see [Remote Tunneling](#remote-tunneling).


## Remote Tunneling

For the end-user's mobile device to be able to communicate with the server, the server will need to be exposed to the internet. One way to do this is to setup a remote tunnel. Here are described a couple different ways you can do this.


### Using SSH and a VPS

You can use this method if you already have a virtual private server (VPS) with its own static, public IP address.

Login to your VPS and add a few required configurations to its SSH config file:
```bash
cat >> /etc/ssh/sshd_config << EOL
    RSAAuthentication yes
    PubkeyAuthentication yes
    GatewayPorts yes
    AllowTcpForwarding yes
    ClientAliveInterval 60
    EOL
```
Restart the SSH service:
```bash
service ssh restart
```
On your local machine, run the following command to open a reverse-proxy tunnel:
```bash
ssh -v -N -T -R 3000:localhost:3000 VPS_HOST_OR_IP_ADDRESS
```
This will forward all traffic to port 3000 to the VPS thru the SSH tunnel to port 3000 on your local machine.

Set the value of `lnurl.url` in `./server/config.json` as follows:
```js
{
	"lnurl": {
		// ..
		"url": "http://VPS_IP_ADDRESS:3000"
		// ..
	}
}
```
Be sure to replace `VPS_IP_ADDRESS` with the IP address of your server.

Stop the server if it's currently running - press <kbd>Ctrl</kbd> + <kbd>C</kbd>.

Start the server again:
```bash
make server
```

Now let's move on to [Generate and Test Signed LNURLs](#generate-and-test-signed-lnurls).


### Using ngrok

If you don't have access to your own VPS, [ngrok](https://ngrok.com/) is another possible solution. Follow the installation instructions on the project's website before continuing here. Once you have ngrok installed, you can continue with the instructions here.

To create an HTTP tunnel to the local server:
```bash
ngrok http -region eu 3000
```
Copy and paste the HTTPS tunnel URL to `lnurl.url` in `./server/config.json` as follows:
```js
{
	"lnurl": {
		// ..
		"url": "https://0fe4d56b.eu.ngrok.io"
		// ..
	}
}
```
Note that each time you open a tunnel with ngrok, your tunnel URL changes.

Stop the server if it's currently running - press <kbd>Ctrl</kbd> + <kbd>C</kbd>.

Start the server again:
```bash
make server
```

Now let's move on to [Generate and Test Signed LNURLs](#generate-and-test-signed-lnurls).


### Using TOR

It should also be possible to use TOR for remote tunneling to your local server. Anyone who has done this and would like to write how-to instructions, please add them here.


## Generate and Test Signed LNURLs

There is a helper script included in this project that can generate signed LNURLs that are valid for your server. Run the script as follows:
```bash
make signedLnurl AMOUNT=1.00
```
The output is an unencoded URL - for example:
```
https://94fb07c3f0620eu.ngrok.io/u?id=ngNmBZs%3D&n=ead7b208dc1f6901&t=withdrawRequest&f=CZK&pn=1.00&px=1.00&pd=&s=2faf6595e2df24837ac1b655accd7a172901be216fac33d6643f3135bb4067cb
```
Query the URL with cURL:
```bash
curl "https://94fb07c3f0620eu.ngrok.io/u?id=ngNmBZs%3D&n=ead7b208dc1f6901&t=withdrawRequest&f=CZK&pn=1.00&px=1.00&pd=&s=2faf6595e2df24837ac1b655accd7a172901be216fac33d6643f3135bb4067cb"
```
The response will look something like this:
```
{"minWithdrawable":401000,"maxWithdrawable":401000,"defaultDescription":"","callback":"https://94fb07c3f0620eu.eu.ngrok.io/u","k1":"826fc889dd20128d11fbf9d07447254cc3b9f4db9f347aa05f2ca2e88ea046e7","tag":"withdrawRequest"}
```
Generate a signed LNURL and encode it as a QR code:
```bash
make signedLnurl AMOUNT=1.00 | ./server/node_modules/.bin/lnurl encode | qr
```
Note that this requires the [qr](https://github.com/lincolnloop/python-qrcode/) CLI utility.

If you prefer not to use the `qr` utility, then print the encoded LNURL:
```bash
make signedLnurl AMOUNT=1.00 | ./server/node_modules/.bin/lnurl encode
```
And copy the result into the QR encoder of your choice.

Once you've got a sample QR code, scan it with your mobile wallet app. The whole flow should work even if your server is still running the mock c-lightning, but obviously the LN payment will never be completed.

If you don't already have a mobile wallet app that supports LNURL, you can check out the [Introduction to Bleskomat](https://www.bleskomat.com/intro) page which has a list of mobile wallet apps that work with this project.


## Notes

* Exchange rates are queried using Coinbase's `/v2/exchange-rates` API end-point. The function that does this is defined in `./server/utils.js`. If you prefer to use a different provider, that is the place to make your changes.


## License

The device firmware source code is licensed under the [GNU General Public License v3 (GPL-3)](https://tldrlegal.com/license/gnu-general-public-license-v3-(gpl-3)):
> You may copy, distribute and modify the software as long as you track changes/dates in source files. Any modifications to or software including (via compiler) GPL-licensed code must also be made available under the GPL along with build & install instructions.

The server source code is licensed under the [GNU Affero General Public License v3 (AGPL-3.0)](https://tldrlegal.com/license/gnu-affero-general-public-license-v3-(agpl-3.0)):
> The AGPL license differs from the other GNU licenses in that it was built for network software. You can distribute modified versions if you keep track of the changes and the date you made them. As per usual with GNU licenses, you must license derivatives under AGPL. It provides the same restrictions and freedoms as the GPLv3 but with an additional clause which makes it so that source code must be distributed along with web publication. Since web sites and services are never distributed in the traditional sense, the AGPL is the GPL of the web.
