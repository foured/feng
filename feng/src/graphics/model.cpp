#include "model.h"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <algorithm>

#include "../logging/logging.h"

namespace feng {

	model::model(std::string filepath, model_render_type render_type) 
		: _render_type(render_type) {
		//START_TIMER();
		load_model(filepath);
		add_instance(glm::vec3(0.0f));

		switch (_render_type)
		{
		case feng::batched:
			batch_meshes();
			for (mesh_batch& batch : _batches)
				batch.setup_data();
			setup_batched();
			break;
		case feng::mesh_by_mesh:
			for (mesh& mesh : _meshes)
				mesh.setup();
			setup_mesh_by_mesh();
			break;
		}

		//setup();

		//LOG_INFO("Model at '" + filepath + "' has " + std::to_string(_meshes.size()) + " meshes.");
	}

	void model::setup_mesh_by_mesh() {
		_pos_array_buffer.generate();
		_pos_array_buffer.bind();
		_pos_array_buffer.buffer_data(MAX_NO_MODEL_INSTANCES * sizeof(glm::vec3), &_positions[0], GL_DYNAMIC_DRAW);

		for (mesh& m : _meshes) {
			m.vertex_array.bind();
			_pos_array_buffer.bind();
			m.vertex_array.set_attrib_pointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), 0, 1);
			vertexarray::unbind();
		}
	}

	void model::setup_batched() {
		_pos_array_buffer.generate();
		_pos_array_buffer.bind();
		_pos_array_buffer.buffer_data(MAX_NO_MODEL_INSTANCES * sizeof(glm::vec3), &_positions[0], GL_DYNAMIC_DRAW);

		for (mesh_batch& batch : _batches) {
			batch.vertex_array.bind();
			_pos_array_buffer.bind();
			batch.vertex_array.set_attrib_pointer(6, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), 0, 1);
			vertexarray::unbind;
		}

	}

	void model::add_instance(glm::vec3 position) {
		_no_instances++;
		_positions.push_back(position);
	}

	void model::load_model(std::string path) {
		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile(path, 
			aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
		{
			LOG_ERROR("ASSIMP error: " + std::string(importer.GetErrorString()));
			return;
		}
		_directory = path.substr(0, path.find_last_of('/'));

		process_node(scene->mRootNode, scene, glm::mat4(1.0f));
	}

	void model::process_node(aiNode* node, const aiScene* scene, glm::mat4 matrix)
	{
		aiMatrix4x4 n_mat_ai = node->mTransformation;
		glm::mat4 n_mat = glm::mat4(
			n_mat_ai.a1, n_mat_ai.a2, n_mat_ai.a3, n_mat_ai.a4,
			n_mat_ai.b1, n_mat_ai.b2, n_mat_ai.b3, n_mat_ai.b4,
			n_mat_ai.c1, n_mat_ai.c2, n_mat_ai.c3, n_mat_ai.c4,
			n_mat_ai.d1, n_mat_ai.d2, n_mat_ai.d3, n_mat_ai.d4
		) * matrix;

		for (size_t i = 0; i < node->mNumMeshes; i++){
			aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
			_meshes.emplace_back(process_mesh(mesh, scene, n_mat));
		}
		for (size_t i = 0; i < node->mNumChildren; i++){
			process_node(node->mChildren[i], scene, n_mat);
		}
	}

	mesh model::process_mesh(aiMesh* mesh, const aiScene* scene, glm::mat4 matrix) {
		std::vector<vertex> vertices;
		std::vector<uint32_t> indices;
		std::vector<texture> textures;

		for (size_t i = 0; i < mesh->mNumVertices; i++) {
			vertex vert;
			glm::vec3 vector;

			vector.x = mesh->mVertices[i].x;
			vector.y = mesh->mVertices[i].y;
			vector.z = mesh->mVertices[i].z;
			vert.position = glm::vec4(vector, 1.0f) * matrix;

			if (mesh->HasNormals()) {
				vector.x = mesh->mNormals[i].x;
				vector.y = mesh->mNormals[i].y;
				vector.z = mesh->mNormals[i].z;
				vert.normal = vector * glm::mat3(matrix);
			}

			glm::vec2 vec(0.0f);
			if (mesh->mTextureCoords[0]) {
				vec.x = mesh->mTextureCoords[0][i].x;
				vec.y = mesh->mTextureCoords[0][i].y;
			}
			vert.tex_coords = vec;

			vertices.push_back(vert);
		}

		for (size_t i = 0; i < mesh->mNumFaces; i++) {
			aiFace face = mesh->mFaces[i];
			for (size_t j = 0; j < face.mNumIndices; j++)
				indices.push_back(face.mIndices[j]);
		}

		if (mesh->mMaterialIndex >= 0) {
			aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
			if (material->GetTextureCount(aiTextureType_DIFFUSE) > 0) {
				std::vector<texture> diffuse_maps
					= load_material_textures(material, aiTextureType_DIFFUSE);
				textures.insert(textures.end(), diffuse_maps.begin(), diffuse_maps.end());
				std::vector<texture> specular_maps
					= load_material_textures(material, aiTextureType_SPECULAR);
				textures.insert(textures.end(), specular_maps.begin(), specular_maps.end());

				return feng::mesh(vertices, indices, textures);
			}
			else {
				aiColor4D diff(1.0f);
				aiGetMaterialColor(material, AI_MATKEY_COLOR_DIFFUSE, &diff);
				aiColor4D spec(1.0f);
				aiGetMaterialColor(material, AI_MATKEY_COLOR_SPECULAR, &spec);

				return feng::mesh(vertices, indices, diff, spec);
			}
		}
	}

	std::vector<texture> model::load_material_textures(aiMaterial* mat, aiTextureType type) {
		std::vector<texture> textures;
		for (size_t i = 0; i < mat->GetTextureCount(type); i++)
		{
			aiString str;
			mat->GetTexture(type, i, &str);
			bool skip = false;
			for (size_t j = 0; j < _textures_loaded.size(); j++)
			{
				if (std::strcmp(_textures_loaded[j].path().data(), str.C_Str()) == 0)
				{
					textures.push_back(_textures_loaded[j]);
					skip = true;
					break;
				}
			}
			if (!skip) {
				texture current_texture(_directory, str.C_Str(), type);
				textures.push_back(current_texture);
				_textures_loaded.push_back(current_texture);
			}
		}
		return textures;
	}

	void model::render(shader& shader) {
		switch (_render_type)
		{
		case feng::batched:
			render_batched(shader);
			break;
		case feng::mesh_by_mesh:
			render_mesh_by_mesh(shader);
			break;
		}
	}

	void model::render_mesh_by_mesh(shader& shader) {
		_pos_array_buffer.bind();
		_pos_array_buffer.buffer_sub_data(0, sizeof(glm::vec3) * _no_instances, &_positions[0]);

		for (mesh& m : _meshes) {
			m.render(shader, _no_instances);
		}
	}

	void model::render_batched(shader& shader) {
		_pos_array_buffer.bind();
		_pos_array_buffer.buffer_sub_data(0, sizeof(glm::vec3) * _no_instances, &_positions[0]);
		for (mesh_batch& batch : _batches) {
			batch.render(shader, _no_instances);
		}
	}

	void model::batch_meshes() {
		_batches.emplace_back();

		for (mesh& mesh : _meshes) {
			bool batched = false;
			for (mesh_batch& batch : _batches) {
				uint16_t unique_textures = 0;
				for (texture& tex : mesh._textures) {
					if (std::find(batch.textures.begin(), batch.textures.end(), tex) == batch.textures.end()) {
						unique_textures++;;
					}
				}

				if ((batch.textures.size() + unique_textures) <= 32) {
					batched = true;
					batch.add_mesh(mesh);
				}
			}

			if (!batched) {
				_batches.emplace_back();
				_batches.back().add_mesh(mesh);
			}
		}

		//std::cout << batches.size() << std::endl;
	}

	void mesh_batch::add_mesh(mesh& mesh) {
		int8_t d_idx = -1, s_idx = -1;
		for (int i = 0; i < textures.size(); i++) {
			for (texture& mtex : mesh._textures) {
				if (mtex == textures[i]) {
					switch (mtex.type())
					{
					case aiTextureType_DIFFUSE:
						d_idx = i;
						break;
					case aiTextureType_SPECULAR:
						s_idx = i;
						break;
					}
				}
			}
		}
		if (d_idx == -1) {
			for (texture& mtex : mesh._textures) {
				if (mtex.type() == aiTextureType_DIFFUSE) {
					d_idx = textures.size();
					textures.push_back(mtex);
					break;
				}
			}
		}
		if (s_idx == -1) {
			for (texture& mtex : mesh._textures) {
				if (mtex.type() == aiTextureType_SPECULAR) {
					s_idx = textures.size();
					textures.push_back(mtex);
					break;
				}
			}
		}

		uint32_t v_offset = asvds.size();

		indices.reserve(mesh._indices.size());
		for (uint32_t v : mesh._indices) {
			indices.emplace_back(v + v_offset);
		}

		int32_t textures_idxs = d_idx | s_idx << 8;
		advanced_static_vertex_data nasvd;
		if (mesh._has_textures) {
			nasvd.diffuse = mesh._diffuse;
			nasvd.specular = mesh._specular;
		}
		nasvd.textures_indexes = textures_idxs;
		asvds.reserve(mesh._vertices.size());
		for (size_t i = 0; i < mesh._vertices.size(); i++) {
			nasvd.vert = mesh._vertices[i];
			asvds.emplace_back(nasvd);
		}
	}

	void mesh_batch::setup_data() {
		vertex_array.generate();
		vertex_array.bind();

		element_buffer.generate();
		element_buffer.bind();
		element_buffer.buffer_data(indices.size() * sizeof(uint32_t), &indices[0], GL_STATIC_DRAW);

		uint32_t s = sizeof(advanced_static_vertex_data);

		array_buffer.generate();
		array_buffer.bind();
		array_buffer.buffer_data(asvds.size() * s, &asvds[0], GL_STATIC_DRAW);

		vertex_array.set_attrib_pointer(0, 3, GL_FLOAT, GL_FALSE, s, 0);
		vertex_array.set_attrib_pointer(1, 3, GL_FLOAT, GL_FALSE, s, (3 * sizeof(float)));
		vertex_array.set_attrib_pointer(2, 2, GL_FLOAT, GL_FALSE, s, (6 * sizeof(float)));
		vertex_array.set_attrib_pointer(3, 4, GL_FLOAT, GL_FALSE, s, (8 * sizeof(float)));
		vertex_array.set_attrib_pointer(4, 4, GL_FLOAT, GL_FALSE, s, (12 * sizeof(float)));
		vertex_array.set_attrib_pointer(5, 1, GL_FLOAT, GL_FALSE, s, (16 * sizeof(float)));

		vertexarray::unbind();
	}

	void mesh_batch::render(shader& shader, uint32_t no_instances) {
		for (size_t i = 0; i < textures.size(); i++) {
			shader.set_int("textures[" + std::to_string(i) + "]", i);
			glActiveTexture(GL_TEXTURE0 + i);
			textures[i].bind();
		}

		vertex_array.bind();
		vertex_array.draw_elements_inctanced(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0, no_instances);

		vertexarray::unbind();
		glActiveTexture(GL_TEXTURE0);
	}

}