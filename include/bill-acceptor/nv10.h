#ifndef BLESKOMAT_BILL_ACCEPTOR_NV10_H
#define BLESKOMAT_BILL_ACCEPTOR_NV10_H

#include "config.h"
#include "logger.h"
#include "util.h"
#include <HardwareSerial.h>
#include <deque>
#include <map>

namespace billAcceptor_nv10 {
	void init();
	void loop();
	float getAccumulatedValue();
	void resetAccumulatedValue();
	void inhibit();
	void disinhibit();
}

#endif
