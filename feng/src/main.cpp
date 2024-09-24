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
#include "graphics/gl_buffers/uniformbuffer.hpp"
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

int32_t get_uniform_block_size(shader& s, const char* block_name);

struct A {
	int iv = 1;
	float fv = 0.5f;
};

int main() {
	window win("Feng", 800, 600);

	shader obj_shader("res/shaders/object.vs", "res/shaders/object.fs");
	shader obj_batch_shader("res/shaders/object_batching.vs", "res/shaders/object_batching.fs");
	shader framebuffer_shader("res/shaders/framebuffer.vs", "res/shaders/framebuffer.fs");
	shader skybox_shader("res/shaders/skybox.vs", "res/shaders/skybox.fs");
	shader ui_shader("res/shaders/uiobject.vs", "res/shaders/uiobject.fs");
	shader text_shader("res/shaders/text.vs", "res/shaders/text.fs");

	camera cam;
	framebuffer fb(&framebuffer_shader);
	skybox sb(&skybox_shader, skybox_faces);

	model backpack("res/models/survival_guitar_backpack/scene.gltf", model_render_type::mesh_by_mesh);
	//model brickwall("res/models/brickwall/brickwall.gltf", model_render_type::mesh_by_mesh);

	DirLight dir_light{
		{ -0.2f, -1.0f, -0.3f },
		glm::vec3(0.5f),
		glm::vec3(0.5f),
		glm::vec3(1.0f)
	};
	PointLight point_lights[MAX_POINT_LIGHTS] { 
		{ 
			glm::vec3(1.0f),
			1.0f,
			1.0f,
			1.0f,
			glm::vec3(1.0f),
			glm::vec3(1.0f),
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
			glm::vec3(1.0f)
		}
	};

	obj_shader.set_ubo_index("Matrices", 0);
	obj_shader.set_ubo_index("Lights", 1);
	obj_batch_shader.set_ubo_index("Matrices", 0);
	skybox_shader.set_ubo_index("Matrices", 0);

	ssbo matrices_ssbo;
	matrices_ssbo.allocate(2 * sizeof(glm::mat4), 3);
	ssbo dirlight_ssbo;
	dirlight_ssbo.allocate(sizeof(DirLight), 2);

	// Broken UBOs: 
	
	//uniformbuffer_struct dir_light_ubo_struct;
	//dir_light_ubo_struct.add_elements<glm::vec3, glm::vec3, glm::vec3, glm::vec3>();
	//uniformbuffer_struct point_light_ubo_struct;
	//point_light_ubo_struct.add_elements<glm::vec3, float, float, float, glm::vec3, glm::vec3, glm::vec3>();
	//uniformbuffer_struct spot_light_ubo_struct;
	//spot_light_ubo_struct.add_elements<glm::vec3, glm::vec3, float, float, float, float, float, glm::vec3, glm::vec3, glm::vec3>();
	//
	//uniformbuffer_pack matrices_ubuffer_pack;
	//matrices_ubuffer_pack.add_element<glm::mat4>();
	//matrices_ubuffer_pack.add_element<glm::mat4>();
	//uniformbuffer matrices_ubuffer;
	//matrices_ubuffer.fast_setup(0, matrices_ubuffer_pack);
	//
	//uniformbuffer_pack lights_ubuffer_pack;
	//lights_ubuffer_pack.add_element(dir_light_ubo_struct);
	//lights_ubuffer_pack.add_element<int32_t>();
	//lights_ubuffer_pack.add_array<MAX_POINT_LIGHTS>(point_light_ubo_struct);
	//lights_ubuffer_pack.add_element<int32_t>();
	//lights_ubuffer_pack.add_array<MAX_SPOT_LIGHTS>(spot_light_ubo_struct);
	//uniformbuffer lights_ubuffer;
	//lights_ubuffer.fast_setup(1, lights_ubuffer_pack);

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

		// prepare to render

		fb.set();

		if (input::get_key_down(GLFW_KEY_F)) is_spot_light_working = !is_spot_light_working;
		if (input::get_key_down(GLFW_KEY_N)) use_normal_mapping = !use_normal_mapping;
		if (input::get_key_down(GLFW_KEY_E)) inst1->uitransform.set_pos_pix({ 300, 300 });

		glm::mat4 model(1.0f);
		model = glm::scale(model, glm::vec3(0.01f));
		glm::mat4 projection;
		projection = glm::perspective(
			glm::radians(45.0f), (float)window::win_width / (float)window::win_height, 0.1f, 100.0f);

		dirlight_ssbo.update(0, sizeof(DirLight), &dir_light);
		matrices_ssbo.update(0, sizeof(glm::mat4), glm::value_ptr(projection));
		matrices_ssbo.update(sizeof(glm::mat4), sizeof(glm::mat4), (void*)glm::value_ptr(cam.get_view_matrix()));

		// More UBOs:
		
		//matrices_ubuffer.start_block();
		//matrices_ubuffer.buffer_block_element((glm::mat4*)glm::value_ptr(projection));
		//matrices_ubuffer.buffer_block_element((glm::mat4*)glm::value_ptr(cam.get_view_matrix()));
		//matrices_ubuffer.end_block();
		//
		//lights_ubuffer.start_block();
		//lights_ubuffer.buffer_block_struct(dir_light_ubo_struct, &dir_light);
		//int32_t pls = 0;
		//lights_ubuffer.buffer_block_element(&pls);
		//lights_ubuffer.buffer_block_struct_array(point_light_ubo_struct, &point_lights[0], MAX_POINT_LIGHTS);
		//int32_t sls = 1;
		//lights_ubuffer.buffer_block_element(&sls);
		//lights_ubuffer.buffer_block_struct_array(spot_light_ubo_struct, &spot_lights[0], MAX_SPOT_LIGHTS);
		//lights_ubuffer.end_block();

		// start render

		obj_shader.activate();
		
		obj_shader.set_3float("viewPos", cam.position());
		obj_shader.set_int("isSLWorking", is_spot_light_working);
		obj_shader.set_int("useNormalMapping", use_normal_mapping);
		obj_shader.set_float("material.shininess", 32.0f);
		
		//obj_shader.set_3float("dirLight.direction", -0.2f, -1.0f, -0.3f);
		//obj_shader.set_3float("dirLight.ambient", glm::vec3(0.5f));
		//obj_shader.set_3float("dirLight.diffuse", glm::vec3(0.5f));
		//obj_shader.set_3float("dirLight.specular", glm::vec3(1.0f));

		//obj_shader.set_3float("pointLights[0].position", 1.2f, 1.0f, 2.0f);
		//obj_shader.set_3float("pointLights[0].ambient", glm::vec3(0.2f));
		//obj_shader.set_3float("pointLights[0].diffuse", glm::vec3(0.5f));
		//obj_shader.set_3float("pointLights[0].specular", glm::vec3(1.0f));
		//obj_shader.set_float("pointLights[0].constant", 1.0f);
		//obj_shader.set_float("pointLights[0].linear", 0.09f);
		//obj_shader.set_float("pointLights[0].quadratic", 0.032f);

		//obj_shader.set_3float("spotLights[0].position", cam.position());
		//obj_shader.set_3float("spotLights[0].direction", cam.front());
		//obj_shader.set_float("spotLights[0].cutOff", glm::cos(glm::radians(12.5f)));
		//obj_shader.set_float("spotLights[0].outerCutOff", glm::cos(glm::radians(17.5f)));
		//obj_shader.set_3float("spotLights[0].ambient", glm::vec3(0.1));
		//obj_shader.set_3float("spotLights[0].diffuse", glm::vec3(0.8f));
		//obj_shader.set_3float("spotLights[0].specular", glm::vec3(1.0f));
		//obj_shader.set_float("spotLights[0].constant", 1.0f);
		//obj_shader.set_float("spotLights[0].linear", 0.09f);
		//obj_shader.set_float("spotLights[0].quadratic", 0.032f);
		
		obj_shader.set_mat4("model", model);

		backpack.render(obj_shader);

		sb.render(cam.get_view_matrix());
		
		// render ui

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

		// end render

		fb.render();

		win.swap_buffers();
		glfwPollEvents();
	}

	fb.delete_buffer();
	return 0;
}

int32_t get_uniform_block_size(shader& s, const char* block_name) {
	uint32_t ubi = glGetUniformBlockIndex(s.id(), block_name);
	int32_t ubs;
	glGetActiveUniformBlockiv(s.id(), ubi,
		GL_UNIFORM_BLOCK_DATA_SIZE,
		&ubs);
	return ubs;
}