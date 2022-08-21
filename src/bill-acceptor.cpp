#include "bill-acceptor.h"

namespace {
	bool inhibited = false;
}

namespace billAcceptor {

	void init() {
		billAcceptor_nv10::init();
	}

	void loop() {
		billAcceptor_nv10::loop();
	}

	float getAccumulatedValue() {
		return billAcceptor_nv10::getAccumulatedValue();
	}

	void resetAccumulatedValue() {
		billAcceptor_nv10::resetAccumulatedValue();
	}

	bool isInhibited() {
		return inhibited;
	}

	void inhibit() {
		logger::write("Inhibiting bill acceptor");
		billAcceptor_nv10::inhibit();
		inhibited = true;
	}

	void disinhibit() {
		logger::write("Disinhibiting bill acceptor");
		billAcceptor_nv10::disinhibit();
		inhibited = false;
	}
}
