#include "instance.h"

namespace feng {

	void instance::start() {
		for (auto& c : _components) {
			if (c.get()->is_active)
				c.get()->start();
		}
	}

	void instance::update() {
		for (auto& c : _components) {
			if (c.get()->is_active)
				c.get()->update();
		}
	}

}