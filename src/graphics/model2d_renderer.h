#pragma once

#include <vector>

#include "../fng.h"
#include "mesh2d.h"

namespace feng {

	class model2d_renderer {
	public:
		model2d_renderer(std::vector<mesh2d> meshes);
		virtual ~model2d_renderer() = default;

		virtual void render(shader& shader) = 0;

	protected:
		std::vector<mesh2d> _meshes;

		void render_meshes(shader& shader, uint32_t no_instances);
		virtual void setup() = 0;

	};

}