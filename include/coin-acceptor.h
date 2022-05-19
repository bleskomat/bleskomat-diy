#ifndef BLESKOMAT_COIN_ACCEPTOR_H
#define BLESKOMAT_COIN_ACCEPTOR_H

#include "coin-acceptor/dg600f.h"
#include "coin-acceptor/hx616.h"
#include "config.h"
#include "logger.h"

namespace coinAcceptor {
	void init();
	void loop();
	float getAccumulatedValue();
	void resetAccumulatedValue();
	bool isInhibited();
	void inhibit();
	void disinhibit();
}

#endif
