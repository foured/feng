#include "model2d_renderer.h"

namespace feng {

	model2d_renderer::model2d_renderer(std::vector<mesh2d> meshes)
		: _meshes(meshes) { }

	void model2d_renderer::render_meshes(shader& shader, uint32_t no_instances) {
		for (mesh2d& m : _meshes) {
			m.render(shader, no_instances);
		}
	}

}