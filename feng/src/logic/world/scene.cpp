#include "scene.h"

#include "../../logging/logging.h"

namespace feng {

	void scene::start() {
		for (auto& inst : _instances)
			if (inst.get()->is_active)
				inst.get()->start();
	}

	void scene::update() {
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

	void scene::add_instance(sptr_ins new_instance) {
		_instances.push_back(new_instance);
	}

	sptr_ins scene::copy_instance(sptr_ins instance_to_copy) {
		sptr_ins new_ins = instance_to_copy.get()->copy();
		_instances.push_back(new_ins);
		return new_ins;
	}

}