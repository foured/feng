#include "utilities.h"

#include <glm/gtc/matrix_transform.hpp>

#include "../graphics/window.h"

namespace feng {

	namespace util {
		ortho_matrix_setup::ortho_matrix_setup(float _l, float _r, float _b, float _t, float _n, float _f)
			: l(_l), r(_r), b(_b), t(_t), n(_n), f(_f) { }

		ortho_matrix_setup::ortho_matrix_setup(glm::vec3 min, glm::vec3 max) {
			l = min.x;
			r = max.x;
			b = min.y;
			t = max.y;
			n = -max.z;
			f = -min.z;
		}

		void ortho_matrix_setup::scale(float k) {
			float lrc = (l + r) / 2.0f;
			l += (l - lrc) * k;
			r += (r - lrc) * k;

			float btc = (b + t) / 2.0f;
			b += (b - btc) * k;
			t += (t - btc) * k;
		}

		void ortho_matrix_setup::transform_to_square() {
			float lrc = (l + r) / 2.0f;
			float btc = (b + t) / 2.0f;

			float hlrw = std::abs(r - l) / 2.0f;
			float hbtw = std::abs(b - t) / 2.0f;

			float max_hw = std::max(hlrw, hbtw);

			l = lrc - max_hw;
			r = lrc + max_hw;
			b = btc - max_hw;
			t = btc + max_hw;
		}

		glm::mat4 ortho_matrix_setup::setup_matrix() const {
			return glm::ortho(l, r, b, t, n, f);
		}

		// Static -----------------------------------------------------------------------------------------------

		ortho_matrix_setup ortho_matrix_setup::calculate_in_light_space(glm::vec3 min, glm::vec3 max,
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

			return ortho_matrix_setup(min_ls, max_ls);
		}

		ortho_matrix_setup ortho_matrix_setup::overlap_depth(const ortho_matrix_setup& caster, 
			const ortho_matrix_setup& receiver) {
			float n = std::min(caster.n, receiver.n);
			float f = std::max(caster.f, receiver.f);
			return ortho_matrix_setup(caster.l, caster.r, caster.b, caster.t, n, f);
		}

	}

	double utilities::_last_frame_time = 0;
	double utilities::_frame_time = 0;
	double utilities::_delta_time = 0;
	bool utilities::_is_first = true;
	std::unique_ptr<helpers::box_renderer_instanced> utilities::test_octree_visualiser = nullptr;

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