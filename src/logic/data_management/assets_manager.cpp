#include "assets_manager.h"

#include <ft2build.h>
#include FT_FREETYPE_H  

namespace feng {

	void assets_manager::shaders_set::load() {
		obj_shader = shader("res/shaders/object_batching.vs", "res/shaders/object_batching.fs");
		fullscreen_quad_shader = shader("res/shaders/fullscreen_quad.vs", "res/shaders/main_framebuffer.fs");
		skybox_shader = shader("res/shaders/skybox.vs", "res/shaders/skybox.fs");
		ui_shader = shader("res/shaders/uiobject.vs", "res/shaders/uiobject.fs");
		text_shader = shader("res/shaders/text.vs", "res/shaders/text.fs");
		dirlight_depth_shader = shader("res/shaders/depth/dirlight_depth.vs", "res/shaders/depth/dirlight_depth.fs");
		pointlight_depth_shader = shader("res/shaders/depth/pointlight_depth.vs", "res/shaders/depth/pointlight_depth.fs", { shader_sub_program("res/shaders/depth/pointlight_depth.gs", GL_GEOMETRY_SHADER) }, { });
		light_bake = shader("res/shaders/light_bake.vs", "res/shaders/light_bake.fs");
		debug_line_shader = shader("res/shaders/debug_line.vs", "res/shaders/debug_line.fs");
		debug_line_inst_shader = shader("res/shaders/debug_line_instanced.vs", "res/shaders/debug_line.fs");
		gaussian_blur_shader = shader("res/shaders/fullscreen_quad.vs", "res/shaders/gaussian.fs");
	}

	assets_manager::assets_manager() 
		: _shader_folder("res/shaders") {
		load_fonts();
	}

	std::string assets_manager::get_shader(const std::string& file_name) {
		return _shader_folder.read_file(file_name);
	}

	void assets_manager::load_fonts() {
		FT_Library ft;
		if (FT_Init_FreeType(&ft))
		{
			THROW_ERROR("Error to init FreeType Library");
		}

		univers_condensed_atlas_20 = font_atlas("res/fonts/UniversCondensed.ttf", ft, 20);
		clacon2_atlas_30 = font_atlas("res/fonts/clacon2.ttf", ft, 30);

		FT_Done_FreeType(ft);
	}

}