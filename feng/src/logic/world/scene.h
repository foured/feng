#pragma once

#include <memory>
#include <vector>

#include "../../graphics/model.h"
#include "../../graphics/shader.h"
#include "instance.h"
#include "../../utilities/uuid.hpp"

namespace feng {
	
	using sptr_mdl = std::shared_ptr<model>;
	using sptr_ins = std::shared_ptr<instance>;

	class scene : public util::uuid_owner {
	public:
		void start();
		void update();

		void register_model(sptr_mdl model);
		
		template<typename... Args>
		typename std::enable_if_t<std::is_constructible_v<model, Args...>, sptr_mdl>
		register_model(Args... args) {
			sptr_mdl new_model = std::make_shared<model>(std::forward<Args>(args)...);
			_models.emplace_back(new_model);
			return new_model;
		}

		void render_flag(shader& shader, inst_flag_type flag);
		void render_models(shader& shader);
		void add_instance(sptr_ins new_instance);

		template<typename... Args>
		typename std::enable_if_t<std::is_constructible_v<instance, Args...>, sptr_ins>
		add_instance(Args... args) {
			sptr_ins new_instance = std::make_shared<instance>(std::forward<Args>(args)...);
			_instances.emplace_back(new_instance);
			return new_instance;
		}

		sptr_ins copy_instance(sptr_ins instance_to_copy);


	private:
		std::vector<sptr_mdl> _models;
		std::vector<sptr_ins> _instances;

		friend class scene_saver;
	};

}