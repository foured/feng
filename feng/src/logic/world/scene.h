#pragma once

#include <memory>
#include <vector>
#include <array>
#include <bitset>

#include "../../graphics/model.h"
#include "../../graphics/shader.h"
#include "instance.h"
#include "../../utilities/uuid.hpp"
#include "../../graphics/light/lights.h"
#include "../../graphics/gl_buffers/ssbo.hpp"
#include "../../io/camera.h"

namespace feng {
	
	namespace data {
		class scene_serializer;
	}

	namespace editor {
		class lightbaker;
	}

	using sptr_mdl = std::shared_ptr<model>;
	using sptr_ins = std::shared_ptr<instance>;

	class scene : public util::uuid_owner {
	public:

		dir_light dir_light;
		std::array<point_light, MAX_POINT_LIGHTS> point_lights;
		std::array<spot_light, MAX_SPOT_LIGHTS> spot_lights;

		camera main_camera;

		//==================
		// USUAL
		//==================
		void start();
		void update();

		//==================
		// MODELS
		//==================
		sptr_mdl find_model(uuid_type uuid);
		void register_model(sptr_mdl model);
		
		template<typename... Args>
		typename std::enable_if_t<std::is_constructible_v<model, Args...>, sptr_mdl>
		register_model(Args... args) {
			sptr_mdl new_model = std::make_shared<model>(std::forward<Args>(args)...);
			_models.emplace_back(new_model);
			return new_model;
		}

		//=====================
		// INSTANCES
		//=====================
		void add_instance(sptr_ins new_instance);
		
		template<typename... Args>
		typename std::enable_if_t<std::is_constructible_v<instance, Args...>, sptr_ins>
			add_instance(Args... args) {
			sptr_ins new_instance = std::make_shared<instance>(std::forward<Args>(args)...);
			_instances.push_back(new_instance);
			return new_instance;
		}

		sptr_ins copy_instance(sptr_ins instance_to_copy);


		//=======================
		// RENDERING
		//=======================
		void render_flag(shader& shader, inst_flag_type flag);
		void render_models(shader& shader);

		//=======================
		// LIGHTS
		//=======================
		void generate_lights_buffers();
		void generate_lightspace_matrices();
		void bind_lights_ssbo();
		size_t get_free_spot_light_idx();


	private:
		friend class data::scene_serializer;
		friend class editor::lightbaker;
		friend class scene_saver;

		std::vector<sptr_mdl> _models;
		std::vector<sptr_ins> _instances;

		ssbo _lights_ssbo;
		std::bitset<MAX_SPOT_LIGHTS> _free_spot_lights;
	};

}