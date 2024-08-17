#pragma once

#include <vector>

#include "mesh2d.h"

#define MAX_NO_MODEL2D_INSTANCES 10

namespace feng {

	class model2d_renderer {
	public:
		model2d_renderer(std::vector<mesh2d> meshes);

		virtual void render(shader& shader) = 0;

	protected:
		std::vector<mesh2d> _meshes;

		void render_meshes(shader& shader, uint32_t no_instances);
		virtual void setup() = 0;

	};

}