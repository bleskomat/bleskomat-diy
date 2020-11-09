/*
	Copyright (C) 2020 Samotari (Charles Hill, Carlos Garcia Ortiz)

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include "sdcard.h"

namespace {
	const char* mountpoint = "/sdcard";
	bool initialized = false;
	int mount() {

		// https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/peripherals/sdspi_host.html
		sdmmc_host_t host = SDSPI_HOST_DEFAULT();
		sdspi_slot_config_t slot_config = SDSPI_SLOT_CONFIG_DEFAULT();

		slot_config.gpio_miso = (gpio_num_t) SD_MISO;
		slot_config.gpio_mosi = (gpio_num_t) SD_MOSI;
		slot_config.gpio_sck  = (gpio_num_t) SD_SCK;
		slot_config.gpio_cs   = (gpio_num_t) SD_CS;

		// Options for mounting the filesystem.
		// If format_if_mount_failed is set to true, SD card will be partitioned and
		// formatted in case when mounting fails.
		esp_vfs_fat_sdmmc_mount_config_t mount_config =
			{
				.format_if_mount_failed = false,
				.max_files = 5,
				.allocation_unit_size = 16 * 1024
			};

		// Use settings defined above to initialize SD card and mount FAT filesystem.
		// Note: esp_vfs_fat_sdmmc_mount is an all-in-one convenience function.
		// Please check its source code and implement error recovery when developing
		// production applications.
		sdmmc_card_t* card;
		esp_err_t ret = esp_vfs_fat_sdmmc_mount(mountpoint, &host, &slot_config, &mount_config, &card);

		if (ret != ESP_OK) {
			if (ret == ESP_FAIL) {
				logger::write("Failed to mount filesystem, try to format the card.");
			} else {
				logger::write("Failed to initialize the card: " + std::string(esp_err_to_name(ret)));
			}
			return -1;
		}

		// Card has been initialized, print its properties
		sdmmc_card_print_info(stdout, card);

		return 1;
	}

	// void unmount() {
	// 	// All done, unmount partition and disable SDMMC or SPI peripheral
	// 	esp_vfs_fat_sdmmc_unmount();
	// }
}

namespace sdcard {

	bool isReady() {
		return initialized;
	}

	std::string getMountPoint() {
		return std::string(mountpoint);
	}

	void init() {
		if (mount() == 1) {
			logger::write("SD card initialized");
			initialized = true;
		} else {
			logger::write("SD card initialization failed");
		}
	};
}
