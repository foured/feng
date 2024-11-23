#pragma once

#include <memory>
#include <vector>

#include "../../graphics/model.h"
#include "../../graphics/shader.h"
#include "instance.h"

namespace feng {

	class scene {
	public:
		using sptr_mdl = std::shared_ptr<model>;
		using sptr_ins = std::shared_ptr<instance>;

		void start();
		void update();

		void register_model(sptr_mdl model);
		void direct_render_flag(shader& shader, inst_flag_type flag);
		void add_instance(sptr_ins new_instance);

		template<typename... Args>
		typename std::enable_if_t<std::is_constructible_v<instance, Args...>, sptr_ins>
		add_instance(Args... args) {
			sptr_ins new_instance = std::make_shared<instance>(std::forward<Args>(args)...);
			_instances.emplace_back(new_instance);
			return new_instance;
		}

	private:
		std::vector<sptr_mdl> _models;
		std::vector<sptr_ins> _instances;
	};

}