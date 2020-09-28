#include "sdcard.h"

#ifdef USE_SPI_MODE
#define PIN_NUM_MISO SD_MISO
#define PIN_NUM_MOSI SD_MOSI
#define PIN_NUM_CLK  SD_SCK
#define PIN_NUM_CS   SD_CS
#endif //USE_SPI_MODE

namespace {
}

namespace sdcard {

    int open() {
		sdmmc_host_t host = SDSPI_HOST_DEFAULT();
		sdspi_slot_config_t slot_config = SDSPI_SLOT_CONFIG_DEFAULT();

		slot_config.gpio_miso = (gpio_num_t) PIN_NUM_MISO;
		slot_config.gpio_mosi = (gpio_num_t) PIN_NUM_MOSI;
		slot_config.gpio_sck  = (gpio_num_t) PIN_NUM_CLK;
		slot_config.gpio_cs   = (gpio_num_t) PIN_NUM_CS;

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
		esp_err_t ret = esp_vfs_fat_sdmmc_mount("/sdcard", &host, &slot_config, &mount_config, &card);

		if (ret != ESP_OK) {
			if (ret == ESP_FAIL) {
				printf("Failed to mount filesystem, try to format the card.");
			} else {
				printf("Failed to initialize the card (%s). ", esp_err_to_name(ret));
			}
			return -1;
		}

		// Card has been initialized, print its properties
		// sdmmc_card_print_info(stdout, card);

		return 1;
    }

    std::ifstream getIFStream () {
		std::ifstream fpd;
		fpd.open("/sdcard/bleskomat.conf",std::ios::in);

		return fpd;
    }

    void close() {
		// All done, unmount partition and disable SDMMC or SPI peripheral
		esp_vfs_fat_sdmmc_unmount();
        printf("Card unmounted");
    }

}
