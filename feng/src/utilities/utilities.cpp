#include "utilities.h"

#include "../graphics/window.h"

namespace feng {

	double utilities::_last_frame_time = 0;
	double utilities::_frame_time = 0;
	double utilities::_delta_time = 0;
	bool utilities::_is_first = true;


	double utilities::delta_time() {
		return _delta_time;
	}

	void utilities::update_delta_time() {
		if (_is_first) {
			_last_frame_time = glfwGetTime();
			_is_first = false;
		}
		else {
			_last_frame_time = _frame_time;
		}
		_frame_time = glfwGetTime();
		_delta_time = _frame_time - _last_frame_time;
	}

	glm::vec2 utilities::pixel_to_ndc(glm::vec2 pixel_pos) {
		return glm::vec2(pixel_pos.x / window::win_width, pixel_pos.y / window::win_height);
	}

	glm::vec2 utilities::ndc_to_pixel(glm::vec2 ndc_pos) {
		return glm::vec2(ndc_pos.x * window::win_width, ndc_pos.y * window::win_height);
	}

	int32_t utilities::round_to(int32_t val, int32_t align) {
		if (val % align == 0)
			return val;
		return (val / align + 1) * align;
	}

	std::string utilities::strip(const std::string& str) {
		size_t start = str.find_first_not_of(' ');
		if (start == std::string::npos) {
			return "";
		}

		size_t end = str.find_last_not_of(' ');
		return str.substr(start, end - start + 1);
	}


}