#pragma once

#include <glm/glm.hpp>
#include <string>

namespace feng {

	class utilities {
	public:
		static double delta_time();
		static void update_delta_time();
		
		static glm::vec2 pixel_to_ndc(glm::vec2 pixel_pos);
		static glm::vec2 ndc_to_pixel(glm::vec2 ndc_pos);

		static int32_t round_to(int32_t val, int32_t align);

		static std::string strip(const std::string& str);

		static std::tuple<glm::vec3, glm::vec3> calculate_min_max_light_space(glm::vec3 min, glm::vec3 max,
			const glm::mat4& light_view, const glm::mat4& model = glm::mat4(1.0f));

	private:
		static double _last_frame_time, _frame_time, _delta_time;
		static bool _is_first;

	};

}