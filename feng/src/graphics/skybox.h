#pragma once

#include "shader.h"
#include "cubemap.h"

#include "vertexarray.h"
#include "gl_buffers/arraybuffer.hpp"

namespace feng {

	class skybox {
	public:
		skybox(shader* skybox_shader, const std::vector<std::string>& faces);

		void render(glm::mat4 view_matrix);

	public:
		cubemap cubemap;

	private:
		shader* _shader;
		vertexarray _vertexarray;
		arraybuffer _arraybuffer;
	};

}