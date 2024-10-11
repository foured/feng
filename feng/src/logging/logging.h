#pragma once

#include <string>
#include <chrono>

#define FENG_LOGGIN 1
#define LOG_FUNC_NAME 0

#define WIN_LOGGIN_COLOR_NORMAL 15
#define WIN_LOGGIN_COLOR_DARK_BLUE 9
#define WIN_LOGGIN_COLOR_GREEN 10
#define WIN_LOGGIN_COLOR_BLUE 11
#define WIN_LOGGIN_COLOR_RED 12
#define WIN_LOGGIN_COLOR_PINK 13
#define WIN_LOGGIN_COLOR_YELLOW 14

#if FENG_LOGGIN
	#define LOG_INFO(msg) feng::logger::log_info(msg, __FUNCTION__)
	#define LOG_ACTION(msg) feng::logger::log_action(msg, __FUNCTION__)
	#define LOG_ERROR(msg) feng::logger::log_error(msg, __FUNCTION__)
	#define LOG_WARNING(msg) feng::logger::log_warning(msg, __FUNCTION__)

	#define START_TIMER() feng::timer t(__FUNCTION__)
#else
	#define LOG_INFO(msg)
	#define LOG_ACTION(msg)
	#define LOG_ERROR(msg) 
	#define LOG_WARNING(msg)

	#define START_TIMER(name)
#endif

#define THROW_ERROR(msg) feng::logger::throw_error(msg, __FUNCTION__)

namespace feng {

	class logger {
	public:
		static void log_info(const std::string& msg, const std::string& func_name = "");
		static void log_action(const std::string& msg, const std::string& func_name = "");
		static void log_error(const std::string& msg, const std::string& func_name = "");
		static void log_warning(const std::string& msg, const std::string& func_name = "");
		static void throw_error(const std::string& msg, const std::string& func_name = "");

	private:
		static void log_time();
		static void log(const std::string& msg, const std::string& type_name, char color,
			std::string f_name);
		static void set_color(char color);
	};

	class timer {
	public:
		timer(const char* name);
		~timer();

		void stop();

	private:
		std::string _name;
		std::chrono::time_point<std::chrono::steady_clock> _start_timepoint;
		bool _stopped = false;

	};

}