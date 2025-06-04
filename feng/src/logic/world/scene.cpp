#include "scene.h"

#include "../../logging/logging.h"
#include "../../graphics/window.h"

namespace feng {

	void scene::start() {
		_framebuffer_change_sub = window::on_framebuffer_size.subscribe([this](int32_t width, int32_t height) {
				calculate_projection_matrix();
			});

		calculate_projection_matrix();

		for (auto& inst : _instances)
			if (inst.get()->is_active)
				inst.get()->start();
	}

	void scene::update() {
		main_camera.move();

		for (const auto& m : _models)
			m.get()->clear_instances();

		for (auto& inst : _instances)
			if (inst.get()->is_active)
				inst.get()->update();
	}

	void scene::register_model(sptr_mdl model) {
		for (const auto& m : _models) {
			if (m.get()->get_uuid() == model.get()->get_uuid()) {
				LOG_WARNING("Model " + model.get()->get_uuid_string() + " has already been registered!");
				return;
			}
		}
		_models.push_back(model);
	}

	void scene::render_flag(shader& shader, inst_flag_type flag) {
		for (const auto& m : _models)
			m.get()->render_flag(shader, flag);
	}

	void scene::render_models(shader& shader) {
		for (const auto& m : _models)
			m.get()->render(shader);
	}

	void scene::generate_matrices_buffers() {
		_matrices_ssbo.allocate(2 * sizeof(glm::mat4), 1);
	}

	void scene::bind_matrices_ssbo() {
		_matrices_ssbo.start_block();
		_matrices_ssbo.add_element<glm::mat4>((glm::mat4*)glm::value_ptr(_projection));
		_matrices_ssbo.add_element<glm::mat4>((glm::mat4*)glm::value_ptr(main_camera.get_view_matrix()));
		_matrices_ssbo.end_block();
	}

	void scene::calculate_projection_matrix() {
		_projection = glm::perspective(
			glm::radians(60.0f), (float)window::win_width / (float)window::win_height, 0.01f, 100.0f);
	}

	void scene::add_instance(sptr_ins new_instance) {
		_instances.push_back(new_instance);
	}

	sptr_ins scene::copy_instance(sptr_ins instance_to_copy) {
		sptr_ins new_ins = instance_to_copy.get()->copy();
		_instances.push_back(new_ins);
		return new_ins;
	}

	sptr_mdl scene::find_model(uuid_type uuid) {
		for (const auto& model : _models) {
			if (model.get()->get_uuid() == uuid)
				return model;
		}
		THROW_ERROR("No model with uuid: [" + std::to_string(uuid) + "].");
	}

	void scene::generate_lights_buffers() {
		dir_light.generate_buffers();
		for (auto& point_light : point_lights)
			point_light.generate_buffers();

		_lights_ssbo.allocate(ssbo::generate_lights_buffer(), 2);
	}

	void scene::generate_lightspace_matrices() {
		dir_light.generate_lightspace_matrix();
		for (auto& point_light : point_lights)
			point_light.generate_lightspace_matrices();
	}

	void scene::bind_lights_ssbo() {
		int32_t no_spotlights = MAX_SPOT_LIGHTS, no_pointlights = MAX_POINT_LIGHTS;
		_lights_ssbo.start_block();
		_lights_ssbo.add_structure(ssbo::dirlight_buffer_structure, &dir_light);
		_lights_ssbo.add_element(&no_spotlights);
		for(int32_t i = 0; i < no_spotlights; i++)
			_lights_ssbo.add_structure(ssbo::spotlight_buffer_structure, &spot_lights[i]);
		_lights_ssbo.add_element(&no_pointlights);
		for(int32_t i = 0; i < no_pointlights; i++)
			_lights_ssbo.add_structure(ssbo::pointlight_buffer_structure, &point_lights[i]);
		_lights_ssbo.end_block();
	}

	size_t scene::get_free_spot_light_idx() {
		for (size_t i = 0; i < _free_spot_lights.size(); i++) {
			if (!_free_spot_lights.test(i)) {
				_free_spot_lights.set(i);
				return i;
			}
		}
		LOG_WARNING("There are not as many spot lights as expected (max: " + std::to_string(MAX_SPOT_LIGHTS) + ")");
		return -1;
	}

}