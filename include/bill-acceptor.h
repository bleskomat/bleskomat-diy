#ifndef BLESKOMAT_BILL_ACCEPTOR_H
#define BLESKOMAT_BILL_ACCEPTOR_H

#include "bill-acceptor/nv10.h"
#include "logger.h"

namespace billAcceptor {
	void init();
	void loop();
	float getAccumulatedValue();
	void resetAccumulatedValue();
	bool isInhibited();
	void inhibit();
	void disinhibit();
}

#endif
