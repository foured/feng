#pragma once

#include "../vertexarray.h"
#include "../gl_buffers/arraybuffer.hpp"
#include "../shader.h"
#include "../texture.h"

namespace feng::helpers {

	float fullscreen_quad_vertices[] = {
		-1.0f,  1.0f,  0.0f, 1.0f,
		-1.0f, -1.0f,  0.0f, 0.0f,
		 1.0f, -1.0f,  1.0f, 0.0f,

		-1.0f,  1.0f,  0.0f, 1.0f,
		 1.0f, -1.0f,  1.0f, 0.0f,
		 1.0f,  1.0f,  1.0f, 1.0f
	};

	class texture_quad {
	public:
		texture_quad() {
			_vertex_array.generate();
			_vertex_array.bind();

			_vertex_buffer.generate();
			_vertex_buffer.bind();
			_vertex_buffer.buffer_data(sizeof(fullscreen_quad_vertices), fullscreen_quad_vertices, GL_STATIC_DRAW);

			_vertex_array.set_attrib_pointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
			_vertex_array.set_attrib_pointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (2 * sizeof(float)));
		}

		void render(shader& shader, texture& tex) {
			shader.activate();
			shader.set_int("quad_texture", 0);
			texture::activate_slot(0);
			tex.bind();
			_vertex_array.bind();
			_vertex_array.draw_arrays(GL_TRIANGLES, 6);
			vertexarray::unbind();
		}

	private:
		vertexarray _vertex_array;
		arraybuffer _vertex_buffer;
	};

}