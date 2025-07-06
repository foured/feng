#pragma once

#include <vector>

#include "model2d_renderer.h"

namespace feng {
	class model2d : public model2d_renderer {
	public:
		model2d(std::vector<mesh2d> meshes);

		void render(shader& shader2d);
		void add_instance(glm::vec3 position);

	private:
		uint32_t _no_instances = 0;
		std::vector<glm::vec3> _positions;
		arraybuffer _pos_array_buffer;

		void setup();
	};
}