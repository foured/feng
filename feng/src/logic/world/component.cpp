#include "component.h"
#include "instance.h"

#include "scene.h"

namespace feng {

	component::component(instance* instance) 
		: _instance(instance) {	}


	instance* component::get_instance() const {
		return _instance;
	}

}