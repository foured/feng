#pragma once

namespace feng::ui {
	class uiinstance;

	class uicomponent {
	public:
		uicomponent(uiinstance* instance);

		bool is_active = true;

		virtual void start() = 0;
		virtual void update() = 0;

		uiinstance* get_instance() const;

	protected:
		uiinstance* _instance;

	};

}