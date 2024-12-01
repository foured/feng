#include <iostream>

#include <glad/glad.h>
#include <stb/stb_image.h>
#include <glm/glm.hpp>

#include "graphics/window.h"
#include "graphics/shader.h"
#include "logging/logging.h"
#include "graphics/texture.h"
#include "io/camera.h"
#include "utilities/utilities.h"
#include "io/input.h"
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
#include "graphics/helpers/texture_quad.hpp"
#include "utilities/uuid.hpp"
#include "logic/world/scene.h"
#include "logic/world/components/model_instance.h"
#include "logic/world/components/line_animator.h"
#include "logic/data_management/assets_manager.h"

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

void check_errors();

int main() {
#ifdef IMPLEMENT_WORK_IN_PROGRESS_CODE
	LOG_WARNING("'IMPLEMENT_WORK_IN_PROGRESS_CODE' was detected!");
#endif

	//========================
	//    CREATING OBJECTS
	//========================

	window win("Feng", 800, 600);

	shader obj_shader("res/shaders/object_batching.vs", "res/shaders/object_batching.fs");
	shader fullscreen_quad_shader("res/shaders/fullscreen_quad.vs", "res/shaders/main_framebuffer.fs");
	shader skybox_shader("res/shaders/skybox.vs", "res/shaders/skybox.fs");
	shader ui_shader("res/shaders/uiobject.vs", "res/shaders/uiobject.fs");
	shader text_shader("res/shaders/text.vs", "res/shaders/text.fs");
	shader depth_shader("res/shaders/depth.vs", "res/shaders/depth.fs");
	shader penumbra_mask_shader("res/shaders/penumbra_mask.vs", "res/shaders/penumbra_mask.fs");

	helpers::texture_quad fullscreen_quad;

	//======================
	//    CREATING SCENE
	//======================

	scene sc1;
	camera cam;
	skybox sb(&skybox_shader, skybox_faces);

	//model vampire("res/models/vampire/dancing_vampire.dae", model_render_type::batched);
	//model backpack("res/models/survival_guitar_backpack/scene.gltf");
	sptr_mdl cube1 = sc1.register_model(primitives::generate_cube_mesh(glm::vec3(1, 0, 0), glm::vec3(0.2)));
	sptr_mdl cube2 = sc1.register_model(primitives::generate_cube_mesh(glm::vec3(1, 1, 1), glm::vec3(0.6)));

	sptr_ins cube1_i1 = sc1.add_instance();
	cube1_i1.get()->add_component<model_instance>(cube1);
	cube1_i1.get()->flags.set(INST_FLAG_RCV_SHADOWS, false);
	sptr_ins cube1_i2 = sc1.copy_instance(cube1_i1);
	cube1_i2.get()->transform.set_position(glm::vec3(2, 3, -2));
	cube1_i2.get()->transform.set_size(glm::vec3(0.5, 3, 0.5));
	//cube1_i2.get()->add_component<line_animator>(glm::vec3(2, 3, -2), glm::vec3(2, 10, -2), 1);

	sptr_ins cube2_i1 = sc1.add_instance();
	cube2_i1.get()->add_component<model_instance>(cube2);
	cube2_i1.get()->flags.set(INST_FLAG_CAST_SHADOWS, false);
	cube2_i1.get()->transform.set_position(glm::vec3(0, -2, 0));
	cube2_i1.get()->transform.set_size(glm::vec3(20, 0.5f, 20));

	DirLight dir_light{
		{ 0.2f, -1.0f, -0.3f },
		glm::vec3(0.1f),
		glm::vec3(0.6f),
		glm::vec3(0.7f)
	};
	PointLight point_lights[MAX_POINT_LIGHTS] { 
		{ 
			glm::vec3(2, 0, 2),
			1,
			0.09f,
			0.032f,
			glm::vec3(0.1f),
			glm::vec3(0.8f),
			glm::vec3(1.0f)
		} 
	};
	SpotLight spot_lights[MAX_SPOT_LIGHTS] {
		{
			cam.position(),
			cam.front(),
			glm::cos(glm::radians(12.5f)),
			glm::cos(glm::radians(17.5f)),
			1,
			0.09f,
			0.032f,
			glm::vec3(0.1f),
			glm::vec3(0.8f),
			glm::vec3(0.5f)
		}
	};

	//model point_light_cube(primitives::generate_cube_mesh(glm::vec3(1), glm::vec3(0)), model_render_type::batched, 
	//	point_lights[0].position, glm::vec3(0.1f));

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

	const uint32_t SHADOW_WIDTH = 4 * 1024, SHADOW_HEIGHT = 4 * 1024;
	framebuffer depth_map_framebuffer(SHADOW_WIDTH, SHADOW_HEIGHT);
	depth_map_framebuffer.bind();
	texture depth_map_texture = depth_map_framebuffer.allocate_and_attach_texture(
		GL_DEPTH_ATTACHMENT, GL_NEAREST, GL_NEAREST, GL_CLAMP_TO_BORDER, GL_CLAMP_TO_BORDER, GL_DEPTH_COMPONENT);
	float border_color[] = { 1.0, 1.0, 1.0, 1.0 };
	depth_map_texture.set_param_fv(GL_TEXTURE_BORDER_COLOR, border_color);
	depth_map_framebuffer.set_draw_buffer(GL_NONE);
	depth_map_framebuffer.set_read_buffer(GL_NONE);
	framebuffer::check_status();
	depth_map_framebuffer.unbind();
	
	framebuffer penumbra_mask_framebuffer((uint32_t)(window::win_width / 2), (uint32_t)(window::win_height / 2));
	penumbra_mask_framebuffer.bind();
	texture penumbra_mask_texture = penumbra_mask_framebuffer.allocate_and_attach_texture(
		GL_COLOR_ATTACHMENT0, GL_LINEAR, GL_LINEAR, NULL, NULL, GL_RG32F, GL_RED);
	renderbuffer penumbra_mask_renderbuffer = penumbra_mask_framebuffer.allocate_and_attach_renderbuffer(
		GL_DEPTH_COMPONENT24, GL_DEPTH_ATTACHMENT);
	framebuffer::check_status();
	penumbra_mask_framebuffer.unbind();

	ssbo matrices_ssbo;
	matrices_ssbo.allocate(2 * sizeof(glm::mat4), 1);

	glstd::buffer_structure dirlight_buffer_structure;
	dirlight_buffer_structure.add_elements<glm::vec3, glm::vec3, glm::vec3, glm::vec3>();
	glstd::buffer_structure spotlight_buffer_structure;
	spotlight_buffer_structure.add_elements<glm::vec3, glm::vec3, float, float, float, float, float, glm::vec3, glm::vec3, glm::vec3>();
	glstd::buffer_structure pointlight_buffer_structure;
	pointlight_buffer_structure.add_elements<glm::vec3, float, float, float, glm::vec3, glm::vec3, glm::vec3>();

	glstd::buffer_structure lighs_final_buffer_structure;
	lighs_final_buffer_structure.add_struct(dirlight_buffer_structure);
	lighs_final_buffer_structure.add_element<int>();
	lighs_final_buffer_structure.add_struct(spotlight_buffer_structure);
	lighs_final_buffer_structure.add_element<int>();
	lighs_final_buffer_structure.add_struct(pointlight_buffer_structure);
	ssbo lights_ssbo;
	lights_ssbo.allocate(lighs_final_buffer_structure, 2);

	//============
	//    TEXT
	//============

	FT_Library ft;
	if (FT_Init_FreeType(&ft))
	{
		std::cout << "Error to init FreeType Library" << std::endl;
		return -1;
	}

	font_atlas atlas1("res/fonts/UniversCondensed.ttf", ft, 15);
	font_atlas atlas2("res/fonts/clacon2.ttf", ft, 30);
	text_renderer text1(atlas1), text2(atlas2);

	FT_Done_FreeType(ft);

	//==========
	//    UI
	//==========

	ui::ui ui(ui_shader);
	auto layer1 = ui.create_layer();
	layer1->support_input = true;

	auto square1 = ui.create_model(primitives2d::generate_square_mesh(penumbra_mask_texture));
	auto inst1 = ui.add_instance(layer1, square1, glm::vec2(0), glm::vec2(0.5f));
	inst1->uitransform.set_anchor(ui::anchor::BOTTOM_RIGHT);
	inst1->uitransform.set_size_pix({ 200, 200 });
	inst1->uitransform.set_pos_pix({ -200, 200 });
	
	//auto square2 = ui.create_model(primitives2d::generate_square_mesh({ 1, 1, 1 }));
	//auto inst2 = ui.add_instance(layer1, square2);
	//inst2->uitransform.set_size_pix({ 200, 20 });
	//inst2->uitransform.set_anchor(ui::anchor::BOTTOM_RIGHT);
	//inst2->uitransform.set_pos_pix({ -200, 430 });
	//
	//auto square3 = ui.create_model(primitives2d::generate_square_mesh({ 0, 0, 0 }));
	//auto inst3 = ui.add_instance(layer1, square3);
	//inst3->uitransform.set_size_pix({ 10, 18 });
	//
	//ui::slider& slider1 = inst2->add_component<ui::slider>(inst3.get());

	text_batcher tb;

	//============================
	//    PREPARATIONS TO LOOP
	//============================
	
	glm::mat4 dir_lightspace_matrix = dir_light.generate_lightspace_matrix();

	bool is_spot_light_working = false;
	ui.start();
	sc1.start();
	double time = 0;
	uint64_t frames_count = 0, fps = 0;
	while (!win.should_close())
	{
		utilities::update_delta_time();
		cam.move();
		win.process_input();
		ui.update();
		sc1.update();

		//==================
		//    SETUP DATA
		//==================

		if (input::get_key_down(GLFW_KEY_F)) is_spot_light_working = !is_spot_light_working;
		//if (input::get_key_down(GLFW_KEY_M)) depth_map_texture.save_to_png("res/shadowmap.png");

		//pcss_sw = slider1.get_value() * 900 + 100;

		glm::mat4 model(1.0f);
		model = glm::scale(model, glm::vec3(0.2f));
		glm::mat4 projection;
		projection = glm::perspective(
			glm::radians(45.0f), (float)window::win_width / (float)window::win_height, 0.01f, 100.0f);

		matrices_ssbo.start_block();
		matrices_ssbo.add_element<glm::mat4>((glm::mat4*)glm::value_ptr(projection));
		matrices_ssbo.add_element<glm::mat4>((glm::mat4*)value_ptr(cam.get_view_matrix()));
		matrices_ssbo.end_block();

		spot_lights[0].position = cam.position();
		spot_lights[0].direction = cam.front();

		int32_t no_spotlights = MAX_SPOT_LIGHTS, no_pointlights = MAX_POINT_LIGHTS;
		lights_ssbo.start_block();
		lights_ssbo.add_structure(dirlight_buffer_structure, &dir_light);
		lights_ssbo.add_element(&no_spotlights);
		lights_ssbo.add_structure(spotlight_buffer_structure, &spot_lights[0]);
		lights_ssbo.add_element(&no_pointlights);
		lights_ssbo.add_structure(pointlight_buffer_structure, &point_lights[0]);
		lights_ssbo.end_block();

		//=================
		//    RENDERING
		//=================

		//shadow preparations
		depth_map_framebuffer.set_viewport();
		depth_map_framebuffer.bind();
		glEnable(GL_DEPTH_TEST);
		glClear(GL_DEPTH_BUFFER_BIT);
		glCullFace(GL_FRONT);
		
		//shadow pass
		depth_shader.activate();
		depth_shader.set_mat4("lightSpaceMatrix", dir_lightspace_matrix);
		depth_shader.set_mat4("model", model);
		sc1.render_flag(depth_shader, INST_FLAG_CAST_SHADOWS);
		
		glCullFace(GL_BACK);
		depth_map_framebuffer.unbind();

		//penumbra mask pass
		penumbra_mask_framebuffer.set_viewport();
		penumbra_mask_framebuffer.bind();
		glEnable(GL_DEPTH_TEST);
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		penumbra_mask_shader.activate();
		penumbra_mask_shader.set_mat4("lightSpaceMatrix", dir_lightspace_matrix);
		penumbra_mask_shader.set_mat4("model", model);
		penumbra_mask_shader.set_int("shadowMap", 31);
		texture::activate_slot(31);
		depth_map_texture.bind();
		sc1.render_flag(penumbra_mask_shader,  INST_FLAG_RCV_SHADOWS);
		//sc1.render_models(penumbra_mask_shader);
		
		penumbra_mask_framebuffer.unbind();

		//main render pass preparations
		main_framebuffer.set_viewport();
		main_framebuffer.bind();
		glEnable(GL_DEPTH_TEST);
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//main render pass
		obj_shader.activate();
		obj_shader.set_3float("viewPos", cam.position());
		obj_shader.set_int("isSLWorking", is_spot_light_working);
		obj_shader.set_float("material.shininess", 32.0f);
		obj_shader.set_mat4("lightSpaceMatrix", dir_lightspace_matrix);
		obj_shader.set_mat4("model", model);
		obj_shader.set_int("shadowMap", 31);
		texture::activate_slot(31);
		depth_map_texture.bind();
		obj_shader.set_int("penumbraMask", 30);
		texture::activate_slot(30);
		penumbra_mask_texture.bind();
		
		sc1.render_models(obj_shader);

		sb.render(cam.get_view_matrix());

		//==================================
		//    RENDERING THE FINAL RESULT
		//==================================

		//fb.render();

		main_framebuffer.unbind();
		glDisable(GL_DEPTH_TEST);
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		fullscreen_quad.render(fullscreen_quad_shader, main_render_texture);

		//====================
		//    UI RENDERING
		//====================

		ui.render();
		glClear(GL_DEPTH_BUFFER_BIT);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		text_shader.activate();
		float hw = window::win_width / 2.0f;
		float hh = window::win_height / 2.0f;
		text_shader.set_mat4("projection", glm::ortho(-hw, hw, -hh, hh, 0.0f, (float)UCHAR_MAX));

		time += utilities::delta_time();
		frames_count++;
		if (time >= 0.5) {
			fps = frames_count / time;
			time = 0;
			frames_count = 0;
		}

		text2.render(&tb, std::to_string(fps), { -400, 280, 0 }, glm::vec3(0, 1, 0));
		//text1.render(&tb, std::to_string((int)pcss_sw), { 290, -90, 0 }, glm::vec3(1, 0, 0));
		tb.render(text_shader);
		glDisable(GL_BLEND);

		win.swap_buffers();
		glfwPollEvents();
	}

	//fb.delete_buffer();
	return 0;
}

void check_errors() {
	uint32_t gl_error = glGetError();
	if (gl_error != GL_NO_ERROR)
		LOG_ERROR("OpenGL error: '" + std::to_string(gl_error) + "'.");
}