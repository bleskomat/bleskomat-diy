#include "sdcard.h"

namespace {
	bool mounted = false;
}

namespace sdcard {

	void init() {
		SPI.begin(SD_SCK, SD_MISO, SD_MOSI, SD_CS);
		if (!SD.begin(SD_CS)) {
			std::cout << "Failed to mount SD card" << std::endl;
			return;
		}
		const uint8_t cardType = SD.cardType();
		if (cardType == CARD_NONE) {
			std::cout << "No SD card attached" << std::endl;
			return;
		}
		mounted = true;
		std::cout << "SD Card Mounted" << std::endl;
		std::cout << "SD Card Type: ";
		if (cardType == CARD_MMC) {
			std::cout << "MMC" << std::endl;
		} else if (cardType == CARD_SD) {
			std::cout << "SDSC" << std::endl;
		} else if (cardType == CARD_SDHC) {
			std::cout << "SDHC" << std::endl;
		} else {
			std::cout << "UNKNOWN" << std::endl;
		}
		const uint64_t cardSize = SD.cardSize() / (1024 * 1024);
		std::cout << "SD Card Size: " << std::to_string(cardSize) << " MB" << std::endl;
	}

	bool isMounted() {
		return mounted;
	}
}
