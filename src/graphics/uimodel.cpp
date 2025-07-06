#include "uimodel.h"

#include "../logging/logging.h"
#include "../logic/ui/ui_layer.h"

namespace feng {

	uimodel::uimodel(std::vector<mesh2d> meshes)
		: model2d_renderer(meshes), _rect(meshes) {
		setup();
	}

	void uimodel::setup() {
		_pos_array_buffer.generate();
		_pos_array_buffer.bind();
		_pos_array_buffer.buffer_data<glm::vec3>(MAX_NO_MODEL2D_INSTANCES * sizeof(glm::vec3), nullptr, GL_DYNAMIC_DRAW);

		_size_array_buffer.generate();
		_size_array_buffer.bind();
		_pos_array_buffer.buffer_data<glm::vec2>(MAX_NO_MODEL2D_INSTANCES * sizeof(glm::vec2), nullptr, GL_DYNAMIC_DRAW);


		for (mesh2d& m : _meshes) {
			m.vertex_array.bind();
			_pos_array_buffer.bind();
			m.vertex_array.set_attrib_pointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), 0, 1);
			_size_array_buffer.bind();
			m.vertex_array.set_attrib_pointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), 0, 1);
			vertexarray::unbind();
		}
	}

	void uimodel::render(shader& shader2d) {
		_gen_arrays_data();
		size_t no_instances = _positions.size();
		if (_positions.size() > 0) {
			_pos_array_buffer.bind();
			_pos_array_buffer.buffer_sub_data(0, sizeof(glm::vec3) * no_instances, &_positions[0]);
			_size_array_buffer.bind();
			_size_array_buffer.buffer_sub_data(0, sizeof(glm::vec2) * no_instances, &_sizes[0]);

			render_meshes(shader2d, no_instances);
		}
	}

	std::shared_ptr<ui::uiinstance> uimodel::add_instance(
		ui::ui_layer* layer, glm::vec2 pos_in_ndc, glm::vec2 size) {
		std::shared_ptr<ui::uiinstance> sptr_inst = std::make_shared<ui::uiinstance>(this, layer);
		sptr_inst->uitransform.set_pos_ndc(pos_in_ndc);
		sptr_inst->uitransform.set_size(size);

		_instances.emplace_back(sptr_inst);

		return sptr_inst;
	}

	void uimodel::_gen_arrays_data() {
		_positions.clear();
		_sizes.clear();

		for (const auto& ptr_i : _instances) {
			if (ptr_i->is_active && ptr_i->is_layer_active()) {
				_positions.emplace_back(ptr_i->uitransform.get_pos_ndc(), ptr_i->calculate_z());
				_sizes.emplace_back(ptr_i->uitransform.get_size_ndc());
			}
		}
	}

	ui::uibounds_rect uimodel::rect() {
		return _rect;
	}

}