#include "modules.h"

namespace modules {

	void init() {
		#ifdef COIN_ACCEPTOR
			coinAcceptor::init();
			logger::write("Coin Reader OK");
		#endif
		#ifdef BUTTON
			button::init();
			logger::write("Button OK");
		#endif
		screen::init();
		logger::write("Screen OK");
	}

	void loop() {
		#ifdef COIN_ACCEPTOR
			coinAcceptor::loop();
		#endif
		#ifdef BUTTON
			button::loop();
		#endif
	}
}
