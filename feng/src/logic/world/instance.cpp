#include "instance.h"

#include "../data_management/files.h"

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
	
	std::shared_ptr<instance> instance::copy() const {
		std::shared_ptr<instance> new_instance = std::make_shared<instance>();
		instance* ni = new_instance.get();
		ni->transform = transform;
		ni->is_active = is_active;
		ni->flags = flags;
		for (auto& c : _components) {
			ni->_components.push_back(c.get()->copy(ni));
		}
		return new_instance;
	}

	void instance::serialize(data::wfile* file)
	{
		file->write_raw(get_uuid());
		file->write_raw(flags._flags);
		file->write_raw(is_active);
		file->write_raw(transform);
		file->write_raw(_components.size());
		for (const auto& comp : _components)
			file->write_serializable(comp.get());
	}

	void instance::deserialize(data::rfile* file)
	{

	}

}