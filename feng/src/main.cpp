#include <iostream>

#include <glad/glad.h>
#include <stb/stb_image.h>
#include <glm/glm.hpp>

#include "graphics/window.h"
#include "graphics/shader.h"
#include "graphics/texture.h"
#include "io/camera.h"
#include "utilities/utilities.h"
#include "io/input.h"
#include "logging/logging.h"
#include "graphics/model.h"
#include "graphics/gl_buffers/framebuffer.hpp"
#include "graphics/skybox.h"
#include "graphics/model2d.h"
#include "graphics/primitives2d.h"
#include "graphics/uimodel.h"
#include "logic/ui/ui_layer.h"
#include "logic/ui/ui.h"
#include "logic/ui/components/slider.h"
#include "graphics/text/text_renderer.h"
#include "graphics/batching/text_batcher.h"
#include "graphics/light/lights.h"
#include "graphics/gl_buffers/ssbo.hpp"
#include "graphics/primitives.h"
#include "graphics/helpers/fullscreen_quad.hpp"
#include "utilities/uuid.hpp"
#include "logic/world/scene.h"
#include "logic/world/components/model_instance.h"
#include "logic/world/components/line_animator.h"
#include "logic/data_management/assets_manager.h"
#include "logic/data_management/scene_serializer.h"
#include "logic/world/components/flash_light.h"
#include "utilities/fps_counter.h"
#include "graphics/helpers/box_renderer.hpp"

#include "editor/lightbaker.h"

#define PRINT(msg) std::cout << msg << '\n'

using namespace feng;

std::array<std::string, 6> skybox_faces{
	"res/textures/skyboxes/1/right.jpg",
	"res/textures/skyboxes/1/left.jpg",
	"res/textures/skyboxes/1/top.jpg",
	"res/textures/skyboxes/1/bottom.jpg",
	"res/textures/skyboxes/1/front.jpg",
	"res/textures/skyboxes/1/back.jpg"
};

