#pragma once

#include <glm/glm.hpp>

#include <string>
#include <memory>

#include "../graphics/helpers/box_renderer.h"

namespace feng {

	namespace util {
		struct ortho_matrix_setup {
			float l = 0, r = 0, b = 0, t = 0, n = 0, f = 0;

			ortho_matrix_setup(float _l, float _r, float _b, float _t, float _n, float _f);
			ortho_matrix_setup(glm::vec3 min, glm::vec3 max);

			void scale(float k);
			void transform_to_square();
			glm::mat4 setup_matrix() const;

			static ortho_matrix_setup calculate_in_light_space(glm::vec3 min, glm::vec3 max,
				const glm::mat4& light_view);

			static ortho_matrix_setup overlap_depth(const ortho_matrix_setup& caster, const ortho_matrix_setup& receiver);
		};
	}

	class utilities {
	public:
		static std::unique_ptr<helpers::box_renderer_instanced> test_octree_visualiser;

		static double delta_time();
		static void update_delta_time();
		
		static glm::vec2 pixel_to_ndc(glm::vec2 pixel_pos);
		static glm::vec2 ndc_to_pixel(glm::vec2 ndc_pos);

		static int32_t round_to(int32_t val, int32_t align);
		static bool compare_floats(float v1, float v2, float epsilon = FLT_EPSILON);
		// no direction comparison
		static bool compare_normals_nd(const glm::vec3& n1, const glm::vec3& n2, float epsilon = FLT_EPSILON);

		static std::string strip(const std::string& str);

		static glm::mat3 euler2mat3x3(const glm::vec3& deg);
		static glm::mat4 euler2mat4x4(const glm::vec3& deg);
		static glm::vec3 mul(const glm::mat4& m, const glm::vec3& p);

	private:
		static double _last_frame_time, _frame_time, _delta_time;
		static bool _is_first;

	};

}