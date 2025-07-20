#include "assets_manager.h"

#include <ft2build.h>
#include FT_FREETYPE_H  

namespace feng {

	void assets_manager::shaders_set::load() {
		obj_shader = shader(RESOURCES_PATH"shaders/object_batching.vert", RESOURCES_PATH"shaders/object_batching.frag");
		fullscreen_quad_shader = shader(RESOURCES_PATH"shaders/fullscreen_quad.vert", RESOURCES_PATH"shaders/main_framebuffer.frag");
		skybox_shader = shader(RESOURCES_PATH"shaders/skybox.vert", RESOURCES_PATH"shaders/skybox.frag");
		ui_shader = shader(RESOURCES_PATH"shaders/uiobject.vert", RESOURCES_PATH"shaders/uiobject.frag");
		text_shader = shader(RESOURCES_PATH"shaders/text.vert", RESOURCES_PATH"shaders/text.frag");
		dirlight_depth_shader = shader(RESOURCES_PATH"shaders/depth/dirlight_depth.vert", RESOURCES_PATH"shaders/depth/dirlight_depth.frag");
		pointlight_depth_shader = shader(RESOURCES_PATH"shaders/depth/pointlight_depth.vert", RESOURCES_PATH"shaders/depth/pointlight_depth.frag", { shader_sub_program(RESOURCES_PATH"shaders/depth/pointlight_depth.geom", GL_GEOMETRY_SHADER) }, { });
		light_bake = shader(RESOURCES_PATH"shaders/light_bake.vert", RESOURCES_PATH"shaders/light_bake.frag");
		debug_line_shader = shader(RESOURCES_PATH"shaders/debug_line.vert", RESOURCES_PATH"shaders/debug_line.frag");
		debug_line_inst_shader = shader(RESOURCES_PATH"shaders/debug_line_instanced.vert", RESOURCES_PATH"shaders/debug_line.frag");
		gaussian_blur_shader = shader(RESOURCES_PATH"shaders/fullscreen_quad.vert", RESOURCES_PATH"shaders/gaussian.frag");
	}

	assets_manager::assets_manager() 
		: _shader_folder(RESOURCES_PATH"shaders") {
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

		univers_condensed_atlas_20 = font_atlas(RESOURCES_PATH"fonts/UniversCondensed.ttf", ft, 20);
		clacon2_atlas_30 = font_atlas(RESOURCES_PATH"fonts/clacon2.ttf", ft, 30);

		FT_Done_FreeType(ft);
	}

}