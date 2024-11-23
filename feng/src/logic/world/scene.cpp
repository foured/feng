#include "scene.h"

#include "../../logging/logging.h"

namespace feng {

	void scene::start() {
		for (auto& inst : _instances)
			if (inst.get()->is_active)
				inst.get()->start();
	}

	void scene::update() {
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

	void scene::direct_render_flag(shader& shader, inst_flag_type flag) {
		for (auto m : _models)
			m.get()->render_flag(shader, flag);
	}

	void scene::add_instance(sptr_ins new_instance) {
		_instances.push_back(new_instance);
	}


}