int main() {
#ifdef IMPLEMENT_WORK_IN_PROGRESS_CODE
	LOG_WARNING("'IMPLEMENT_WORK_IN_PROGRESS_CODE' was detected!");
#endif

	//========================
	//    CREATING OBJECTS
	//========================
	timer startup_timer("Startup timer");
	window win("Feng", 1920 - 192, 1080 - 108);

	assets_manager* am = assets_manager::get_instance();
	am->shaders.load();

	//gaussian_blur::get_instance()->generate_buffers();

	helpers::fullscreen_quad fullscreen_quad;

	//======================
	//    CREATING SCENE
	//======================

	scene sc1;
	skybox sb(&am->shaders.skybox_shader, skybox_faces);

	sptr_mdl backpack_m = sc1.register_model("res/models/survival_guitar_backpack/scene.gltf");
	sptr_mdl cube1 = sc1.register_model(primitives::generate_cube_mesh(glm::vec3(1, 0, 0), glm::vec3(0.2)));
	sptr_mdl cube2 = sc1.register_model(primitives::generate_cube_mesh(glm::vec3(1, 1, 1), glm::vec3(0.6)));
	sptr_mdl light_cube = sc1.register_model(primitives::generate_cube_mesh(glm::vec3(1, 1, 1), glm::vec3(1)));

	sptr_ins bp_i = sc1.add_instance();
	bp_i->add_component<model_instance>(backpack_m);
	bp_i->transform.set_size(glm::vec3(0.005f));
	bp_i->transform.set_position(glm::vec3(0.0f, 3.0f, 0.0f));
	LOG_INFO("bp_i: " + bp_i->get_uuid_string());

	sptr_ins cube1_i1 = sc1.add_instance();
	cube1_i1.get()->add_component<model_instance>(cube1);
	LOG_INFO("cube1_i1: " + cube1_i1->get_uuid_string());

	sptr_ins cube1_i2 = sc1.copy_instance(cube1_i1);
	cube1_i2.get()->flags.set(INST_FLAG_RCV_SHADOWS, false);
	cube1_i2.get()->transform.set_position(glm::vec3(2, 2, -2));
	cube1_i2.get()->transform.set_size(glm::vec3(0.5, 10, 0.5));
	LOG_INFO("cube1_i2: " + cube1_i2->get_uuid_string());

	//bottom
	sptr_ins cube2_i1 = sc1.add_instance();
	auto cube2_i1_mi = cube2_i1.get()->add_component<model_instance>(cube2);
	cube2_i1.get()->flags.set(INST_FLAG_CAST_SHADOWS, false);
	cube2_i1.get()->transform.set_position(glm::vec3(0, -2, 0));
	cube2_i1.get()->transform.set_size(glm::vec3(20, 0.5f, 20));

	sptr_ins light_cube_i1 = sc1.add_instance();
	light_cube_i1.get()->add_component<model_instance>(light_cube);
	//light_cube_i1.get()->flags.set(INST_FLAG_CAST_SHADOWS, false);
	//light_cube_i1.get()->flags.set(INST_FLAG_RCV_SHADOWS, false);
	light_cube_i1.get()->transform.set_size(glm::vec3(0.1f));
	light_cube_i1.get()->transform.set_position(glm::vec3(1.3f, 0, -2));
	LOG_INFO("light_cube_i1: " + light_cube_i1->get_uuid_string());

	sptr_ins flash_light_i = sc1.add_instance();
	flash_light_i.get()->add_component<flash_light>(&sc1);

	sc1.dir_light = dir_light(glm::vec3(0.2f, -1.0f, -0.3f), glm::vec3(0.1f), glm::vec3(0.6f), glm::vec3(0.7f), 4 * 1024);
	sc1.point_lights[0] = point_light{
		//light_cube_i1.get()->transform.get_position(),
		glm::vec3(1.3f, 0, -2),
		1.0f, 0.09f, 0.032f,
		glm::vec3(0.1f), glm::vec3(0.8f), glm::vec3(1.0f)
	};

	//===============
	//    BUFFERS
	//===============

	framebuffer main_framebuffer(window::win_width, window::win_height);
	main_framebuffer.bind();
	texture main_render_texture = main_framebuffer.allocate_and_attach_texture(
		GL_COLOR_ATTACHMENT0, GL_LINEAR, GL_LINEAR, NULL, NULL, GL_RGB16F, GL_RGB);
	renderbuffer main_renderbuffer = main_framebuffer.allocate_and_attach_renderbuffer(
		GL_DEPTH24_STENCIL8, GL_DEPTH_STENCIL_ATTACHMENT);
	framebuffer::check_status();
	main_framebuffer.unbind();

	auto framebuffer_subscription = window::on_framebuffer_size.subscribe(
		[&](uint16_t width, uint16_t height) {
			main_framebuffer.bind();
			main_framebuffer.width = width;
			main_framebuffer.height = height;
			main_render_texture.del();
			main_render_texture = main_framebuffer.allocate_and_attach_texture(
				GL_COLOR_ATTACHMENT0, GL_LINEAR, GL_LINEAR, NULL, NULL, GL_RGB16F, GL_RGB);
			main_renderbuffer.bind();
			main_renderbuffer.renderbuffer_storage(GL_DEPTH24_STENCIL8, width, height);
			framebuffer::check_status();
			main_framebuffer.unbind();
		});

	sc1.generate_matrices_buffers();
	sc1.generate_lights_buffers();

	//============
	//    TEXT
	//============

	text_renderer text1(am->univers_condensed_atlas_20);
	text_renderer text2(am->clacon2_atlas_30);

	//==========
	//    UI
	//==========

	ui::ui ui(am->shaders.ui_shader);
	auto layer1 = ui.create_layer();
	layer1->support_input = true;

	texture dir_light_shadowmap = sc1.dir_light.get_texture();
	auto texture_display_model = ui.create_model(primitives2d::generate_square_mesh(dir_light_shadowmap));
	auto texture_display = ui.add_instance(layer1, texture_display_model);
	int32_t a = 300;
	texture_display->uitransform.set_size_pix({a, a});
	texture_display->uitransform.set_anchor(ui::anchor::BOTTOM_RIGHT);
	texture_display->uitransform.set_pos_pix({-a, a});

	text_batcher tb;

	//============================
	//    PREPARATIONS TO LOOP
	//============================

	//LOG_INFO(std::to_string(std::filesystem::file_size("scene1.fstp")));

	//editor::lightbaker::bake(&sc1, "scene1.fstp");
	//data::scene_serializer::serialize_models(&sc1, "pack1.fmp");
	//data::scene_serializer::serialize(&sc1, "scene1.fsp");

	//data::scene_serializer::deserialize_models(&sc1, "pack1.fmp");
	//data::scene_serializer::deserialize(&sc1, "scene1.fsp");
	//data::scene_serializer::deserialize_baked_light(&sc1, "scene1.fstp");

	sc1.generate_lightspace_matrices();
	
	sc1.dir_light.lightspace_matrix = sc1.dir_light.generate_custom_relative_lightspace_matrix(sc1.get_bounds(),
		cube2_i1_mi->calculate_bounds(), sc1.model_matrix);

	glm::mat4 floor_lml = sc1.dir_light.generate_custom_lightspace_matrix(cube2_i1_mi->calculate_bounds(), sc1.model_matrix);

	helpers::box_renderer light_view_box(&am->shaders.debug_box_shader, sc1.dir_light.lightspace_matrix);
	helpers::box_renderer floor_lml_box(&am->shaders.debug_box_shader, floor_lml);
	helpers::box_renderer floor_bounds(&am->shaders.debug_box_shader, cube2_i1_mi->calculate_bounds().scale(sc1.model_matrix));

	bool is_spot_light_working = false;
	ui.start();
	sc1.start();
	fps_counter counter;
	startup_timer.stop();
	while (!win.should_close())
	{
		utilities::update_delta_time();
		counter.update();
		win.process_input();
		ui.update();
		sc1.update();

		//==================
		//    SETUP DATA
		//==================

		if (input::get_key_down(GLFW_KEY_F)) is_spot_light_working = !is_spot_light_working;
		if (input::get_key_down(GLFW_KEY_T)) 
			//sc1.main_camera.position = glm::mat3(sc1.model_matrix) * light_cube_i1->transform.get_position();
			sc1.main_camera.position = glm::mat3(sc1.model_matrix) * glm::vec3(1.3f, 0, -2);

		sc1.bind_matrices_ssbo();
		sc1.bind_lights_ssbo();

		//=================
		//    RENDERING
		//=================

		//shadow preparations
		sc1.dir_light.full_render_preparations(am->shaders.dirlight_depth_shader, sc1.model_matrix);
		sc1.render_flag(am->shaders.dirlight_depth_shader, INST_FLAG_CAST_SHADOWS);
		sc1.dir_light.render_cleanup();
		

		//for (auto& pl : sc1.point_lights) {
		//	pl.render_preparations(am->shaders.pointlight_depth_shader);
		//	sc1.render_flag(am->shaders.pointlight_depth_shader, INST_FLAG_CAST_SHADOWS);
		//	pl.render_cleanup();
		//}

		//main render pass preparations
		main_framebuffer.set_viewport();
		main_framebuffer.bind();
		glEnable(GL_DEPTH_TEST);
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//main render pass
		am->shaders.obj_shader.activate();
		am->shaders.obj_shader.set_3float("viewPos", sc1.main_camera.position);
		am->shaders.obj_shader.set_int("isSLWorking", is_spot_light_working);
		am->shaders.obj_shader.set_float("material.shininess", 32.0f);
		am->shaders.obj_shader.set_mat4("lightSpaceMatrix", sc1.dir_light.lightspace_matrix);
		am->shaders.obj_shader.set_mat4("model", sc1.model_matrix);
		am->shaders.obj_shader.set_int("shadowMap", 31);
		sc1.dir_light.bind_shadowmap();
		//obj_shader.set_int("penumbraMask", 30);
		for (uint8_t i = 0; i < MAX_POINT_LIGHTS; i++) {
			uint8_t slot = 30 - i;
			am->shaders.obj_shader.set_int("pointLightCube", slot);
			sc1.point_lights[i].bind_shadowmap(slot);
		}
		
		sc1.render_models(am->shaders.obj_shader);

		light_view_box.render(glm::vec3(0.0f, 1.0f, 0.0f), sc1.main_camera.get_view_matrix(), sc1.get_projection_matrix());
		floor_bounds.render(glm::vec3(1.0f, 1.0f, 0.0f), sc1.main_camera.get_view_matrix(), sc1.get_projection_matrix());
		floor_lml_box.render(glm::vec3(0.0f, 0.0f, 1.0f), sc1.main_camera.get_view_matrix(), sc1.get_projection_matrix());

		sb.render(sc1.main_camera.get_view_matrix());
		main_framebuffer.unbind();

		//==================================
		//    RENDERING THE FINAL RESULT
		//==================================

		glDisable(GL_DEPTH_TEST);
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		fullscreen_quad.render_framebuffer_shader(am->shaders.fullscreen_quad_shader, main_render_texture);

		//====================
		//    UI RENDERING
		//====================

		ui.render();
		glClear(GL_DEPTH_BUFFER_BIT);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		am->shaders.text_shader.activate();
		float hw = window::win_width / 2.0f;
		float hh = window::win_height / 2.0f;
		am->shaders.text_shader.set_mat4("projection", glm::ortho(-hw, hw, -hh, hh, 0.0f, (float)UCHAR_MAX));

		text1.render(&tb, "FPS: " + std::to_string(counter.fps), {-hw, hh - 15, 0}, glm::vec3(0, 1, 0));
		text1.render(&tb, "time: " + std::to_string(counter.msdt), {-hw, hh - 30, 0}, glm::vec3(0, 1, 0));
		tb.render(am->shaders.text_shader);
		glDisable(GL_BLEND);

		win.swap_buffers();
		glfwPollEvents();
	}

	return 0;
}