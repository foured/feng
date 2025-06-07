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

	std::tuple<glm::vec3, glm::vec3> utilities::calculate_min_max_light_space(glm::vec3 min, glm::vec3 max,
		const glm::mat4& light_view, const glm::mat4& model) {
		std::vector<glm::vec3> corners = {
			{ min.x, min.y, min.z },
			{ min.x, min.y, max.z },
			{ min.x, max.y, min.z },
			{ min.x, max.y, max.z },
			{ max.x, min.y, min.z },
			{ max.x, min.y, max.z },
			{ max.x, max.y, min.z },
			{ max.x, max.y, max.z }
		};

		glm::vec3 min_ls(FLT_MAX), max_ls(-FLT_MAX);
		for (const auto& c : corners) {
			glm::vec3 light_space_pos = glm::vec3(light_view * model * glm::vec4(c, 1.0f));
			min_ls = glm::min(min_ls, light_space_pos);
			max_ls = glm::max(max_ls, light_space_pos);
		}

		return { min_ls, max_ls };
	}


}