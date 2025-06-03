#include "mesh.h"

#include <glad/glad.h>
//#include <GLFW/glfw3.h>
#include "../logic/data_management/files.h"
#include "../logic/world/scene.h"
#include "../logging/logging.h"

namespace feng {

	mesh::mesh(std::vector<vertex> vertices, std::vector<uint32_t> indices, std::vector<texture> textures)
		: _vertices(vertices), _indices(indices), _textures(textures), _has_textures(true) {
	}

	mesh::mesh(std::vector<vertex> vertices, std::vector<uint32_t> indices, aiColor4D diffuse, aiColor4D specular)
		: _vertices(vertices), _indices(indices), _has_textures(false), _diffuse(diffuse), _specular(specular) {
	}

	void mesh::setup() {
		vertex_array.generate();
		vertex_array.bind();

		_elementbuffer.generate();
		_elementbuffer.bind();
		_elementbuffer.buffer_data(_indices.size() * sizeof(uint32_t), &_indices[0], GL_STATIC_DRAW);

		_arraybuffer.generate();
		_arraybuffer.bind();
		_arraybuffer.buffer_data(_vertices.size() * sizeof(vertex), &_vertices[0], GL_STATIC_DRAW);
		
		vertex_array.set_attrib_pointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), 0);
		vertex_array.set_attrib_pointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (3 * sizeof(float)));
		vertex_array.set_attrib_pointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(vertex), (6 * sizeof(float)));
		vertex_array.set_attrib_pointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (8 * sizeof(float)));

		vertexarray::unbind();
	}

	void mesh::render(shader& shader, uint32_t no_instances) {
		shader.set_int("has_tex", _has_textures);
		if (_has_textures) {
			uint32_t diff_idx = 0, spec_idx = 0, norm_idx = 0;
			for (size_t i = 0; i < _textures.size(); i++)
			{
				glActiveTexture(GL_TEXTURE0 + i);
				std::string name;
				switch (_textures[i].aiTtype()) {
				case aiTextureType_DIFFUSE:
					name = "diffuse" + std::to_string(diff_idx++);
					break;

				case aiTextureType_SPECULAR:
					name = "specular" + std::to_string(spec_idx++);
					break;
				case aiTextureType_NORMALS:
					name = "normal" + std::to_string(norm_idx++);
					break;
				}

				shader.set_int("material." + name, i);
				glBindTexture(GL_TEXTURE_2D, _textures[i].id());
			}
		}
		else {
			shader.set_4float("material.diffuse", _diffuse);
			shader.set_4float("material.specular", _specular);
		}

		vertex_array.bind();
		vertex_array.draw_elements_inctanced(GL_TRIANGLES, _indices.size(), GL_UNSIGNED_INT, 0, no_instances);

		vertexarray::unbind();
		glActiveTexture(GL_TEXTURE0);
	}

	void mesh::serialize(data::wfile* file) {
		file->write_raw(_has_textures);
		if (_has_textures) {
			file->write_raw(_textures.size());
			for (auto& t : _textures) 
				file->write_serializable(&t);
		}
		else {
			file->write_raw(_diffuse);
			file->write_raw(_specular);
		}
		file->write_raw(_indices.size());
		for (const auto i : _indices)
			file->write_raw(i);
		file->write_raw(_vertices.size());
		for (const auto& v : _vertices)
			file->write_raw(v);
	}

	void mesh::deserialize(data::rfile* file, scene* scene) {
		THROW_ERROR("Unimplemented code in mesh::deserialize");
	}

}