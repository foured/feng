#include <iostream>

#include <glad/glad.h>
#include <stb/stb_image.h>
#include <glm/glm.hpp>

#include "io/camera.h"
#include "io/input.h"

#include "utilities/utilities.h"
#include "utilities/fps_counter.h"

#include "logging/logging.h"

#include "graphics/window.h"
#include "graphics/renderer.h"
#include "graphics/shader.h"
#include "graphics/texture.h"
#include "graphics/model.h"
#include "graphics/model2d.h"
#include "graphics/uimodel.h"
#include "graphics/skybox.h"
#include "graphics/primitives2d.h"
#include "graphics/primitives.h"
#include "graphics/text/text_renderer.h"
#include "graphics/batching/text_batcher.h"
#include "graphics/light/lights.h"
#include "graphics/gl_buffers/framebuffer.hpp"
#include "graphics/gl_buffers/shader_storage.hpp"

#include "graphics/helpers/box_renderer.h"
#include "graphics/helpers/fullscreen_quad.hpp"
#include "graphics/helpers/line_renderer.h"

#include "logic/data_management/assets_manager.h"
#include "logic/data_management/scene_serializer.h"
#include "logic/ui/ui_layer.h"
#include "logic/ui/ui.h"
#include "logic/ui/components/slider.h"
#include "logic/world/scene.h"
#include "logic/world/components/model_instance.h"
#include "logic/world/components/line_animator.h"
#include "logic/world/components/size_animator.h"
#include "logic/world/components/flash_light.h"
#include "logic/world/components/simple_collider.h"
#include "logic/world/components/box_collider.h"
#include "logic/world/components/rigidbody.h"

#include <fecs/core/registry.h>

#include "physics/collision/collider.h"

#include "editor/lightbaker.h"

#define PRINT(msg) std::cout << msg << '\n'

using namespace feng;

std::array<std::string, 6> skybox_faces{
	RESOURCES_PATH"textures/skyboxes/1/right.jpg",
	RESOURCES_PATH"textures/skyboxes/1/left.jpg",
	RESOURCES_PATH"textures/skyboxes/1/top.jpg",
	RESOURCES_PATH"textures/skyboxes/1/bottom.jpg",
	RESOURCES_PATH"textures/skyboxes/1/front.jpg",
	RESOURCES_PATH"textures/skyboxes/1/back.jpg"
};

glm::vec3 random_vec3(float min = -1.0f, float max = 1.0f) {
	static std::mt19937 rng(std::random_device{}());
	std::uniform_real_distribution<float> dist(min, max);
	return glm::vec3(dist(rng), dist(rng), dist(rng));
}

