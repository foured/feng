#include "uicomponent.h"

#include "uiinstance.h"

namespace feng::ui {
	
	uicomponent::uicomponent(uiinstance* instance)
		: _instance(instance) {}

	uiinstance* uicomponent::get_instance() const {
		return _instance;
	}
}