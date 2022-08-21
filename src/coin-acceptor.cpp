#include "coin-acceptor.h"

namespace {
	std::string coinAcceptorType;
	bool inhibited = false;
}

namespace coinAcceptor {

	void init() {
		coinAcceptorType = config::getString("coinAcceptorType");
		if (coinAcceptorType == "hx616") {
			coinAcceptor_hx616::init();
		} else if (coinAcceptorType == "dg600f") {
			coinAcceptor_dg600f::init();
		}
	}

	void loop() {
		if (coinAcceptorType == "hx616") {
			coinAcceptor_hx616::loop();
		} else if (coinAcceptorType == "dg600f") {
			coinAcceptor_dg600f::loop();
		}
	}

	float getAccumulatedValue() {
		if (coinAcceptorType == "hx616") {
			return coinAcceptor_hx616::getAccumulatedValue();
		} else if (coinAcceptorType == "dg600f") {
			return coinAcceptor_dg600f::getAccumulatedValue();
		}
		return 0.00;
	}

	void resetAccumulatedValue() {
		logger::write("Resetting coin acceptor accumulated value");
		if (coinAcceptorType == "hx616") {
			coinAcceptor_hx616::resetAccumulatedValue();
		} else if (coinAcceptorType == "dg600f") {
			coinAcceptor_dg600f::resetAccumulatedValue();
		}
	}

	bool isInhibited() {
		return inhibited;
	}

	void inhibit() {
		logger::write("Inhibiting coin acceptor");
		if (coinAcceptorType == "hx616") {
			// inhibit() not implemented for HX616
		} else if (coinAcceptorType == "dg600f") {
			coinAcceptor_dg600f::inhibit();
		}
		inhibited = true;
	}

	void disinhibit() {
		logger::write("Disinhibiting coin acceptor");
		if (coinAcceptorType == "hx616") {
			// disinhibit() not implemented for HX616
		} else if (coinAcceptorType == "dg600f") {
			coinAcceptor_dg600f::disinhibit();
		}
		inhibited = false;
	}
}
