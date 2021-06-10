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

#ifndef BLESKOMAT_MAIN_H
#define BLESKOMAT_MAIN_H

#include "config.h"
#include "logger.h"
#include "modules.h"
#include "util.h"

#include <lnurl.h>
#include <string>

#define STRINGIFY(s) STRINGIFY1(s)
#define STRINGIFY1(s) #s

#ifndef FIRMWARE_COMMIT_HASH
	#error "Missing required build flag: FIRMWARE_COMMIT_HASH"
#endif

namespace {
	std::string trimQuotes(const std::string &str) {
		return str.substr(1, str.length() - 2);
	}
}

const std::string firmwareCommitHash(trimQuotes(STRINGIFY(FIRMWARE_COMMIT_HASH)));

#endif
