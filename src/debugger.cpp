#include "debugger.h"

namespace {

	unsigned long lastHeapSizePrintTime = 0;
	const unsigned int heapSizePrintDelay = 5000;
}

namespace debugger {

	void loop() {
		if (lastHeapSizePrintTime == 0 || millis() - lastHeapSizePrintTime > heapSizePrintDelay) {
			heap_caps_print_heap_info(MALLOC_CAP_DEFAULT);
			lastHeapSizePrintTime = millis();
		}
	}
}
