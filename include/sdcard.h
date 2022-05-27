#ifndef BLESKOMAT_SDCARD_H
#define BLESKOMAT_SDCARD_H

#include "logger.h"

#include <stdio.h>
#include <string.h>
#include <sys/unistd.h>
#include <sys/stat.h>

#include "esp_err.h"
#include "esp_log.h"
#include "esp_vfs_fat.h"
#include "driver/sdmmc_host.h"
#include "driver/sdspi_host.h"
#include "sdmmc_cmd.h"
#include <string>

namespace sdcard {
	bool isMounted();
	std::string getMountedPath(const char* partialPath);
	void init();
}

#endif

