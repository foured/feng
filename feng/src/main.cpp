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

#define PRINT(msg) std::cout << msg << '\n'

using namespace feng;

std::vector<std::string> skybox_faces{
	"res/textures/skyboxes/1/right.jpg",
	"res/textures/skyboxes/1/left.jpg",
	"res/textures/skyboxes/1/top.jpg",
	"res/textures/skyboxes/1/bottom.jpg",
	"res/textures/skyboxes/1/front.jpg",
	"res/textures/skyboxes/1/back.jpg"
};

void check_errors();

int main() {
#ifdef WORK_IN_PROGRESS_CODE
	LOG_WARNING("'WORK_IN_PROGRESS_CODE' was detected!");
#endif

	//========================
	//    CREATING OBJECTS
	//========================

	window win("Feng", 800, 600);

	shader obj_shader("res/shaders/object.vs", "res/shaders/object.fs");
	shader obj_batch_shader("res/shaders/object_batching.vs", "res/shaders/object_batching.fs");
	shader fullscreen_quad_shader("res/shaders/fullscreen_quad.vs", "res/shaders/fullscreen_quad.fs");
	shader skybox_shader("res/shaders/skybox.vs", "res/shaders/skybox.fs");
	shader ui_shader("res/shaders/uiobject.vs", "res/shaders/uiobject.fs");
	shader text_shader("res/shaders/text.vs", "res/shaders/text.fs");
	shader depth_shader("res/shaders/depth.vs", "res/shaders/depth.fs");

	helpers::texture_quad fullscreen_quad;

	//======================
	//    CREATING SCENE
	//======================

	camera cam;
	skybox sb(&skybox_shader, skybox_faces);

	//model backpack("res/models/survival_guitar_backpack/scene.gltf", model_render_type::batched);
	//model brickwall("res/models/brickwall/brickwall.gltf", model_render_type::mesh_by_mesh);
	model cube1(primitives::generate_cube_mesh(glm::vec3(1, 0, 0), glm::vec3(0.2)), model_render_type::batched);
	model cube2(primitives::generate_cube_mesh(glm::vec3(0, 1, 0), glm::vec3(0.6)), model_render_type::batched, 
		glm::vec3(0, -2, 0), glm::vec3(20, 0.5f, 20));
	cube1.add_instance(glm::vec3(2, 3, 2));

	DirLight dir_light{
		{ -0.2f, -1.0f, -0.3f },
		glm::vec3(0.1f),
		glm::vec3(0.6f),
		glm::vec3(0.7f)
	};
	PointLight point_lights[MAX_POINT_LIGHTS] { 
		{ 
			glm::vec3(1.0f),
			2.0f,
			3.0f,
			4.0f,
			glm::vec3(5.0f),
			glm::vec3(6.0f),
			glm::vec3(7.0f)
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
			glm::vec3(1.0f)
		}
	};

	//===============
	//    BUFFERS
	//===============

	framebuffer main_framebuffer(window::win_width, window::win_height);
	main_framebuffer.bind();
	texture main_render_texture = main_framebuffer.allocate_and_attach_texture(
		GL_COLOR_ATTACHMENT0, GL_LINEAR, GL_LINEAR, NULL, NULL, GL_RGB, GL_UNSIGNED_BYTE);
	renderbuffer main_renderbuffer = main_framebuffer.allocate_and_attach_renderbuffer(
		GL_DEPTH24_STENCIL8, GL_DEPTH_STENCIL_ATTACHMENT);
	framebuffer::check_status();
	main_framebuffer.unbind();

	const uint32_t SHADOW_WIDTH = 2 * 1024, SHADOW_HEIGHT = 2 * 1024;
	framebuffer depth_map_framebuffer(SHADOW_WIDTH, SHADOW_HEIGHT);
	depth_map_framebuffer.bind();
	texture depth_map_texture = depth_map_framebuffer.allocate_and_attach_texture(
		GL_DEPTH_ATTACHMENT, GL_NEAREST, GL_NEAREST, GL_CLAMP_TO_BORDER, GL_CLAMP_TO_BORDER, GL_DEPTH_COMPONENT, GL_FLOAT);
	float border_color[] = { 1.0, 1.0, 1.0, 1.0 };
	depth_map_texture.set_param_fv(GL_TEXTURE_BORDER_COLOR, border_color);
	depth_map_framebuffer.set_draw_buffer(GL_NONE);
	depth_map_framebuffer.set_read_buffer(GL_NONE);
	framebuffer::check_status();
	depth_map_framebuffer.unbind();

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

	font_atlas atlas1("res/fonts/UniversCondensed.ttf", ft, 30);
	font_atlas atlas2("res/fonts/clacon2.ttf", ft, 30);
	text_renderer text1(atlas1), text2(atlas2);

	FT_Done_FreeType(ft);

	//==========
	//    UI
	//==========

	ui::ui ui(ui_shader);
	auto layer1 = ui.create_layer();
	layer1->support_input = true;

	auto square1 = ui.create_model(primitives2d::generate_square_mesh({0, 1, 0}));
	auto inst1 = ui.add_instance(layer1, square1, glm::vec2(0), glm::vec2(0.5f));
	inst1->uitransform.set_size_pix({ 300, 50 });
	inst1->render_order = 2;

	auto square2 = ui.create_model(primitives2d::generate_square_mesh({ 1, 0, 0 }));
	auto inst2 = ui.add_instance(layer1, square2, glm::vec2(0), glm::vec2(0.5f));
	inst2->uitransform.set_size_pix({ 10, 40 });
	inst2->render_order = 1;

	ui::slider& sl = inst1->add_component<ui::slider>(inst2.get());

	text_batcher tb;

	//============================
	//    PREPARATIONS TO LOOP
	//============================

	float border = 5;
	float dlm_near_plane = 0.0f, dlm_far_plane = 7.5f;
	glm::mat4 dir_light_projection = glm::ortho(-border, border, -border, border, dlm_near_plane, dlm_far_plane);
	//glm::vec3 dl_pos = glm::vec3(0, 10, 0);
	glm::vec3 dl_pos = -2.0f * dir_light.direction;
	glm::mat4 dir_light_view = glm::lookAt(dl_pos,
										   glm::vec3(0.0f, 0.0f, 0.0f),
										   glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 dir_lightspace_matrix = dir_light_projection * dir_light_view;

	bool is_spot_light_working = false;
	bool use_normal_mapping = true;
	ui.start();
	double time = 0;
	uint64_t frames_count = 0, fps = 0;
	while (!win.should_close())
	{
		utilities::update_delta_time();
		cam.move();
		win.process_input();
		ui.update();

		//==================
		//    SETUP DATA
		//==================

		if (input::get_key_down(GLFW_KEY_F)) is_spot_light_working = !is_spot_light_working;
		if (input::get_key_down(GLFW_KEY_N)) use_normal_mapping = !use_normal_mapping;
		if (input::get_key_down(GLFW_KEY_E)) inst1->uitransform.set_pos_pix({ 300, 300 });

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

		glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
		depth_map_framebuffer.bind();
		glEnable(GL_DEPTH_TEST);
		glClear(GL_DEPTH_BUFFER_BIT);
		glCullFace(GL_FRONT);
		{
			depth_shader.activate();
			depth_shader.set_mat4("lightSpaceMatrix", dir_lightspace_matrix);
			depth_shader.set_mat4("model", model);
			cube1.render(depth_shader);
			cube2.render(depth_shader);
		}
		glCullFace(GL_BACK);
		depth_map_framebuffer.unbind();

		glViewport(0, 0, window::win_width, window::win_height);
		main_framebuffer.bind();
		glEnable(GL_DEPTH_TEST);
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		obj_batch_shader.activate();
		obj_batch_shader.set_3float("viewPos", cam.position());
		obj_batch_shader.set_int("isSLWorking", is_spot_light_working);
		obj_batch_shader.set_bool("useNormalMapping", use_normal_mapping);
		obj_batch_shader.set_float("material.shininess", 32.0f);
		obj_batch_shader.set_mat4("lightSpaceMatrix", dir_lightspace_matrix);
		obj_batch_shader.set_mat4("model", model);
		obj_batch_shader.set_int("shadowMap", 32);
		texture::activate_slot(32);
		depth_map_texture.bind();

		//backpack.render(obj_batch_shader);
		cube1.render(obj_batch_shader);
		cube2.render(obj_batch_shader);

		sb.render(cam.get_view_matrix());
		
		//====================
		//    UI RENDERING
		//====================

		//ui.render();
		//glClear(GL_DEPTH_BUFFER_BIT);
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
		text1.render(&tb, "normalmapping: " + std::to_string(use_normal_mapping), {-400, 260, 0}, glm::vec3(0, 1, 0));
		//text1.render(&tb, std::to_string(sl.get_value()), glm::vec3(0), glm::vec3(0));
		tb.render(text_shader);
		glDisable(GL_BLEND);

		//=====================
		//    END RENDERING
		//=====================

		//fb.render();

		main_framebuffer.unbind();
		glDisable(GL_DEPTH_TEST);
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		fullscreen_quad.render(fullscreen_quad_shader, main_render_texture);

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