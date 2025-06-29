#include "component.h"
#include "instance.h"

#include "scene.h"

namespace feng {

	component::component(instance* instance) 
		: _instance(instance) {	}

	void component::late_start() {}

	instance* component::get_instance() const {
		return _instance;
	}

	uuid_type component::get_instance_uuid() const {
		return _instance->get_uuid();
	}

	std::string component::get_instance_uuid_string() const {
		return _instance->get_uuid_string();
	}

}