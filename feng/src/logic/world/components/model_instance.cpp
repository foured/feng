#include "model_instance.h"

#include "../instance.h"

namespace feng {

	model_instance::model_instance(instance* instance, std::shared_ptr<model> model)
		: component(instance), _model(model) {}

	void model_instance::start() {

	}

	void model_instance::update() {
		_model.get()->add_instance(_instance);
	}

	std::shared_ptr<component> model_instance::copy(instance* new_instance) {
		return std::make_shared<model_instance>(new_instance, _model);
	}

}