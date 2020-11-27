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

#ifndef BLESKOMAT_SDCARD_H
#define BLESKOMAT_SDCARD_H

#ifndef SD_CS
#define SD_CS 5
#endif

#ifndef SD_MOSI
#define SD_MOSI 23
#endif

#ifndef SD_MISO
#define SD_MISO 18
#endif

#ifndef SD_SCK
#define SD_SCK 19
#endif

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

namespace sdcard {
	bool isReady();
	std::string getMountPoint();
	void init();
}

#endif
