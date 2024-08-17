#include "mesh2d.h"

namespace feng {

	mesh2d::mesh2d(std::vector<vertex2d>vertices, std::vector<uint32_t> indices, texture texture)
		: _vertices(vertices), _indices(indices), _texture(texture), _has_texture(true) {
		setup();
	}

	mesh2d::mesh2d(std::vector<vertex2d>vertices, std::vector<uint32_t> indices, glm::vec3 color)
		: _vertices(vertices), _indices(indices), _color(color), _has_texture(false) {
		setup();
	}


	void mesh2d::setup() {
		vertex_array.generate();
		vertex_array.bind();

		_elementbuffer.generate();
		_elementbuffer.bind();
		_elementbuffer.buffer_data(_indices.size() * sizeof(uint32_t), &_indices[0], GL_STATIC_DRAW);

		_arraybuffer.generate();
		_arraybuffer.bind();
		_arraybuffer.buffer_data(_vertices.size() * sizeof(vertex2d), &_vertices[0], GL_STATIC_DRAW);

		vertex_array.set_attrib_pointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex2d), 0);
		vertex_array.set_attrib_pointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(vertex2d), (3 * sizeof(float)));

		vertexarray::unbind();
	}

	void mesh2d::render(shader& shader_2d, uint32_t no_instances) {
		shader_2d.set_int("has_tex", _has_texture);
		if (_has_texture) {
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, _texture.id());
		}
		else {
			shader_2d.set_3float("color", _color);
		}
		vertex_array.bind();
		vertex_array.draw_elements_inctanced(GL_TRIANGLES, _indices.size(), GL_UNSIGNED_INT, 0, no_instances);

		vertexarray::unbind();
		glActiveTexture(GL_TEXTURE0);
	}

	std::vector<vertex2d> mesh2d::get_vertices() {
		return _vertices;
	}

}