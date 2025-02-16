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
#include "logic/data_management/scene_serializer.h"

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
	timer startup_timer("Startup timer");
	window win("Feng", 1920, 1080);

	shader obj_shader("res/shaders/object_batching.vs", "res/shaders/object_batching.fs");
	shader fullscreen_quad_shader("res/shaders/fullscreen_quad.vs", "res/shaders/main_framebuffer.fs");
	shader skybox_shader("res/shaders/skybox.vs", "res/shaders/skybox.fs");
	shader ui_shader("res/shaders/uiobject.vs", "res/shaders/uiobject.fs");
	shader text_shader("res/shaders/text.vs", "res/shaders/text.fs");
	shader dirlight_depth_shader("res/shaders/depth/dirlight_depth.vs", "res/shaders/depth/dirlight_depth.fs");
	shader pointlight_depth_shader("res/shaders/depth/pointlight_depth.vs", "res/shaders/depth/pointlight_depth.fs", { shader_sub_program("res/shaders/depth/pointlight_depth.gs", GL_GEOMETRY_SHADER) }, { });

	helpers::texture_quad fullscreen_quad;

	//======================
	//    CREATING SCENE
	//======================

	scene sc1;
	camera cam;
	skybox sb(&skybox_shader, skybox_faces);

	//sptr_mdl backpack_m = sc1.register_model("res/models/survival_guitar_backpack/scene.gltf");
	sptr_mdl cube1 = sc1.register_model(primitives::generate_cube_mesh(glm::vec3(1, 0, 0), glm::vec3(0.2)));
	sptr_mdl cube2 = sc1.register_model(primitives::generate_cube_mesh(glm::vec3(1, 1, 1), glm::vec3(0.6)));
	sptr_mdl light_cube = sc1.register_model(primitives::generate_cube_mesh(glm::vec3(1, 1, 1), glm::vec3(1)));

	sptr_ins cube1_i1 = sc1.add_instance();
	cube1_i1.get()->add_component<model_instance>(cube1);
	sptr_ins cube1_i2 = sc1.copy_instance(cube1_i1);
	cube1_i2.get()->flags.set(INST_FLAG_RCV_SHADOWS, false);
	cube1_i2.get()->transform.set_position(glm::vec3(2, 2, -2));
	cube1_i2.get()->transform.set_size(glm::vec3(0.5, 3, 0.5));
	//cube1_i2.get()->add_component<line_animator>(glm::vec3(2, 3, -2), glm::vec3(2, 10, -2), 1);

	//bottom
	sptr_ins cube2_i1 = sc1.add_instance();
	cube2_i1.get()->add_component<model_instance>(cube2);
	//cube2_i1.get()->flags.set(INST_FLAG_CAST_SHADOWS, false);
	cube2_i1.get()->transform.set_position(glm::vec3(0, -2, 0));
	cube2_i1.get()->transform.set_size(glm::vec3(20, 0.5f, 20));
	////top
	//sptr_ins cube2_i2 = sc1.copy_instance(cube2_i1);
	//cube2_i2.get()->transform.set_position(glm::vec3(0, 18, 0));
	////right
	//sptr_ins cube2_i3 = sc1.copy_instance(cube2_i1);
	//cube2_i3.get()->transform.set_position(glm::vec3(20, 10, 0));
	//cube2_i3.get()->transform.set_size(glm::vec3(1, 20, 20));
	////left
	//sptr_ins cube2_i4 = sc1.copy_instance(cube2_i1);
	//cube2_i4.get()->transform.set_position(glm::vec3(-20, 10, 0));
	//cube2_i4.get()->transform.set_size(glm::vec3(1, 20, 20));
	////forward
	//sptr_ins cube2_i5 = sc1.copy_instance(cube2_i1);
	//cube2_i5.get()->transform.set_position(glm::vec3(0, 10, -20));
	//cube2_i5.get()->transform.set_size(glm::vec3(20, 20, 1));
	////back
	//sptr_ins cube2_i6 = sc1.copy_instance(cube2_i1);
	//cube2_i6.get()->transform.set_position(glm::vec3(0, 10, 20));
	//cube2_i6.get()->transform.set_size(glm::vec3(20, 20, 1));

	sptr_ins light_cube_i1 = sc1.add_instance();
	light_cube_i1.get()->add_component<model_instance>(light_cube);
	light_cube_i1.get()->flags.set(INST_FLAG_CAST_SHADOWS, false);
	light_cube_i1.get()->flags.set(INST_FLAG_RCV_SHADOWS, false);
	light_cube_i1.get()->transform.set_size(glm::vec3(0.1f));
	light_cube_i1.get()->transform.set_position(glm::vec3(1.3f, 0, -2));

	DirLight dir_light(glm::vec3(0.2f, -1.0f, -0.3f), glm::vec3(0.1f), glm::vec3(0.6f), glm::vec3(0.7f), 2 * 2048);
	PointLight point_lights[MAX_POINT_LIGHTS] { 
		{ 
			light_cube_i1.get()->transform.get_position(),
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
			cam.position,
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

	dir_light.generate_buffers();
	for(auto& pl : point_lights)
		pl.generate_buffers();

	ssbo matrices_ssbo;
	matrices_ssbo.allocate(2 * sizeof(glm::mat4), 1);

	glstd::buffer_structure dirlight_buffer_structure;
	dirlight_buffer_structure.add_elements<glm::vec3, glm::vec3, glm::vec3, glm::vec3>();
	glstd::buffer_structure spotlight_buffer_structure;
	spotlight_buffer_structure.add_elements<glm::vec3, glm::vec3, float, float, float, float, float, glm::vec3, glm::vec3, glm::vec3>();
	glstd::buffer_structure pointlight_buffer_structure;
	pointlight_buffer_structure.add_elements<glm::vec3, float, float, float, glm::vec3, glm::vec3, glm::vec3, float>();

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

	font_atlas atlas1("res/fonts/UniversCondensed.ttf", ft, 20);
	font_atlas atlas2("res/fonts/clacon2.ttf", ft, 30);
	text_renderer text1(atlas1), text2(atlas2);

	FT_Done_FreeType(ft);

	//==========
	//    UI
	//==========

	ui::ui ui(ui_shader);
	auto layer1 = ui.create_layer();
	layer1->support_input = true;

	text_batcher tb;

	//============================
	//    PREPARATIONS TO LOOP
	//============================
	
	dir_light.generate_lightspace_matrix();
	for(auto& pl : point_lights)
		pl.generate_lightspace_matrices();

	data::scene_serializer::serialize(&sc1, "scene1.fsp");

	bool is_spot_light_working = false;
	ui.start();
	sc1.start();
	double time = 0;
	double msdt = 0;
	uint64_t frames_count = 0, fps = 0;
	startup_timer.stop();
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

		
		glm::mat4 model(1.0f);
		model = glm::scale(model, glm::vec3(0.2f));
		glm::mat4 projection;
		projection = glm::perspective(
			glm::radians(45.0f), (float)window::win_width / (float)window::win_height, 0.01f, 100.0f);

		if (input::get_key_down(GLFW_KEY_F)) is_spot_light_working = !is_spot_light_working;
		if (input::get_key_down(GLFW_KEY_T)) 
			cam.position = glm::mat3(model) * light_cube_i1->transform.get_position();

		matrices_ssbo.start_block();
		matrices_ssbo.add_element<glm::mat4>((glm::mat4*)glm::value_ptr(projection));
		matrices_ssbo.add_element<glm::mat4>((glm::mat4*)value_ptr(cam.get_view_matrix()));
		matrices_ssbo.end_block();

		spot_lights[0].position = cam.position;
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
		dir_light.render_preparations();
		dirlight_depth_shader.activate();
		dirlight_depth_shader.set_mat4("lightSpaceMatrix", dir_light.lightspace_matrix);
		dirlight_depth_shader.set_mat4("model", model);
		sc1.render_flag(dirlight_depth_shader, INST_FLAG_CAST_SHADOWS);
		dir_light.render_cleanup();

		for (auto& pl : point_lights) {
			pl.render_preparations(pointlight_depth_shader);
			sc1.render_flag(pointlight_depth_shader, INST_FLAG_CAST_SHADOWS);
			pl.render_cleanup();
		}

		//main render pass preparations
		main_framebuffer.set_viewport();
		main_framebuffer.bind();
		glEnable(GL_DEPTH_TEST);
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//main render pass
		obj_shader.activate();
		obj_shader.set_3float("viewPos", cam.position);
		obj_shader.set_int("isSLWorking", is_spot_light_working);
		obj_shader.set_float("material.shininess", 32.0f);
		obj_shader.set_mat4("lightSpaceMatrix", dir_light.lightspace_matrix);
		obj_shader.set_mat4("model", model);
		obj_shader.set_int("shadowMap", 31);
		dir_light.bind_shadowmap();
		//obj_shader.set_int("penumbraMask", 30);
		for (uint8_t i = 0; i < MAX_POINT_LIGHTS; i++) {
			uint8_t slot = 30 - i;
			obj_shader.set_int("pointLightCube", slot);
			point_lights[i].bind_shadowmap(slot);
		}
		
		sc1.render_models(obj_shader);
		sb.render(cam.get_view_matrix());
		main_framebuffer.unbind();

		//==================================
		//    RENDERING THE FINAL RESULT
		//==================================

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
			msdt = 1000.0 / fps;
			time = 0;
			frames_count = 0;
		}

		text1.render(&tb, "FPS: " + std::to_string(fps), {-hw, hh - 35, 0}, glm::vec3(0, 1, 0));
		text1.render(&tb, "time: " + std::to_string(msdt), {-hw, hh - 50, 0}, glm::vec3(0, 1, 0));
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