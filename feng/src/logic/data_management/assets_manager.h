#pragma once

#include "../../utilities/singleton.hpp"
#include "../../utilities/files_in_folder.hpp"
#include "../../graphics/shader.h"

namespace feng {

	class assets_manager : public util::singleton<assets_manager> {
	public:

		struct shaders_set
		{
			shader obj_shader;
			shader fullscreen_quad_shader;
			shader skybox_shader;
			shader ui_shader;
			shader text_shader;
			shader dirlight_depth_shader;
			shader pointlight_depth_shader;

			void load();
		};

		assets_manager();

		std::string get_shader(const std::string& file_name);

		shaders_set shaders;


	private:
		util::files_in_folder _shader_folder;
	};

}