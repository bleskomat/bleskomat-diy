#ifndef BLESKOMAT_COIN_ACCEPTOR_HX616_H
#define BLESKOMAT_COIN_ACCEPTOR_HX616_H

#include "config.h"
#include "logger.h"
#include <Arduino.h>

namespace coinAcceptor_hx616 {
	void init();
	void loop();
	float getAccumulatedValue();
	void resetAccumulatedValue();
}

#endif