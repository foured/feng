#pragma once

#include <string>
#include <chrono>
#include <sstream>

#define FENG_DEBUG
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
	#define LOG_INFO(...) feng::logger::log_info(feng::logger::gen_string(__VA_ARGS__), __FUNCTION__, __LINE__)
	#define LOG_ACTION(...) feng::logger::log_action(feng::logger::gen_string(__VA_ARGS__), __FUNCTION__, __LINE__)
	#define LOG_ERROR(...) feng::logger::log_error(feng::logger::gen_string(__VA_ARGS__), __FUNCTION__, __LINE__)
	#define LOG_WARNING(...) feng::logger::log_warning(feng::logger::gen_string(__VA_ARGS__), __FUNCTION__, __LINE__)

	#define START_TIMER() feng::timer t(__FUNCTION__)


	#define GL_CHECK_ERRORS() { \
		GLenum err; \
		while ((err = glGetError()) != GL_NO_ERROR) { \
			LOG_WARNING(std::string("OpenGL error: ") + feng::logger::get_error_string(err) + std::string(" at line ") + std::to_string(__LINE__));\
		} \
	}	

#else
	#define LOG_INFO(...)
	#define LOG_ACTION(...)
	#define LOG_ERROR(...) 
	#define LOG_WARNING(...)

	#define START_TIMER(name)

	#define GL_CHECK_ERRORS()
#endif

#define THROW_ERROR(...) feng::logger::throw_error(feng::logger::gen_string(__VA_ARGS__), __FUNCTION__, __LINE__)
#define FENG_ASSERT(condition, ...) \
    do { \
        if (!(condition)) { \
            feng::logger::call_assert(feng::logger::gen_string(__VA_ARGS__), __FUNCTION__, __LINE__); \
        } \
    } while(0)
										
namespace feng {

	class logger {
	public:
		static void log_info(const std::string& msg, const std::string& func_name = "", int32_t line = 0);
		static void log_action(const std::string& msg, const std::string& func_name = "", int32_t line = 0);
		static void log_error(const std::string& msg, const std::string& func_name = "", int32_t line = 0);
		static void log_warning(const std::string& msg, const std::string& func_name = "", int32_t line = 0);
		static void throw_error(const std::string& msg, const std::string& func_name = "", int32_t line = 0);
		static void call_assert(const std::string& msg, const std::string& func_name = "", int32_t line = 0);

		static std::string get_error_string(uint32_t err);

		template<typename... Ts>
		static std::string gen_string(Ts&&... msgs) {
			std::ostringstream oss;
			(oss << ... << std::forward<Ts>(msgs));
			return oss.str();
		}

		static void log(const std::string& msg, const std::string& type_name, char color,
			std::string f_name, int32_t line);

	private:
		static void log_time();
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


#include <glm/glm.hpp>
#include <ostream>

inline std::ostream& operator<<(std::ostream& os, const glm::vec3& v) {
	os << "vec3(" << v.x << ", " << v.y << ", " << v.z << ")";
	return os;
}

inline std::ostream& operator<<(std::ostream& os, const glm::vec4& v) {
	os << "vec4(" << v.x << ", " << v.y << ", " << v.z << ", " << v.w << ")";
	return os;
}