int main() {
#ifdef IMPLEMENT_WORK_IN_PROGRESS_CODE
	LOG_WARNING("'IMPLEMENT_WORK_IN_PROGRESS_CODE' was detected!");
#endif

	//========================
	//    CREATING OBJECTS
	//========================
	timer startup_timer("Startup timer");
	window win("Feng", 1920 - 192, 1080 - 108);

	renderer::get_static_values();

	assets_manager* am = assets_manager::get_instance();
	am->shaders.load();

	shader_storage::add_name_binding("Matrices", 0);
	shader_storage::add_name_binding("Lights", 3);

	helpers::fullscreen_quad fullscreen_quad;

	//======================
	//    CREATING SCENE
	//======================
	scene sc1;
	skybox sb(&am->shaders.skybox_shader, skybox_faces);

	//sptr_mdl backpack_m = sc1.register_model("res/models/survival_guitar_backpack/scene.gltf");
	sptr_mdl cube1 = sc1.register_model(primitives::generate_cube_mesh(glm::vec3(1, 0, 0), glm::vec3(0.2)));
	sptr_mdl plane1 = sc1.register_model(primitives::generate_plane_mesh(glm::vec3(1, 1, 1), glm::vec3(0.6)));
	sptr_mdl light_cube = sc1.register_model(primitives::generate_cube_mesh(glm::vec3(1, 1, 1), glm::vec3(1)));

	//sptr_ins bp_i = sc1.add_instance();
	//bp_i.get()->flags.set(INST_FLAG_RCV_SHADOWS, false);
	//bp_i->add_component<model_instance>(backpack_m);
	//bp_i->transform.set_size(glm::vec3(0.005f));
	//bp_i->transform.set_position(glm::vec3(0.0f, 3.0f, 0.0f));

	sptr_ins cube1_i1 = sc1.add_instance();
	cube1_i1->flags.set(INST_FLAG_RCV_SHADOWS, false);
	cube1_i1->add_component<model_instance>(cube1);
	cube1_i1->add_component<simple_collider>();
	LOG_INFO("cube1_i1 ", cube1_i1->get_uuid_string());

	sptr_ins cube1_i2 = sc1.copy_instance(cube1_i1);
	auto cube1_i2_mi = cube1_i2->try_get_component<model_instance>();
	cube1_i2->flags.set(INST_FLAG_RCV_SHADOWS, false);
	cube1_i2->transform.set_position(glm::vec3(2.7f, 2, -2));
	cube1_i2->transform.set_size(glm::vec3(0.5, 2, 0.5));
	cube1_i2->add_component<box_collider>();
	LOG_INFO("cube1_i2 ", cube1_i2->get_uuid_string());

	sptr_ins cube1_i3 = sc1.copy_instance(cube1_i1);
	cube1_i3->flags.set(INST_FLAG_STATIC, false);
	cube1_i3->transform.set_position(glm::vec3(2, 6, -2));
	cube1_i3->transform.set_rotation(glm::vec3(0, 0, 45));
	auto cube1_i3_mi = cube1_i3->try_get_component<model_instance>();
	cube1_i3->add_component<box_collider>();
	cube1_i3->add_component<rigidbody>(2.0f);
	//cube1_i3->add_component<line_animator>(glm::vec3(1, 4, -1), glm::vec3(1, 4, 10), 5);
	LOG_INFO("cube1_i3 ", cube1_i3->get_uuid_string());

	//for (int i = 0; i < 7; i++) {
	//	sptr_ins random_instance = sc1.copy_instance(cube1_i1);
	//	random_instance->transform.set_position(random_vec3(-20, 20));
	//}

	//bottom
	sptr_ins plane1_i1 = sc1.add_instance();
	auto plane1_i1_mi = plane1_i1->add_component<model_instance>(plane1);
	plane1_i1->add_component<simple_collider>();
	plane1_i1->add_component<box_collider>();
	plane1_i1->flags.set(INST_FLAG_CAST_SHADOWS, false);
	plane1_i1->transform.set_position(glm::vec3(0, -2, -5));
	plane1_i1->transform.set_size(glm::vec3(20, 0.5f, 20));
	//plane1_i1->transform.set_rotation(glm::vec3(20, 0, 0));
	LOG_INFO("plane1_i1 ", plane1_i1->get_uuid_string());

	//sptr_ins light_cube_i1 = sc1.add_instance();
	//light_cube_i1.get()->add_component<model_instance>(light_cube);
	//light_cube_i1.get()->flags.set(INST_FLAG_CAST_SHADOWS, false);
	//light_cube_i1.get()->flags.set(INST_FLAG_RCV_SHADOWS, false);
	//light_cube_i1.get()->transform.set_size(glm::vec3(0.1f));
	//light_cube_i1.get()->transform.set_position(glm::vec3(1.3f, 0, -2));

	sptr_ins flash_light_i = sc1.add_instance();
	flash_light_i->add_component<flash_light>(&sc1);

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

	// uniforms
	am->shaders.obj_shader.add_ubo("Matrices");
	am->shaders.skybox_shader.add_ubo("Matrices");

	am->shaders.obj_shader.add_ubo("Lights");


	glstd::buffer_structure test_bs;

	framebuffer main_framebuffer(window::win_width, window::win_height);
	main_framebuffer.bind();
	texture main_render_texture = main_framebuffer.allocate_and_attach_texture(
		GL_COLOR_ATTACHMENT0, GL_LINEAR, GL_LINEAR, 0, 0, GL_RGB16F, GL_RGB);
	renderbuffer main_renderbuffer = main_framebuffer.allocate_and_attach_renderbuffer(
		GL_DEPTH24_STENCIL8, GL_DEPTH_STENCIL_ATTACHMENT);
	framebuffer::check_status();
	main_framebuffer.unbind();

	auto framebuffer_subscription = window::on_framebuffer_size.subscribe(
		[&](uint16_t width, uint16_t height) {
			main_framebuffer.bind();
			main_framebuffer.width = width;
			main_framebuffer.height = height;
			main_render_texture.delete_buffer();
			main_render_texture = main_framebuffer.allocate_and_attach_texture(
				GL_COLOR_ATTACHMENT0, GL_LINEAR, GL_LINEAR, 0, 0, GL_RGB16F, GL_RGB);
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
	layer1->support_input = false;

	//texture dir_light_shadowmap = sc1.dir_light.get_texture();
	//auto texture_display_model = ui.create_model(primitives2d::generate_square_mesh(dir_light_shadowmap));
	//auto texture_display = ui.add_instance(layer1, texture_display_model); 
	//int32_t a = 300;
	//texture_display->uitransform.set_size_pix({ a, a });
	//texture_display->uitransform.set_anchor(ui::anchor::BOTTOM_RIGHT);
	//texture_display->uitransform.set_pos_pix({ -a, a });

	text_batcher tb;

	//============================
	//    PREPARATIONS TO LOOP
	//============================
	sc1.generate_lightspace_matrices();
	
	sc1.dir_light.lightspace_matrix = sc1.dir_light.generate_custom_relative_lightspace_matrix(sc1.get_bounds(),
		plane1_i1_mi->calculate_bounds());	

	helpers::box_renderer lightspace_box(&am->shaders.debug_line_shader, sc1.dir_light.lightspace_matrix);
	float axis_len = 5.0f;
	helpers::line_renderer x_axis(&am->shaders.debug_line_shader, { glm::vec3(0), glm::vec3(axis_len, 0, 0) });
	helpers::line_renderer y_axis(&am->shaders.debug_line_shader, { glm::vec3(0), glm::vec3(0, axis_len, 0) });
	helpers::line_renderer z_axis(&am->shaders.debug_line_shader, { glm::vec3(0), glm::vec3(0, 0, axis_len) });

	//glm::vec3 d1(2.00507, 2.70203, -3);
	//glm::vec3 d2(2.00507, 2.70203, -1);
	//glm::vec3 d3(2, 2.70711, -1);
	//glm::vec3 d4(2, 2.70711, -3);
	//helpers::line_renderer debug_axis1(&am->shaders.debug_line_shader, { d1, d2 });
	//helpers::line_renderer debug_axis2(&am->shaders.debug_line_shader, { d2, d3 });
	//helpers::line_renderer debug_axis3(&am->shaders.debug_line_shader, { d3, d4 });
	//helpers::line_renderer debug_axis4(&am->shaders.debug_line_shader, { d4, d1 });

	utilities::test_octree_visualiser = std::make_unique<helpers::box_renderer_instanced>(
		&am->shaders.debug_line_inst_shader, aabb(glm::vec3(-0.5f), glm::vec3(0.5f)), 200);

	//glm::quat test_rotation(glm::radians(glm::vec3(0.0f, 45.0f, 0.0f)));
	//aabb test_box(glm::vec3(-3), glm::vec3(3));

	//helpers::box_renderer test_box_renderer(&am->shaders.debug_box_shader, 
	//	cube1_i3_mi->calculate_bounds());

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

		if (input::get_key_down(GLFW_KEY_F)) { 
			is_spot_light_working = !is_spot_light_working; 
		}
		if (input::get_key_down(GLFW_KEY_T)) {
			sc1.main_camera.position = glm::mat3(sc1.model_matrix) * glm::vec3(1.3f, 0, -2);
		}

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
		am->shaders.obj_shader.set_int("shadowMap", renderer::get_shadowmap_texture_slot());
		sc1.dir_light.bind_shadowmap(renderer::get_shadowmap_texture_slot());
		//obj_shader.set_int("penumbraMask", 30);
		for (uint8_t i = 0; i < MAX_POINT_LIGHTS; i++) {
			uint8_t slot = 30 - i;
			am->shaders.obj_shader.set_int("pointLightCube", slot);
			sc1.point_lights[i].bind_shadowmap(slot);
		}
		sc1.render_models(am->shaders.obj_shader);
		sb.render(sc1.main_camera.get_view_matrix());
		//utilities::test_octree_visualiser->update_buffers();
		//utilities::test_octree_visualiser->render(glm::vec3(0.0f, 1.0f, 0.0f), sc1.model_matrix,
		//																	   sc1.main_camera.get_view_matrix(), 
		//																	   sc1.get_projection_matrix());
		//lightspace_box.render(glm::vec3(1.0f, 1.0f, 0.0f), sc1.model_matrix,
		//												   sc1.main_camera.get_view_matrix(),
		//												   sc1.get_projection_matrix());

		glDisable(GL_DEPTH_TEST);
		x_axis.render(glm::vec3(1.0f, 0.0f, 0.0f), sc1.model_matrix,
												   sc1.main_camera.get_view_matrix(),
												   sc1.get_projection_matrix());

		y_axis.render(glm::vec3(0.0f, 1.0f, 0.0f), sc1.model_matrix,
												   sc1.main_camera.get_view_matrix(),
												   sc1.get_projection_matrix());

		z_axis.render(glm::vec3(0.0f, 0.0f, 1.0f), sc1.model_matrix,
												   sc1.main_camera.get_view_matrix(),
												   sc1.get_projection_matrix());

		//debug_axis1.render(glm::vec3(0.0f, 0.0f, 0.0f), sc1.model_matrix,
		//											   sc1.main_camera.get_view_matrix(),
		//											   sc1.get_projection_matrix());
		//
		//debug_axis2.render(glm::vec3(0.0f, 0.0f, 0.0f), sc1.model_matrix,
		//	sc1.main_camera.get_view_matrix(),
		//	sc1.get_projection_matrix());
		//
		//debug_axis3.render(glm::vec3(0.0f, 0.0f, 0.0f), sc1.model_matrix,
		//	sc1.main_camera.get_view_matrix(),
		//	sc1.get_projection_matrix());

		glEnable(GL_DEPTH_TEST);

		utilities::test_octree_visualiser->clear_instances();
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