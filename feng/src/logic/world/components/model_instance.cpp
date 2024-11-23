#include "model_instance.h"

#include "../instance.h"

namespace feng {

	model_instance::model_instance(instance* instance, model* model)
		: component(instance), _model(model) {}

	void model_instance::start() {

	}

	void model_instance::update() {
		_model->add_instance(
			_instance->transform.get_position(),
			_instance->transform.get_size(),
			_instance->transform.get_rotation());
	}

}