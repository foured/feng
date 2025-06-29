#include "scene.h"

#include "../../logging/logging.h"
#include "../../graphics/window.h"
#include "components/model_instance.h"

namespace feng {

	scene::scene() 
		: _octree(glm::vec3(23), 210) { }

	void scene::start() {
		_framebuffer_change_sub = window::on_framebuffer_size.subscribe([this](int32_t width, int32_t height) {
				calculate_projection_matrix();
			});

		calculate_projection_matrix();

		for (auto& inst : _instances) {
			if (inst.get()->is_active) {
				inst.get()->start();
			}
		}

		for (auto& inst : _instances) {
			if (inst.get()->is_active) {
				inst.get()->late_start();
			}
		}

		for (auto& inst : _instances) {
			std::shared_ptr<simple_collider> collider = inst->try_get_component<simple_collider>();
			if (collider) {
				if (!_octree.add_insance(collider)) {
					LOG_WARNING("Instance ", inst->get_uuid_string(), " is out of octree bounds");
				}
			}
		}
	}

	void scene::update() {
		main_camera.move();

		for (const auto& m : _models)
			m.get()->clear_instances();

		for (auto& inst : _instances) {
			if (inst.get()->is_active)
				inst.get()->update();
		}

		_octree.update();
		_octree.process_left_objects();
		_octree.check_collisions();
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

	glm::mat4 scene::get_projection_matrix() const {
		return _projection;
	}

	void scene::add_instance(sptr_ins new_instance) {
		_instances.push_back(new_instance);
	}

	sptr_ins scene::copy_instance(sptr_ins instance_to_copy) {
		sptr_ins new_ins = instance_to_copy.get()->copy();
		_instances.push_back(new_ins);
		return new_ins;
	}

	sptr_ins scene::get_instance(uuid_type uuid) {
		for (const auto& instance : _instances) {
			if (instance->get_uuid() == uuid)
				return instance;
		}
		THROW_ERROR("No instance with uuid: [" + std::to_string(uuid) + "].");
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
		//dir_light.generate_lightspace_matrix();
		calculate_bounds();
		dir_light.lightspace_matrix = dir_light.generate_custom_lightspace_matrix(_bounds);
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

	// OCTREE----------------------------------------------------------------------------------------------------------

	uint32_t scene::find_in_octree(std::shared_ptr<instance> instance) {
		auto sim_col = instance->try_get_component<simple_collider>();
		if (sim_col) {
			return _octree.find_instance(sim_col);
		}
		return 0;
	}

	void scene::calculate_bounds() {
		_bounds.set_numeric_limits();
		transform* instance_transform = nullptr;
		aabb* model_bounds = nullptr;

		//int32_t i = 0;
		for (const auto& instance : _instances) {
			std::shared_ptr<model_instance> model = instance->try_get_component<model_instance>();
			if (model && instance->flags.get(INST_FLAG_STATIC) && instance->flags.get(INST_FLAG_CAST_SHADOWS)) {
				instance_transform = &instance->transform;
				model_bounds = &model->get_model()->bounds;
				glm::vec3 max = model_bounds->max * instance_transform->get_size() + instance_transform->get_position();
				glm::vec3 min = model_bounds->min * instance_transform->get_size() + instance_transform->get_position();

				_bounds.max = glm::max(_bounds.max, max);
				_bounds.min = glm::min(_bounds.min, min);
			}
		}
	}

	aabb scene::get_bounds() const{
		return _bounds;
	}

}