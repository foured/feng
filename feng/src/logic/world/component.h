#pragma once

#include <memory>
#include "../../utilities/uuid.hpp"
#include "../data_management/serializable.h"

enum class component_list : uint32_t {
	model_instance = 0,
	line_animator,
	flash_light
};

namespace feng {
	class instance;

	class component : public util::uuid_owner, public data::serializable {
	public:
		component(instance* instance);

		bool is_active = true;

		virtual void start() = 0;
		virtual void update() = 0;
		virtual std::shared_ptr<component> copy(instance* new_instance) = 0;

		instance* get_instance() const;

		virtual void serialize(data::wfile* file) = 0;
		virtual void deserialize(data::rfile* file, scene* scene) = 0;

	protected:
		instance* _instance;

		friend class instance;

	};
}