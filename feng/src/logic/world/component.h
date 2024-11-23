#pragma once

namespace feng {
	class instance;

	class component {
	public:
		component(instance* instance);

		bool is_active = true;

		virtual void start() = 0;
		virtual void update() = 0;

		instance* get_instance() const;

	protected:
		instance* _instance;

	};
}