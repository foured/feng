#pragma once

#include "../../utilities/singleton.hpp"
#include "../../utilities/files_in_folder.hpp"
#include "../../graphics/shader.h"
#include "../../graphics/text/font_atlas.h"

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

			shader light_bake;
			shader gaussian_blur_shader;

			shader debug_line_shader;
			shader debug_line_inst_shader;

			void load();
		};

		shaders_set shaders;

		font_atlas univers_condensed_atlas_20;
		font_atlas clacon2_atlas_30;

		assets_manager();

		std::string get_shader(const std::string& file_name);

	private:
		util::files_in_folder _shader_folder;

		void load_fonts();
	};

}