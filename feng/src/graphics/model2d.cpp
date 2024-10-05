#include "model2d.h"

namespace feng {

	model2d::model2d(std::vector<mesh2d> meshes)
		: model2d_renderer(meshes){
		add_instance(glm::vec3(0));
		setup();
	}

	void model2d::setup() {
		_pos_array_buffer.generate();
		_pos_array_buffer.bind();
		_pos_array_buffer.buffer_data(MAX_NO_MODEL2D_INSTANCES * sizeof(glm::vec3), &_positions[0], GL_DYNAMIC_DRAW);
		
		for (mesh2d& m : _meshes) {
			m.vertex_array.bind();
			_pos_array_buffer.bind();
			m.vertex_array.set_attrib_pointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), 0, 1);
			vertexarray::unbind();
		}
	}

	void model2d::render(shader& shader2d) {
		_pos_array_buffer.bind();
		_pos_array_buffer.buffer_sub_data(0, sizeof(glm::vec3) * _no_instances, &_positions[0]);

		render_meshes(shader2d, _no_instances);
	}

	void model2d::add_instance(glm::vec3 position) {
		_no_instances++;
		_positions.push_back(position);
	}

}