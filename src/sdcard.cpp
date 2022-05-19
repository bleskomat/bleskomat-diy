#include "sdcard.h"

namespace {

	const char* mountpoint = "/sdcard";
	bool mounted = false;

	bool mount() {
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
		esp_err_t result = esp_vfs_fat_sdmmc_mount(mountpoint, &host, &slot_config, &mount_config, &card);
		if (result != ESP_OK) {
			if (result == ESP_FAIL) {
				logger::write("Failed to mount SD card: ESP_FAIL (Partition cannot be mounted)");
			} else {
				logger::write("Failed to mount SD card: " + std::string(esp_err_to_name(result)));
			}
			return false;
		}
		// Card has been initialized, print its properties.
		sdmmc_card_print_info(stdout, card);
		return true;
	}

	// void unmount() {
	// 	// All done, unmount partition and disable SDMMC or SPI peripheral
	// 	esp_vfs_fat_sdmmc_unmount();
	// }
}

namespace sdcard {

	bool isMounted() {
		return mounted;
	}

	std::string getMountedPath(const char* partialPath) {
		return std::string(mountpoint) + "/" + std::string(partialPath);
	}

	void init() {
		if (mount()) {
			logger::write("SD card mounted");
			mounted = true;
		}
	}
}
