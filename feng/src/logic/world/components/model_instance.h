#pragma once

#include "../component.h"
#include "../../../graphics/model.h"

namespace feng {

	class model_instance : public component {
	public:
		model_instance(instance* instance, model* model);

		void start();
		void update();

	private:
		model* _model;

	};

}