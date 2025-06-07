#pragma once

#include <string>
#include <chrono>


#define FENG_LOGGIN
//#define LOG_FUNC_NAME

#define WIN_LOGGIN_COLOR_NORMAL 15
#define WIN_LOGGIN_COLOR_DARK_BLUE 9
#define WIN_LOGGIN_COLOR_GREEN 10
#define WIN_LOGGIN_COLOR_BLUE 11
#define WIN_LOGGIN_COLOR_RED 12
#define WIN_LOGGIN_COLOR_PINK 13
#define WIN_LOGGIN_COLOR_YELLOW 14

#ifdef FENG_LOGGIN
	#define LOG_INFO(msg) feng::logger::log_info(msg, __FUNCTION__, __LINE__)
	#define LOG_ACTION(msg) feng::logger::log_action(msg, __FUNCTION__, __LINE__)
	#define LOG_ERROR(msg) feng::logger::log_error(msg, __FUNCTION__, __LINE__)
	#define LOG_WARNING(msg) feng::logger::log_warning(msg, __FUNCTION__, __LINE__)

	#define START_TIMER() feng::timer t(__FUNCTION__)


	#define GL_CHECK_ERRORS() { \
		GLenum err; \
		while ((err = glGetError()) != GL_NO_ERROR) { \
			LOG_WARNING(std::string("OpenGL error: ") + std::to_string(err) + std::string(" at line ") + std::to_string(__LINE__));\
		} \
	}	

#else
	#define LOG_INFO(msg)
	#define LOG_ACTION(msg)
	#define LOG_ERROR(msg) 
	#define LOG_WARNING(msg)

	#define START_TIMER(name)

	#define GL_CHECK_ERRORS()
#endif

#define THROW_ERROR(msg) feng::logger::throw_error(msg, __FUNCTION__, __LINE__)

namespace feng {

	class logger {
	public:
		static void log_info(const std::string& msg, const std::string& func_name = "", int32_t line = 0);
		static void log_action(const std::string& msg, const std::string& func_name = "", int32_t line = 0);
		static void log_error(const std::string& msg, const std::string& func_name = "", int32_t line = 0);
		static void log_warning(const std::string& msg, const std::string& func_name = "", int32_t line = 0);
		static void throw_error(const std::string& msg, const std::string& func_name = "", int32_t line = 0);
		
	private:
		static void log_time();
		static void log(const std::string& msg, const std::string& type_name, char color,
			std::string f_name, int32_t line);
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