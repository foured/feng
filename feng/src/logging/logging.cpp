#include "logging.h"

#include <iostream>
#include <ctime>
#include <iomanip>
#include <exception>

#pragma warning(disable : 4996)

namespace feng {

	void logger::log_action(const std::string& msg, const std::string& func_name) {
		log(msg, "ACTION", func_name);
	}

	void logger::log_info(const std::string& msg, const std::string& func_name) {
		log(msg, "INFO", func_name);
	}

	void logger::log_error(const std::string& msg, const std::string& func_name) {
		log(msg, "ERROR", func_name);
	}

	void logger::log_warning(const std::string& msg, const std::string& func_name) {
		log(msg, "WARNING", func_name);
	}

	void logger::log_time() {
		auto now = std::chrono::system_clock::now();
		std::time_t now_c = std::chrono::system_clock::to_time_t(now);
		std::tm now_tm = *localtime(&now_c);
		std::cout << '[';
		std::cout << std::put_time(&now_tm, "%H:%M:%S");
		std::cout << ']';
	}

	void logger::throw_error(const std::string& msg, const std::string& func_name) {
		log_error(msg, func_name);
		throw std::exception(msg.c_str());
	}

	void logger::log(const std::string& msg, const std::string& type_name, std::string func_name) {
		log_time();
		std::cout << " | [";
		std::cout << type_name;
#if LOG_FUNC_NAME
		std::cout << "] | [";
		std::cout << func_name;
#endif
		std::cout << "] -> " << msg << std::endl;
	}

	timer::timer(const char* name)
		: _name(name) {
		_start_timepoint = std::chrono::high_resolution_clock::now();

		int16_t count = -1;
		int32_t dc_pos;
		for (int i = _name.size(); i >= 0; --i) {
			if (_name[i] == ':') {
				count++;
				if (count == 2) {
					dc_pos = i;
					break;
				}
			}
		}

		if (count != -1) {
			_name = _name.substr(dc_pos + 1, _name.size() - dc_pos - 1);
		}
	}

	void timer::stop() {
		auto end_timepoint = std::chrono::high_resolution_clock::now();
		int64_t start = std::chrono::time_point_cast<std::chrono::milliseconds>(_start_timepoint)
			.time_since_epoch().count();
		int64_t end = std::chrono::time_point_cast<std::chrono::milliseconds>(end_timepoint)
			.time_since_epoch().count();

		logger::log_info(_name + " : " + std::to_string(end - start) + "ms");

		_stopped = true;
	}

	timer::~timer() {
		if (!_stopped)
			stop();
	}

}