#include "assets_manager.h"

namespace feng {

	void assets_manager::shaders_set::load() {
		obj_shader = shader("res/shaders/object_batching.vs", "res/shaders/object_batching.fs");
		fullscreen_quad_shader = shader("res/shaders/fullscreen_quad.vs", "res/shaders/main_framebuffer.fs");
		skybox_shader = shader("res/shaders/skybox.vs", "res/shaders/skybox.fs");
		ui_shader = shader("res/shaders/uiobject.vs", "res/shaders/uiobject.fs");
		text_shader = shader("res/shaders/text.vs", "res/shaders/text.fs");
		dirlight_depth_shader = shader("res/shaders/depth/dirlight_depth.vs", "res/shaders/depth/dirlight_depth.fs");
		pointlight_depth_shader = shader("res/shaders/depth/pointlight_depth.vs", "res/shaders/depth/pointlight_depth.fs", { shader_sub_program("res/shaders/depth/pointlight_depth.gs", GL_GEOMETRY_SHADER) }, { });
	}

	assets_manager::assets_manager() 
		: _shader_folder("res/shaders") {
	}

	std::string assets_manager::get_shader(const std::string& file_name) {
		return _shader_folder.read_file(file_name);
	}

}