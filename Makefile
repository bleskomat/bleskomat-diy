#
#	Copyright (C) 2020 Samotari (Charles Hill, Carlos Garcia Ortiz)
#
#	This program is free software: you can redistribute it and/or modify
#	it under the terms of the GNU General Public License as published by
#	the Free Software Foundation, either version 3 of the License, or
#	(at your option) any later version.
#
#	This program is distributed in the hope that it will be useful,
#	but WITHOUT ANY WARRANTY; without even the implied warranty of
#	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#	GNU General Public License for more details.
#
#	You should have received a copy of the GNU General Public License
#	along with this program.  If not, see <https://www.gnu.org/licenses/>.
#

## Usage
#
#   $ make install     # run install process(es)
#   $ make compile     # compile the device firmware
#   $ make upload      # compile then upload the device firmware
#   $ make monitor     # start the serial monitor
#   $ make server      # start an instance of the HTTP server
#

## Variables
DEVICE=./device
SERVER=./server

## Targets
#
# The format goes:
#
#   target: list of dependencies
#     commands to build target
#
# If something isn't re-compiling double-check the changed file is in the
# target's dependencies list.

## Phony targets - these are for when the target-side of a definition
# (such as "install" below) isn't a file but instead a just label. Declaring
# it as phony ensures that it always run, even if a file by the same name
# exists.
.PHONY: install\
compile\
upload\
monitor\
server\
signedLnurl

.SILENT: signedLnurl

install:
	cd $(DEVICE) && platformio lib install
	cd $(SERVER) && npm install

compile:
	cd $(DEVICE) && npm run compile:only

upload:
	cd $(DEVICE) && npm run compile:upload

monitor:
	cd $(DEVICE) && npm run monitor

server:
	cd $(SERVER) && npm start

signedLnurl:
	cd $(SERVER) && npm run --silent generate:signedLnurl -- "${AMOUNT}"
