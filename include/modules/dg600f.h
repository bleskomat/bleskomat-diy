/*
	Copyright (C) 2020 Samotari (Charles Hill, Carlos Garcia Ortiz)

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

#ifndef BLESKOMAT_MODULES_DG600F_H
#define BLESKOMAT_MODULES_DG600F_H

#include "config.h"
#include "logger.h"
#include "util.h"

#include <HardwareSerial.h>

#include <deque>

#ifndef COIN_ACCEPTOR_SIGNAL
	#define COIN_ACCEPTOR_SIGNAL 3
#endif

#ifndef COIN_ACCEPTOR_INHIBIT
	#define COIN_ACCEPTOR_INHIBIT 1
#endif

#ifndef COIN_ACCEPTOR_BAUDRATE
	#define COIN_ACCEPTOR_BAUDRATE 9600
#endif

namespace dg600f {
	void init();
	void loop();
	float getAccumulatedValue();
	float getMaxCoinValue();
	void reset();
	bool isOff();
	bool isOn();
	void on();
	void off();
}

#endif
