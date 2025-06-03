#include "instance.h"

#include <type_traits>

#include "../data_management/files.h"
#include "scene.h"

#include "components/model_instance.h"
#include "components/line_animator.h"

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

	void instance::deserialize(data::rfile* file, scene* scene)
	{
		uuid_type uuid;
		file->read_raw(&uuid);
		reset(uuid);
		file->read_raw(&flags._flags);
		file->read_raw(&is_active);
		file->read_raw(&transform);
		size_t components_size;
		file->read_raw(&components_size);
		_components.reserve(components_size);
		for (size_t i = 0; i < components_size; i++) {
			component_list type;
			file->read_raw(&type);
			switch (type) {
			case component_list::model_instance: {
				uuid_type model_uuid;
				file->read_raw(&model_uuid);
				std::shared_ptr<model_instance> mi = std::make_shared<model_instance>(this, scene->find_model(model_uuid));
				//file->read_serializable(mi.get(), scene);
				_components.push_back(mi);
				break;
			}
			case component_list::line_animator: {
				glm::vec3 start;
				glm::vec3 end;
				float speed;
				file->read_raw(&start);
				file->read_raw(&end);
				file->read_raw(&speed);

				std::shared_ptr<line_animator> la = std::make_shared<line_animator>(this, start, end, speed);
				//file->read_serializable(la.get(), scene);
				_components.push_back(la);
				break;
			}
			default: {
				THROW_ERROR("Unregistered componenet type: '" + std::to_string(static_cast<std::underlying_type<component_list>::type>(type)) + "'.");
			}
			}
		}
	}

}