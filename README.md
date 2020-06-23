# Bleskomat

An open-source Lightning Network ATM with simple components and a simple setup - just plug it in and it works!

* [Overview](#overview)
* [Requirements](#requirements)
* [Physical Device Setup](#physical-device-setup)
* [Running the HTTP Server](#running-the-http-server)
* [Connecting Everything Together](#connecting-everything-together)
* [License](#license)


## Overview

Key features include:
* Works offline - no WiFi required
* Inexpensive, easily-sourced components
* Easily hackable and extendible

The project consists of two parts:
* __HTTP Server__ - supports [lnurl-withdraw](https://github.com/btcontract/lnurl-rfc/blob/master/lnurl-withdraw.md) with additional request handlers for fiat-currency -> satoshi conversion and request signing.
* __Physical Device (ATM)__ - user inserts coins, device generates a signed URL and displays as QR code, user's app (which supports lnurl-withdraw) scans QR code and makes request to HTTP server, withdraw process is completed and the user has successfully bought satoshis with fiat coins.


## Requirements

This section includes information about the requirements (software + hardware) that you will need to build your own open-source Bleskomat ATM. 

To build the physical device:
* Hardware:
	* ESP32
	* TFT screen
	* Coin Acceptor
	* 12V DC power adaptor
* Software:
	* [Arduino IDE](https://www.arduino.cc/en/Main/Software); alternative:
		* [Sublime Text 3](https://www.sublimetext.com/3) + [Deviot (Arduino IDE)](https://packagecontrol.io/packages/Deviot%20(Arduino%20IDE))
	* [M5Stack](https://github.com/m5stack/M5Stack) - Arduino library for the ESP32 used for this project
	* [TFT_eSPI.zip](https://github.com/samotari/bleskomat/tree/master/atm/libraries/TFT_eSPI.zip) - Additional library needed for the TFT screen

To run the HTTP server:
* [nodejs](https://nodejs.org/) - For Linux and Mac install node via [nvm](https://github.com/creationix/nvm)


## Physical Device Setup

Install the [Arduino IDE](https://www.arduino.cc/en/Main/Software).

Clone the repository of [M5Stack](https://github.com/m5stack/M5Stack) into your `~/Arduino/libraries` folder.

Download and unzip [TFT_eSPI.zip](https://github.com/samotari/bleskomat/tree/master/atm/libraries/TFT_eSPI.zip) into your `~/Arduino/libraries` folder.

In Arduino IDE, go to `File -> Preferences` and add `https://dl.espressif.com/dl/package_esp32_index.json` to the input field in `Additional Boards Manager URLs`.

### Cable map to connect ESP32 to TFT screen

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


## Running the HTTP Server

Install node.js server dependencies:
```bash
cd ./server && npm install
```
Start the server:
```bash
npm start
```
A configuration file is automatically generated for you to help get you started. You will eventually want to overwrite the configurations with real values later - for example when you are ready to connect your LN node to make payments on the Lightning Network. The auto-generated config file is located at `./server/config.json`.

Note that the server is only accessible on your local machine. But in order for the whole UX flow to work, you will need to expose the server to the internet - see [Remote Tunneling](#remote-tunneling).


### Remote Tunneling

For the end-user's mobile device to be able to communicate with your HTTP server, the server will need to be exposed to the internet. One way to do this is to setup a remote tunnel. Here are described a couple different ways you can do this.

#### Using SSH and a VPS

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

Set the value of `url` in `./server/config.json` as follows:
```js
{
	// ..
	"url": "http://VPS_IP_ADDRESS:3000"
	// ..
}
```
Be sure to replace `VPS_IP_ADDRESS` with the IP address of your server.

Stop the server if it's currently running - press <kbd>Ctrl</kbd> + <kbd>C</kbd>.

Start the server:
```bash
npm start
```


#### Using ngrok

If you don't have access to your own VPS, [ngrok](https://ngrok.com/) is another possible solution. Follow the installation instructions on the project's website before continuing here. Once you have ngrok installed, you can continue with the instructions here.

To create an HTTP tunnel to the local server:
```bash
ngrok http -region eu 3000
```
You should see something like the following:

![](https://github.com/samotari/bleskomat/blob/master/images/ngrok-screen-https-tunnel.png)

Copy and paste the HTTPS tunnel URL to `url` in `./server/config.json` as follows:
```js
{
	// ..
	"url": "https://0fe4d56b.eu.ngrok.io"
	// ..
}
```
Note that each time you open a tunnel with ngrok, your tunnel URL changes. 

Stop the server if it's currently running - press <kbd>Ctrl</kbd> + <kbd>C</kbd>.

Start the server:
```bash
npm start
```


## Connecting Everything Together

Now that you have both the physical device and the HTTP server both setup, you are ready to put them together. Have a look at your server's configuration file (`./server/config.json`). There you should see an API key that was automatically generated when you first started your server:
```json
	// ...
	"auth": {
		"apiKeys": [
			{
				"id": "RAW96Uw=",
				"key": "HGtx0LJZZwdZMvbEQTy2V3FD2NFh9RVvGG7haA2ZVYE="
			}
		]
	}
	// ...
```
Copy/paste the "id" to `./atm/bleskomat.ino` as follows (near the top of the file):
```cpp
std::string apiKeyId = "RAW96Uw=";
```
And the same for "key":
```cpp
std::string apiKeySecret = "HGtx0LJZZwdZMvbEQTy2V3FD2NFh9RVvGG7haA2ZVYE=";
```
You will also need the full URL to your server:
```js
{
	// ..
	"url": "https://0fe4d56b.eu.ngrok.io",
	"endpoint": "/u"
	// ..
}
```
Copy/paste the value of "url" + "endpoint" to `./atm/bleskomat.ino`:
```cpp
std::string baseUrl = "https://0fe4d56b.eu.ngrok.io/u?";
```
Rebuild and deploy the ATM project code to your ESP32. Now the QR codes generated by the ATM will be properly signed and accepted by your server.


## License

The ATM firmware source code is licensed under the [GNU General Public License v3 (GPL-3)](https://tldrlegal.com/license/gnu-general-public-license-v3-(gpl-3)):
> You may copy, distribute and modify the software as long as you track changes/dates in source files. Any modifications to or software including (via compiler) GPL-licensed code must also be made available under the GPL along with build & install instructions.

The server source code is licensed under the [GNU Affero General Public License v3 (AGPL-3.0)](https://tldrlegal.com/license/gnu-affero-general-public-license-v3-(agpl-3.0)):
> The AGPL license differs from the other GNU licenses in that it was built for network software. You can distribute modified versions if you keep track of the changes and the date you made them. As per usual with GNU licenses, you must license derivatives under AGPL. It provides the same restrictions and freedoms as the GPLv3 but with an additional clause which makes it so that source code must be distributed along with web publication. Since web sites and services are never distributed in the traditional sense, the AGPL is the GPL of the web.
