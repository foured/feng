#pragma once

#include <memory>

namespace feng {
	class instance;

	class component {
	public:
		component(instance* instance);

		bool is_active = true;

		virtual void start() = 0;
		virtual void update() = 0;
		virtual std::shared_ptr<component> copy(instance* new_instance) = 0;

		instance* get_instance() const;
	protected:
		instance* _instance;

		friend class instance;

	};
}