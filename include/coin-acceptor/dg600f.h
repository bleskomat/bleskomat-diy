#ifndef BLESKOMAT_COIN_ACCEPTOR_DG600F_H
#define BLESKOMAT_COIN_ACCEPTOR_DG600F_H

#include "config.h"
#include "logger.h"
#include "util.h"

#include <HardwareSerial.h>

#include <deque>

namespace coinAcceptor_dg600f {
	void init();
	void loop();
	float getAccumulatedValue();
	void resetAccumulatedValue();
	void inhibit();
	void disinhibit();
}

#endif
