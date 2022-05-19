#ifndef BLESKOMAT_LOGGER_H
#define BLESKOMAT_LOGGER_H

#include <iostream>
#include <string>

namespace logger {
	void write(const std::string &msg, const std::string &type);
	void write(const std::string &msg, const char* type);
	void write(const std::string &msg);
	void write(const char* msg);
	void write(const char* t_msg, const char* type);
}

#endif
