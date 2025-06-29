#include "bounds_updater_context.h"

#include "../world/instance.h"

namespace feng {

	bounds_updater_context::bounds_updater_context()
		: _autonomy(autonomy::independent) { }


	bounds_updater_context::autonomy bounds_updater_context::get_autonomy() const {
		return _autonomy;
	}

	bounds_updater_context::bounds_updater_context(autonomy aut) 
		: _autonomy(aut) { }

	aabb bounds_updater_context::calculate_bounds() const {
		aabb base = get_base();
		return base.scale(get_size()).fit_rotation(get_rotation()).offset(get_position());
	}

	// ---- bounds_updater_context_dependent ----


	bounds_updater_context_dependent::bounds_updater_context_dependent() 
		: bounds_updater_context(autonomy::dependent) {	}

	aabb bounds_updater_context_dependent::get_base() const {
		if (_independent.expired()) {
			THROW_ERROR("Independent bounds_updater_context expired");
		}
		return _independent.lock()->get_base();
	}

	aabb bounds_updater_context_dependent::get_independent_bounds() const {
		if (_independent.expired()) {
			THROW_ERROR("Independent bounds_updater_context expired");
		}

		return _independent.lock()->calculate_bounds();
	}

	void bounds_updater_context_dependent::find_independent(instance* inst) {
		using spc = std::shared_ptr<bounds_updater_context>;

		std::vector<spc> contexts
			= inst->try_find_components_of_type<bounds_updater_context>();

		FENG_ASSERT(contexts.size() > 1, "Cant find bounds updater contexts");

		auto it = std::find_if(contexts.begin(), contexts.end(), [](spc context) {
				return context->get_autonomy() == autonomy::independent;
			});
			
		FENG_ASSERT(it != contexts.end(), "Cant find independent context");

		_independent = *it;
	}

}