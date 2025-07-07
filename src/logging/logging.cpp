#include "logging.h"

#include <glad/glad.h>

#include <iostream>
#include <ctime>
#include <iomanip>
#include <stdexcept>

#ifdef _WIN32
	#include "Windows.h"
#endif

#pragma warning(disable : 4996)

namespace feng {

	void logger::log_action(const std::string& msg, const std::string& func_name, int32_t line) {
		log(msg, "ACTION", log_color::green, func_name, line);
	}

	void logger::log_info(const std::string& msg, const std::string& func_name, int32_t line) {
		log(msg, "INFO", log_color::blue, func_name, line);
	}

	void logger::log_error(const std::string& msg, const std::string& func_name, int32_t line) {
		log(msg, "ERROR", log_color::red, func_name, line);
	}

	void logger::log_warning(const std::string& msg, const std::string& func_name, int32_t line) {
		log(msg, "WARNING", log_color::pink, func_name, line);
	}

	void logger::log_time() {
		auto now = std::chrono::system_clock::now();
		std::time_t now_c = std::chrono::system_clock::to_time_t(now);
		std::tm now_tm = *localtime(&now_c);
		std::cout << '[';
		set_color(log_color::yellow);
		std::cout << std::put_time(&now_tm, "%H:%M:%S");
		set_color(log_color::normal);
		std::cout << ']';
	}

	void logger::throw_error(const std::string& msg, const std::string& func_name, int32_t line) {
		log_error(msg, func_name, line);
		throw std::runtime_error(msg.c_str());
	}

	void logger::call_assert(const std::string& msg, const std::string& func_name, int32_t line) {
		log(msg, "ASSERT", log_color::yellow, func_name, line);
		std::abort();
	}

	std::string logger::get_error_string(uint32_t err) {
		switch (err) {
		case GL_NO_ERROR:								
			return "GL_NO_ERROR";
		case GL_INVALID_ENUM:							
			return "GL_INVALID_ENUM";
		case GL_INVALID_VALUE:							
			return "GL_INVALID_VALUE";
		case GL_INVALID_OPERATION:						
			return "GL_INVALID_OPERATION";
		case GL_STACK_OVERFLOW:							
			return "GL_STACK_OVERFLOW";
		case GL_STACK_UNDERFLOW:						
			return "GL_STACK_UNDERFLOW";
		case GL_OUT_OF_MEMORY:							
			return "GL_OUT_OF_MEMORY";
		case GL_INVALID_FRAMEBUFFER_OPERATION:			
			return "GL_INVALID_FRAMEBUFFER_OPERATION";
		default:                   
			return "Unknown OpenGL error: " + std::to_string(err);
		}
	}

	void logger::log(const std::string& msg, const std::string& type_name, log_color color, std::string func_name, int32_t line) {
		log_time();
		std::cout << " | [";
		set_color(color);
		std::cout << type_name;
#ifdef LOG_FUNC_NAME
		set_color(WIN_LOGGIN_COLOR_NORMAL);
		std::cout << "] | [";
		set_color(WIN_LOGGIN_COLOR_DARK_BLUE);
		std::cout << func_name << " (" << line << ")";
#endif
		set_color(log_color::normal);
		std::cout << "] -> " << msg << std::endl;
	}

	void logger::set_color(log_color color) {
#ifdef _WIN32
		char col = WIN_LOGGIN_COLOR_NORMAL;

		switch(color){
		case log_color::normal:
			col = WIN_LOGGIN_COLOR_NORMAL;
			break;
		case log_color::dark_blue:
			col = WIN_LOGGIN_COLOR_DARK_BLUE;
			break;
		case log_color::green:
			col = WIN_LOGGIN_COLOR_GREEN;
			break;
		case log_color::blue:
			col = WIN_LOGGIN_COLOR_BLUE;
			break;
		case log_color::red:
			col = WIN_LOGGIN_COLOR_RED;
			break;
		case log_color::pink:
			col = WIN_LOGGIN_COLOR_PINK;
			break;
		case log_color::yellow:
			col = WIN_LOGGIN_COLOR_YELLOW;
			break;
		}

		HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
		SetConsoleTextAttribute(hConsole, col); // _WIN32
#else
		const char* col = TERM_COLOR_NORMAL;

		switch(color){
		case log_color::normal:
			col = TERM_COLOR_NORMAL;
			break;
		case log_color::dark_blue:
			col = TERM_COLOR_DARK_BLUE;
			break;
		case log_color::green:
			col = TERM_COLOR_GREEN;
			break;
		case log_color::blue:
			col = TERM_COLOR_BLUE;
			break;
		case log_color::red:
			col = TERM_COLOR_RED;
			break;
		case log_color::pink:
			col = TERM_COLOR_PINK;
			break;
		case log_color::yellow:
			col = TERM_COLOR_YELLOW;
			break;
		}

		std::cout << col;
#endif
	}

	timer::timer(const char* name)
		: _name(name) {
		_start_timepoint = std::chrono::high_resolution_clock::now();

		int16_t count = -1;
		int32_t dc_pos = 0;
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
		int64_t start = std::chrono::time_point_cast<std::chrono::microseconds>(_start_timepoint)
			.time_since_epoch().count();
		int64_t end = std::chrono::time_point_cast<std::chrono::microseconds>(end_timepoint)
			.time_since_epoch().count();

		LOG_INFO(_name, ": ", (end - start) / 1000.0f, "ms");

		_stopped = true;
	}

	timer::~timer() {
		if (!_stopped)
			stop();
	}

}
