#include "skybox.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

float skybox_vertices[] = {
	// positions          
	-1.0f,  1.0f, -1.0f,
	-1.0f, -1.0f, -1.0f,
	 1.0f, -1.0f, -1.0f,
	 1.0f, -1.0f, -1.0f,
	 1.0f,  1.0f, -1.0f,
	-1.0f,  1.0f, -1.0f,

	-1.0f, -1.0f,  1.0f,
	-1.0f, -1.0f, -1.0f,
	-1.0f,  1.0f, -1.0f,
	-1.0f,  1.0f, -1.0f,
	-1.0f,  1.0f,  1.0f,
	-1.0f, -1.0f,  1.0f,

	 1.0f, -1.0f, -1.0f,
	 1.0f, -1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f, -1.0f,
	 1.0f, -1.0f, -1.0f,

	-1.0f, -1.0f,  1.0f,
	-1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f, -1.0f,  1.0f,
	-1.0f, -1.0f,  1.0f,

	-1.0f,  1.0f, -1.0f,
	 1.0f,  1.0f, -1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	-1.0f,  1.0f,  1.0f,
	-1.0f,  1.0f, -1.0f,

	-1.0f, -1.0f, -1.0f,
	-1.0f, -1.0f,  1.0f,
	 1.0f, -1.0f, -1.0f,
	 1.0f, -1.0f, -1.0f,
	-1.0f, -1.0f,  1.0f,
	 1.0f, -1.0f,  1.0f
};

namespace feng {

	skybox::skybox(shader* skybox_shader, const std::vector<std::string>& faces)
		: _shader(skybox_shader), cubemap(faces) {
		_vertexarray.generate();
		_vertexarray.bind();

		_arraybuffer.generate();
		_arraybuffer.bind();
		_arraybuffer.buffer_data(sizeof(skybox_vertices), skybox_vertices, GL_STATIC_DRAW);

		_vertexarray.set_attrib_pointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
		
		_shader->activate();
		_shader->set_int("skybox_texture", 0);
	}

	void skybox::render(glm::mat4 view_matrix){
		glDepthFunc(GL_LEQUAL);
		_shader->activate();
		_shader->set_mat4("sb_view", glm::mat4(glm::mat3(view_matrix)));

		_vertexarray.bind();
		glActiveTexture(GL_TEXTURE0);
		cubemap.bind();
		_vertexarray.draw_arrays(GL_TRIANGLES, 36);
		vertexarray::unbind();
		glDepthFunc(GL_LESS);
	}

}